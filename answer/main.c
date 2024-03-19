#include "simlib.h"
#include "main.h"






// DEFINITION FROM OURSELVES
// define events
#define EVENT_BUS_ARRIVE 1 // bus arrive at any location
#define EVENT_BUS_DEPARTURE 4 // bus is on the way from any location to another location

#define EVENT_ARRIVAL_TERMINAL_1 5 // passenger arrive at terminal 1. store arrival time, destination
#define EVENT_ARRIVAL_TERMINAL_2 6 // passenger arrive at terminal 2. store arrival time, destination
#define EVENT_ARRIVAL_CAR_RENTAL 7 // passenger arrive at car rental. store arrival time, destination

#define EVENT_DEPARTURE_TERMINAL_1 8 // passenger depart from terminal 1 using bus to car rental
#define EVENT_DEPARTURE_TERMINAL_2 9 // passenger depart from terminal 2 using bus to car rental
#define EVENT_DEPARTURE_CAR_RENTAL 10 // passenger depart from car rental using bus to terminal 1 or terminal 2

#define EVENT_PASSENGER_FINISH_UNLOADING 11 // finish unloading passenger X from bus. this marks the finish of unloading - if this event is called, then that passenger is unloaded.

#define EVENT_PASSENGER_FINISH_LOADING 12 // finish loading passenger X to bus

#define EVENT_END_SIMULATION 13

#define LINE_TERMINAL_1 1 // for list_file. represents the line at terminal 1
#define LINE_TERMINAL_2 2 // for list_file. represents the line at terminal 2
#define LINE_CAR_RENTAL 3 // for list_file. represents the line at car rental

#define LINE_PASSENGER_IN_BUS_TO_TERMINAL_1 4 // for list_file. represents passenger in the bus going to terminal 1
#define LINE_PASSENGER_IN_BUS_TO_TERMINAL_2 5 // for list_file. represents passenger in the bus going to terminal 2
#define LINE_PASSENGER_IN_BUS_TO_CAR_RENTAL 6 // for list_file. represents passenger in the bus going to car rental
#define LINE_BUS_ALL 7 // only used for counting the number of passengers in the bus, for report

#define SAMPST_LINE_DELAY_TERMINAL_1 1
#define SAMPST_LINE_DELAY_TERMINAL_2 2
#define SAMPST_LINE_DELAY_CAR_RENTAL 3

#define SAMPST_BUS_STOPPED_AT_TERMINAL_1 4
#define SAMPST_BUS_STOPPED_AT_TERMINAL_2 5
#define SAMPST_BUS_STOPPED_AT_CAR_RENTAL 6

#define SAMPST_BUS_MAKE_A_LOOP 7

#define SAMPST_PASSENGER_IN_SYSTEM_TO_TERMINAL_1 8
#define SAMPST_PASSENGER_IN_SYSTEM_TO_TERMINAL_2 9
#define SAMPST_PASSENGER_IN_SYSTEM_TO_CAR_RENTAL 10

// bus attribute for transfer
#define BUS_ARRIVAL_TIME 3
#define BUS_SAMPST_VARIABLE 4
#define BUS_PREVIOUS_TIME_AT_CAR_RENTAL 5

// for accessing attribute of transfer
#define PASSENGER_ARRIVAL_TIME 1
#define PASSENGER_DESTINATION 2
#define PASSENGER_FUTURE_BUS_LINE 3
#define PASSENGER_SAMPST_IN_LINE_VARIABLE 4 // for easy calling. for report generating purpose
#define PASSENGER_SAMPST_IN_SYSTEM_VARIABLE 5 // for easy calling. for report generating purpose


float meanArrivalTerminal1, meanArrivalTerminal2, meanArrivalCarRental;
double timeCarRentalToTerminal1, timeTerminal1ToTerminal2, timeTerminal2ToCarRental; 

boolean currentlyLoadingOrUnloading = FALSE;  // to prevent double loading or unloading triggered by sudden arrival of passanger;

FILE *outfile;

// Event Handler
void EHpassengerArrivalTerminal1();
void EHpassengerArrivalTerminal2();
void EHpassengerArrivalCarRental();

void EHbusArrive();
void EHbusGoNow();

void EHpassengerFinishUnloading();
void EHPassengerFinishLoading();

// helper
void schedulePassengerArrival(int destination);
int currentNumberOfPassangerInBus();
void triggerPassangerUnloadAndThenLoadIfTheresAny();
void forceBusToWait(double nextEventFinishTime);
void getCurrentBusLineAndLocationLine(int *busLine, int *toBeLoadedLine);
void busGoNow();

void report();

/* Main function. */
int main(){
    outfile = fopen("main.out", "w");
    // for consistency, everything is going to be converted to minutes and miles

    simulationRunTime = simulationRunTime * 60; // hours to minutes

    busSpeed = busSpeed / 60; // miles per minute
    unloadBottomRange = unloadBottomRange / 60; // seconds to minutes
    unloadTopRange = unloadTopRange / 60; // seconds to minutes
    loadBottomRange = loadBottomRange / 60; // seconds to minutes
    loadTopRange = loadTopRange / 60; // seconds to minutes

    meanArrivalTerminal1 = 60 / arrivalRateTerminal1;
    meanArrivalTerminal2 = 60 / arrivalRateTerminal2;
    meanArrivalCarRental = 60 / arrivalRateCarRental;
    timeCarRentalToTerminal1 = distanceCarRentalToTerminal1 / busSpeed;
    timeTerminal1ToTerminal2 = distanceTerminal1ToTerminal2 / busSpeed;
    timeTerminal2ToCarRental = distanceTerminal2ToCarRental / busSpeed;
        
    init_simlib();

     /* Schedule the first arrivals */

    schedulePassengerArrival(TERMINAL_1);
    schedulePassengerArrival(TERMINAL_2);
    schedulePassengerArrival(CAR_RENTAL);

    // as said in the problem, bus leaves immediately from car rental to terminal 1
    busGoNow();
    

    // schedule simulation end
    event_schedule(simulationRunTime, EVENT_END_SIMULATION);

    boolean endSimulation = FALSE;
    while(!endSimulation){
        
        timing();

        switch (next_event_type) {
            case EVENT_ARRIVAL_TERMINAL_1:
                // printf("[%.2f] Passanger arrived at terminal 1\n", sim_time);
                EHpassengerArrivalTerminal1();
                break;
            case EVENT_ARRIVAL_TERMINAL_2:
                // printf("[%.2f] Passanger arrived at terminal 2\n", sim_time);
                EHpassengerArrivalTerminal2();
                break;
            case EVENT_ARRIVAL_CAR_RENTAL:
                // printf("[%.2f] Passanger arrived at car rental\n", sim_time);
                EHpassengerArrivalCarRental();
                break;
            
            case EVENT_BUS_ARRIVE:
                // printf("[%.2f] Bus arrived\n", sim_time);
                EHbusArrive();
                break;
            case EVENT_BUS_DEPARTURE:
                EHbusGoNow();
                break;

            case EVENT_PASSENGER_FINISH_UNLOADING:
                // printf("[%.2f] Passanger unloaded\n", sim_time);
                EHpassengerFinishUnloading();
                break;
            case EVENT_PASSENGER_FINISH_LOADING:
                // printf("[%.2f] Passanger loaded to the bus\n", sim_time);
                EHPassengerFinishLoading();
                break;
        
            case EVENT_END_SIMULATION:
                endSimulation = TRUE; // will stop the simulation, no matter the condition of the other events. we can choose to stop the arrival of new passengers instead and continue till all passengers are transported
                break;
        }
    }

    report();

    fclose(outfile);
    return 0;
}



/*
    handle passenger arrival at Terminal 1
    1. schedule next arrival
    2. get that passenger in line
    3. check the bus. if the bus is there, get in!
*/
void EHpassengerArrivalTerminal1(){
    // schedule the next arrival at terminal 1
    schedulePassengerArrival(TERMINAL_1);

    transfer[PASSENGER_SAMPST_IN_LINE_VARIABLE] = SAMPST_LINE_DELAY_TERMINAL_1; 
    transfer[PASSENGER_SAMPST_IN_SYSTEM_VARIABLE] = SAMPST_PASSENGER_IN_SYSTEM_TO_CAR_RENTAL;

    // get in terminal 1 queue
    transfer[PASSENGER_ARRIVAL_TIME] = sim_time;
    transfer[PASSENGER_DESTINATION] = CAR_RENTAL;
    transfer[PASSENGER_FUTURE_BUS_LINE] = LINE_PASSENGER_IN_BUS_TO_CAR_RENTAL;

    list_file(LAST, LINE_TERMINAL_1);

    // if bus is here, get in!
    if(busLocation == TERMINAL_1){
        if(!currentlyLoadingOrUnloading){// need to check. if not checked, might result in paralel loading.
            triggerPassangerUnloadAndThenLoadIfTheresAny(); 
        }
    }
}

/*
    handle passenger arrival at Terminal 2
    1. schedule next arrival
    2. get that passenger in line
    3. check the bus. if the bus is there, get in!
*/
void EHpassengerArrivalTerminal2(){
    // schedule the next arrival
    schedulePassengerArrival(TERMINAL_2);

    transfer[PASSENGER_SAMPST_IN_LINE_VARIABLE] = SAMPST_LINE_DELAY_TERMINAL_2;
    transfer[PASSENGER_SAMPST_IN_SYSTEM_VARIABLE] = SAMPST_PASSENGER_IN_SYSTEM_TO_CAR_RENTAL;


    // get in line
    transfer[PASSENGER_ARRIVAL_TIME] = sim_time;
    transfer[PASSENGER_DESTINATION] = CAR_RENTAL;
    transfer[PASSENGER_FUTURE_BUS_LINE] = LINE_PASSENGER_IN_BUS_TO_CAR_RENTAL;

    list_file(LAST, LINE_TERMINAL_2);

    // if bus is here, get in!
    if(busLocation == TERMINAL_2){
        if(!currentlyLoadingOrUnloading){// need to check. if not checked, might result in paralel loading.
            triggerPassangerUnloadAndThenLoadIfTheresAny(); 
        }
    }
}

/*
    handle passenger arrival at Car Rental
    1. schedule next arrival
    2. get that passenger in line
    3. check the bus. if the bus is there, get in!
*/
void EHpassengerArrivalCarRental(){
    // schedule the next arrival
    schedulePassengerArrival(CAR_RENTAL);

    transfer[PASSENGER_SAMPST_IN_LINE_VARIABLE] = SAMPST_LINE_DELAY_CAR_RENTAL;

    // get in line
    transfer[PASSENGER_ARRIVAL_TIME] = sim_time;

    int destination = random_integer(probDistribOfPassangerDestinationFromCarRental, STREAM_CAR_RENTAL_TO_TERMINAL_DESTINATION); // 1 or 2
    if(destination == 1){
        // go to terminal 1
        transfer[PASSENGER_DESTINATION] = TERMINAL_1;
        transfer[PASSENGER_FUTURE_BUS_LINE] = LINE_PASSENGER_IN_BUS_TO_TERMINAL_1;
        transfer[PASSENGER_SAMPST_IN_SYSTEM_VARIABLE] = SAMPST_PASSENGER_IN_SYSTEM_TO_TERMINAL_1;

    } else{
        // go to terminal 2
        transfer[PASSENGER_DESTINATION] = TERMINAL_2;
        transfer[PASSENGER_FUTURE_BUS_LINE] = LINE_PASSENGER_IN_BUS_TO_TERMINAL_2;
        transfer[PASSENGER_SAMPST_IN_SYSTEM_VARIABLE] = SAMPST_PASSENGER_IN_SYSTEM_TO_TERMINAL_2;
    }

    list_file(LAST, LINE_CAR_RENTAL);

    // if bus is here, get in!
    if(busLocation == CAR_RENTAL){
        if(!currentlyLoadingOrUnloading){// need to check. if not checked, might result in paralel loading.
            triggerPassangerUnloadAndThenLoadIfTheresAny(); 
        }
    }
}

/*
    1. unload the passengers who want to go to terminal 1
    2. load the passengers
    3. check if the bus is ready to depart (5 minutes waiting time)

*/
void EHbusArrive(){
    int busSampstVariable = -1;
    // update bus location
    switch (busNextLocation){
        case TERMINAL_1:
            busLocation = TERMINAL_1;
            busNextLocation = TERMINAL_2;
            busSampstVariable = SAMPST_BUS_STOPPED_AT_TERMINAL_1;
            break;
        case TERMINAL_2:
            busLocation = TERMINAL_2;
            busNextLocation = CAR_RENTAL;
            busSampstVariable = SAMPST_BUS_STOPPED_AT_TERMINAL_2;
            break;
        case CAR_RENTAL:
            busLocation = CAR_RENTAL;
            busNextLocation = TERMINAL_1;
            busSampstVariable = SAMPST_BUS_STOPPED_AT_CAR_RENTAL;
            break;
        default:
            break;
    }

    // write down bus arrival time
    transfer[BUS_ARRIVAL_TIME] = sim_time;
    transfer[BUS_SAMPST_VARIABLE] = busSampstVariable;
    // schedule departure
    event_schedule(sim_time + busWaitingTime, EVENT_BUS_DEPARTURE);


    // trigger first unload the passengers.
    triggerPassangerUnloadAndThenLoadIfTheresAny();

}

/*
    handle unloading passenger from the bus
    
    called when a passenger if finished unloading.
     1. update the bus line
     2. then trigger the next unload
*/
void EHpassengerFinishUnloading(){
    int busLine = -1;
    int toBeLoadedLine = -1;
    getCurrentBusLineAndLocationLine(&busLine, &toBeLoadedLine);
    
    // update the bus' line. 
    list_remove(FIRST, busLine);
    // for report purpose. note down time in system
    sampst(sim_time - transfer[PASSENGER_ARRIVAL_TIME], transfer[PASSENGER_SAMPST_IN_SYSTEM_VARIABLE]);
    
    list_remove(FIRST, LINE_BUS_ALL); // for report purpose
    

    // trigger the next action either unload or go to load when there's no more unload
    triggerPassangerUnloadAndThenLoadIfTheresAny();

}

/*
    handle loading passenger to the bus
    called when a passenger is finished loading
    1. remove passenger from the line and put it in the bus
    2. then trigger the next loading

*/
void EHPassengerFinishLoading(){
    int busLine = -1;
    int toBeLoadedLine = -1;
    getCurrentBusLineAndLocationLine(&busLine, &toBeLoadedLine);

    // remove passenger from the line and put it in the bus

    list_remove(FIRST, toBeLoadedLine); // after removing, many things can be accessed from transfer variable
    sampst(sim_time - transfer[PASSENGER_ARRIVAL_TIME], transfer[PASSENGER_SAMPST_IN_LINE_VARIABLE]); // note down time in line
    list_file(LAST, transfer[PASSENGER_FUTURE_BUS_LINE]);
    list_file(LAST, LINE_BUS_ALL); // for report purpose


    triggerPassangerUnloadAndThenLoadIfTheresAny();
}

void EHbusGoNow(){
    sampst(sim_time - transfer[BUS_ARRIVAL_TIME], transfer[BUS_SAMPST_VARIABLE]); // for report purpose

     // note time to make a loop from car rental to car rental
    if(busLocation == CAR_RENTAL){
        sampst(sim_time - transfer[BUS_PREVIOUS_TIME_AT_CAR_RENTAL], SAMPST_BUS_MAKE_A_LOOP);
        transfer[BUS_PREVIOUS_TIME_AT_CAR_RENTAL] = sim_time;
    }
    busGoNow();
}

/*
    HELPER FUNCTION TO MAKE THE CODE MORE CONCISE
*/ 

/*
    will schedule time of arrival of the bus at the next location based on bus location, from current time.
*/
void busGoNow(){
   
    switch (busNextLocation){
        case TERMINAL_1:
            event_schedule(sim_time + timeCarRentalToTerminal1, EVENT_BUS_ARRIVE);
            busLocation = ON_THE_WAY;
            break;
        case TERMINAL_2:

            event_schedule(sim_time + timeTerminal1ToTerminal2, EVENT_BUS_ARRIVE);
            busLocation = ON_THE_WAY;
            break;
        case CAR_RENTAL:

            event_schedule(sim_time + timeTerminal2ToCarRental, EVENT_BUS_ARRIVE);
            busLocation = ON_THE_WAY;
            break;
    }
}


/*
    return the current number of passenger in the bus, from all lines of passenger in the bus
*/
int currentNumberOfPassangerInBus () {
    return list_size[LINE_PASSENGER_IN_BUS_TO_TERMINAL_1] + list_size[LINE_PASSENGER_IN_BUS_TO_TERMINAL_2] + list_size[LINE_PASSENGER_IN_BUS_TO_CAR_RENTAL];
}


/*
    schedule the next arrival of passenger at the destination
*/
void schedulePassengerArrival(int destination){
    switch (destination){
        case TERMINAL_1:
            event_schedule(sim_time + expon(meanArrivalTerminal1, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_1), EVENT_ARRIVAL_TERMINAL_1);
            break;
        case TERMINAL_2:
            event_schedule(sim_time + expon(meanArrivalTerminal2, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_2), EVENT_ARRIVAL_TERMINAL_2);
            break;
        case CAR_RENTAL:
            event_schedule(sim_time + expon(meanArrivalCarRental, STREAM_PASSENGER_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);
            break;
    }
}

void getCurrentBusLineAndLocationLine(int *busLine, int *toBeLoadedLine){
    switch (busLocation){
        case TERMINAL_1:
            *busLine = LINE_PASSENGER_IN_BUS_TO_TERMINAL_1;
            *toBeLoadedLine = LINE_TERMINAL_1;
            break;
        case TERMINAL_2:
            *busLine = LINE_PASSENGER_IN_BUS_TO_TERMINAL_2;
            *toBeLoadedLine = LINE_TERMINAL_2;
            break;
        case CAR_RENTAL:
            *busLine = LINE_PASSENGER_IN_BUS_TO_CAR_RENTAL;
            *toBeLoadedLine = LINE_CAR_RENTAL;
            break;
    }
}



/*
    this function is responsible for the job of loading and unloadng. 

    will start unloading if there's any passenger to unload. 
    if there's none, will start load passenger.
    if none, will do nothing. 
*/
void triggerPassangerUnloadAndThenLoadIfTheresAny(){
    int busLine = -1;
    int toBeLoadedLine = -1;
    getCurrentBusLineAndLocationLine(&busLine, &toBeLoadedLine);

    // if there are passengers to be unloaded, will trigger 
    if(list_size[busLine] > 0){
        double finishUnloadingTime = sim_time + uniform(unloadBottomRange, unloadTopRange, STREAM_UNLOADING_TIME);
        event_schedule(finishUnloadingTime, EVENT_PASSENGER_FINISH_UNLOADING);
        
        // handle is bus about to go in the middle of unloading the passenger by postpone bus departure if the bus is about to go
        forceBusToWait(finishUnloadingTime);
        currentlyLoadingOrUnloading = TRUE;
    } else{
        // if there are no passengers to be unloaded, check if there are passengers to be loaded
        if (list_size[toBeLoadedLine] > 0 && currentNumberOfPassangerInBus() < busCapacity){
            double finishLoadingTime = sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME);
            event_schedule(finishLoadingTime, EVENT_PASSENGER_FINISH_LOADING);
            // handle is bus about to go in the middle of loading the passenger by postpone bus departure if the bus is about to go
            forceBusToWait(finishLoadingTime);
            currentlyLoadingOrUnloading = TRUE;
        } else{
            currentlyLoadingOrUnloading = FALSE;
        }
    }
}

/*
    IMPORTANT: call this function after the desired action, not before.
    event scheduling uses list_file, who will resolve ties by FIFO. 
    so if an event is done by 6 and the bus departure is also at 6, that event will be triggered first, and then bus departure will be called next. allowing you to call the next force bus to wait. 
*/
void forceBusToWait(double nextEventFinishTime){
    event_cancel(EVENT_BUS_DEPARTURE); // tf[1] is the previous scheduled time of bus departure
    // schedule the maximum between previous departure time and the finish unloading time
    event_schedule(fmax(nextEventFinishTime, transfer[1]), EVENT_BUS_DEPARTURE); // because of FIFO, event above will be called first. before bus departure if there are more passengers to be unloaded
}




/* Report generator function. */
void report(void){
    /*
        avg max number in each queue
    */
    // line terminal 1
    printf("\n\nREPORT GENERATED\n\n");
    fprintf(outfile,"\n\nREPORT\n\n");
    fprintf(outfile, "Average number in of queue in each location:\n");
    out_filest(outfile, LINE_TERMINAL_1, LINE_CAR_RENTAL);
    fprintf(outfile, "\n\nlegend: \n   1 - terminal 1,\n   2 - terminal 2,\n   3 - car rental\n");


    /*
        avg max delay in each queue
    */
    fprintf(outfile, "\n\nDelays in queue, in minutes:\n");
    out_sampst(outfile, SAMPST_LINE_DELAY_TERMINAL_1,SAMPST_LINE_DELAY_CAR_RENTAL);
    fprintf(outfile, "\n\nlegend: \n   1 - terminal 1,\n   2 - terminal 2,\n   3 - car rental\n");
    /*
        avg max number on the bus
    */
    filest(LINE_BUS_ALL);
    printf("average number in bus = %10.3f\n", transfer[1]);
    printf("max number in bus = %10.3f\n", transfer[2]);
    printf("min number in bus = %10.3f\n", transfer[3]);

    fprintf(outfile, "\n\nWith %d capacity bus, average number in bus = %10.3f", busCapacity, transfer[1]);
    out_filest(outfile, LINE_BUS_ALL, LINE_BUS_ALL);
   
    /*
        avg max min time the bus is stopped at each location
    */
    fprintf(outfile, "\n\nBus stopped at terminal 1, in minutes:\n");
    out_sampst(outfile, SAMPST_BUS_STOPPED_AT_TERMINAL_1, SAMPST_BUS_STOPPED_AT_TERMINAL_1);
    fprintf(outfile, "\n\nBus stopped at terminal 2, in minutes:\n");
    out_sampst(outfile, SAMPST_BUS_STOPPED_AT_TERMINAL_2, SAMPST_BUS_STOPPED_AT_TERMINAL_2);
    fprintf(outfile, "\n\nBus stopped at car rental, in minutes:\n");
    out_sampst(outfile, SAMPST_BUS_STOPPED_AT_CAR_RENTAL, SAMPST_BUS_STOPPED_AT_CAR_RENTAL);
    fprintf(outfile, "\n\nlegend: \n   4 - terminal 1,\n   5 - terminal 2,\n   6 - car rental\n");
    /*
        avg max min time for the bust to make a loop (car rental to car rental)
    */
    fprintf(outfile, "\n\nTime for the bus to make a loop, in minutes:\n");
    out_sampst(outfile, SAMPST_BUS_MAKE_A_LOOP, SAMPST_BUS_MAKE_A_LOOP);
    /*
        avg max min time a person is in the system
    */
    fprintf(outfile, "\n\nTime a person is in the system, in minutes:\n");
    out_sampst(outfile, SAMPST_PASSENGER_IN_SYSTEM_TO_TERMINAL_1, SAMPST_PASSENGER_IN_SYSTEM_TO_CAR_RENTAL);
    fprintf(outfile, "\n\nlegend: \n   8 - terminal 1,\n   9 - terminal 2,\n   10 - car rental\n");
}
