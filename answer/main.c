#include "simlib.h"
#include "main.h"






// DEFINITION FROM OURSELVES
// define events
#define EVENT_BUS_TERMINAL_1 1 // bus arrive at terminal 1, from car rental. 
#define EVENT_BUS_TERMINAL_2 2 // bus arrive at terminal 2, from terminal 1
#define EVENT_BUS_CAR_RENTAL 3 // bus arrive at car rental, from or terminal 2
#define EVENT_BUS_DEPARTURE 4 // bus is on the way from any location to another location

#define EVENT_ARRIVAL_TERMINAL_1 5 // passenger arrive at terminal 1. store arrival time, destination
#define EVENT_ARRIVAL_TERMINAL_2 6 // passenger arrive at terminal 2. store arrival time, destination
#define EVENT_ARRIVAL_CAR_RENTAL 7 // passenger arrive at car rental. store arrival time, destination

#define EVENT_DEPARTURE_TERMINAL_1 8 // passenger depart from terminal 1 using bus to car rental
#define EVENT_DEPARTURE_TERMINAL_2 9 // passenger depart from terminal 2 using bus to car rental
#define EVENT_DEPARTURE_CAR_RENTAL 10 // passenger depart from car rental using bus to terminal 1 or terminal 2
#define EVENT_PASSANGER_LOADING_TERMINAL_1 11 // loading passanger X from terminal 1to the bus
#define EVENT_PASSANGER_UNLOADING_TERMINAL_1 12 // unloading passanger X from the bus to terminal 1
#define EVENT_PASSANGER_LOADING_TERMINAL_2 13 // loading passanger X from terminal 2 to the bus
#define EVENT_PASSANGER_UNLOADING_TERMINAL_2 14 // unloading passanger X from the bus to terminal 2
#define EVENT_PASSANGER_LOADING_CAR_RENTAL 15 // loading passanger X from car rental to the bus
#define EVENT_PASSANGER_UNLOADING_CAR_RENTAL 16 // unloading passanger X from the bus to car rental

#define EVENT_END_SIMULATION 17

#define LINE_TERMINAL_1 1 // for list_file. represents the line at terminal 1
#define LINE_TERMINAL_2 2 // for list_file. represents the line at terminal 2
#define LINE_CAR_RENTAL 3 // for list_file. represents the line at car rental

#define LINE_PASSANGER_IN_BUS_TO_TERMINAL_1 4 // for list_file. represents passanger in the bus going to terminal 1
#define LINE_PASSANGER_IN_BUS_TO_TERMINAL_2 5 // for list_file. represents passanger in the bus going to terminal 2
#define LINE_PASSANGER_IN_BUS_TO_CAR_RENTAL 6 // for list_file. represents passanger in the bus going to car rental





// for accessing attribute of transfer
#define PASSANGER_ARRIVAL_TIME 1
#define PASSANGER_DESTINATION 2


float meanArrivalTerminal1, meanArrivalTerminal2, meanArrivalCarRental;
double timeCarRentalToTerminal1, timeTerminal1ToTerminal2, timeTerminal2ToCarRental; 

double minTimeToDeparture;

int currentNumberOfPassangerInBus();

void busGoNow();

void passengerArrivalTerminal1();
void passengerArrivalTerminal2();
void passengerArrivalCarRental();

void busArriveAtTerminal1();
void busArriveAtTerminal2();
void busArriveAtCarRental();

void passangerUnloadingTerminal1();
void passangerLoadingTerminal1();
void passangerLoadingCarRental();

/* Main function. */
int main(){
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

    event_schedule(expon(meanArrivalTerminal1, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_1),EVENT_ARRIVAL_TERMINAL_1);
    event_schedule(expon(meanArrivalTerminal2, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_2),EVENT_ARRIVAL_TERMINAL_2);
    event_schedule(expon(meanArrivalCarRental, STREAM_PASSENGER_INTERARRIVAL_CAR_RENTAL),EVENT_ARRIVAL_CAR_RENTAL);

    // as said in the problem, bus leaves immediately from car rental to terminal 1
    busGoNow();

    // schedule simulation end
    event_schedule(simulationRunTime, EVENT_END_SIMULATION);

    boolean endSimulation = FALSE;
    while(!endSimulation){
        
        timing();

        switch (next_event_type) {
            case EVENT_ARRIVAL_TERMINAL_1:
                printf("[%.2f] Passanger arrived at terminal 1\n", sim_time);
                passengerArrivalTerminal1();
                break;
            case EVENT_ARRIVAL_TERMINAL_2:
                printf("[%.2f] Passanger arrived at terminal 2\n", sim_time);
                passengerArrivalTerminal2();
                break;
            case EVENT_ARRIVAL_CAR_RENTAL:
                printf("[%.2f] Passanger arrived at car rental\n", sim_time);
                passengerArrivalCarRental();
                break;
            case EVENT_BUS_TERMINAL_1:
                printf("[%.2f] Bus arrived at terminal 1\n", sim_time);
                busArriveAtTerminal1();
                break;
            case EVENT_BUS_TERMINAL_2:
                printf("[%.2f] Bus arrived at terminal 2\n", sim_time);
                busArriveAtTerminal2();
                break;
            case EVENT_BUS_CAR_RENTAL:
                printf("[%.2f] Bus arrived at car rental\n", sim_time);
                busArriveAtCarRental();
                break;
            case EVENT_BUS_DEPARTURE:
                printf("[%.2f] Bus is on the way\n", sim_time);
                busGoNow();
                break;
            case EVENT_PASSANGER_LOADING_TERMINAL_1:
                printf("[%.2f] Passanger loaded to the bus in terminal 1\n", sim_time);
                passangerLoadingTerminal1();
                break;
            case EVENT_PASSANGER_LOADING_CAR_RENTAL:
                printf("[%.2f] Passanger loaded to the bus in car rental\n", sim_time);
                passangerLoadingCarRental();
                break;
            case EVENT_PASSANGER_UNLOADING_TERMINAL_1:
                printf("[%.2f] Passanger unloaded from the bus in terminal 1\n", sim_time);
                passangerUnloadingTerminal1();
                break;
            case EVENT_END_SIMULATION:
                endSimulation = TRUE; // will stop the simulation, no matter the condition of the other events. we can choose to stop the arrival of new passengers instead and continue till all passengers are transported
                break;
        }
    }
    return 0;
}



/*
    1. get in line. terminal line is
    2. if the bus is not yet full and the bus is there, postpone the departure of the bus and schedule the loading of the passanger

*/
void passengerArrivalTerminal1(){
    // schedule the next arrival
    event_schedule(sim_time + expon(meanArrivalTerminal1, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_1), EVENT_ARRIVAL_TERMINAL_1);

    // get in line
    transfer[PASSANGER_ARRIVAL_TIME] = sim_time;
    list_file(LAST, LINE_TERMINAL_1);

    /* TODO:
        1. check if the unloading is done
        2. postpone the departure of the bus (cancel and rescchedule the event of bus departure)
    */
    // if (busLocation == TERMINAL_1 && currentNumberOfPassangerInBus() < busCapacity){
    //     event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADING_TERMINAL_1);
    // }
}

/*
    TODO: handle passanger arrival at Terminal 2
*/
void passengerArrivalTerminal2(){
    // schedule the next arrival
    event_schedule(sim_time + expon(meanArrivalTerminal2, STREAM_PASSENGER_INTERARRIVAL_TERMINAL_2), EVENT_ARRIVAL_TERMINAL_2);

    // get in line
    transfer[PASSANGER_ARRIVAL_TIME] = sim_time;
    list_file(LAST, LINE_TERMINAL_2);
}

/*
    TODO: handle passanger arrival at Car Rental
*/
void passengerArrivalCarRental(){
    // schedule the next arrival
    event_schedule(sim_time + expon(meanArrivalCarRental, STREAM_PASSENGER_INTERARRIVAL_CAR_RENTAL), EVENT_ARRIVAL_CAR_RENTAL);

    // get in line
    transfer[PASSANGER_ARRIVAL_TIME] = sim_time;
    int destination = random_integer(probDistribOfPassangerDestinationFromCarRental, STREAM_CAR_RENTAL_TO_TERMINAL_DESTINATION);
    transfer[PASSANGER_DESTINATION] = destination;
    list_file(LAST, LINE_CAR_RENTAL);

}

/*
    1. unload the passengers who want to go to terminal 1
    2. load the passengers
    3. check if the bus is ready to depart (5 minutes waiting time)

*/
void busArriveAtTerminal1(){
    busLocation = TERMINAL_1;
    minTimeToDeparture = sim_time + busWaitingTime;

    // unload the passengers
    if (list_size[LINE_PASSANGER_IN_BUS_TO_TERMINAL_1] > 0){
        event_schedule(sim_time + uniform(unloadBottomRange, unloadTopRange, STREAM_UNLOADING_TIME), EVENT_PASSANGER_UNLOADING_TERMINAL_1);
    }
    // if no passengers to unload, check if there are passengers to load
    else if (list_size[LINE_TERMINAL_1] > 0 && currentNumberOfPassangerInBus() < busCapacity){
        event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADING_TERMINAL_1);
    }

    // TODO:  need to handle the bus will be here for at least 5 minutes
    // need to maintain the minTimeToDeparture (as it is used as condition for departure in unloading and loading event)
    if (currentNumberOfPassangerInBus() < busCapacity && minTimeToDeparture > sim_time){
        // during this time, check if there are more passengers coming
        event_schedule(minTimeToDeparture, EVENT_BUS_DEPARTURE); // temporary, will be changed to check if there are more passengers coming
    } else {
        event_schedule(sim_time, EVENT_BUS_DEPARTURE);
    }

}

/*
    TODO: handle bus arrival at terminal 2
*/
void busArriveAtTerminal2(){
}

/*
    TODO: handle bus arrival at car rental
*/
void busArriveAtCarRental(){
}

/*
    TODO: handle unloading passanger from the bus in terminal 1
*/
void passangerUnloadingTerminal1(){
    list_remove(FIRST, LINE_PASSANGER_IN_BUS_TO_TERMINAL_1);

    // if there are more passengers going to terminal 1, schedule to unload them
    if (list_size[LINE_PASSANGER_IN_BUS_TO_TERMINAL_1] > 0){
        event_schedule(sim_time + uniform(unloadBottomRange, unloadTopRange, STREAM_UNLOADING_TIME), EVENT_PASSANGER_UNLOADING_TERMINAL_1);
    } 
    // if all is unloaded and there are passengers in terminal 1, schedule to load them
    // check if the bus is not yet full
    else if (list_size[LINE_TERMINAL_1] > 0 && currentNumberOfPassangerInBus() < busCapacity){
        event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADING_TERMINAL_1);
    } 
    // if all is unloaded and there are no passengers in terminal 1, check if the bus is ready to depart
    else if (sim_time >= minTimeToDeparture){
        event_schedule(sim_time, EVENT_BUS_DEPARTURE);
    }
}

/*
    TODO: handle loading passanger to the bus in terminal 1
*/
void passangerLoadingTerminal1(){
    list_remove(FIRST, LINE_TERMINAL_1);
    list_file(LAST, LINE_PASSANGER_IN_BUS_TO_TERMINAL_1);

    // if the bus is not yet full and there are more passengers, schedule to load them
    if (list_size[LINE_TERMINAL_1] > 0 && currentNumberOfPassangerInBus() < busCapacity){
        event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADING_TERMINAL_1);
    // if all passengers are loaded, check if the bus is ready to depart (5 minutes waiting time)
    } else if (sim_time >= minTimeToDeparture){
        event_schedule(sim_time, EVENT_BUS_DEPARTURE);
    }
}

void passangerLoadingCarRental(){
    list_remove(FIRST, LINE_CAR_RENTAL);
    
    if (transfer[PASSANGER_DESTINATION] == TERMINAL_1){
        list_file(LAST, LINE_PASSANGER_IN_BUS_TO_TERMINAL_1);
    } else {
        list_file(LAST, LINE_PASSANGER_IN_BUS_TO_TERMINAL_2);
    }

    // if the bus is not yet full and there are more passengers, schedule to load them
    if (list_size[LINE_CAR_RENTAL] > 0 && currentNumberOfPassangerInBus() < busCapacity){
        event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADING_CAR_RENTAL);
    // all passengers are loaded
    // TODO: need to check if the bus is ready to depart (5 minutes waiting time)
    } else {
        event_schedule(sim_time, EVENT_BUS_DEPARTURE);
    } 
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
            event_schedule(sim_time + timeCarRentalToTerminal1, EVENT_BUS_TERMINAL_1);
            busNextLocation = TERMINAL_2;
            busLocation = ON_THE_WAY;
            break;
        case TERMINAL_2:
            event_schedule(sim_time + timeTerminal1ToTerminal2, EVENT_BUS_TERMINAL_2);
            busNextLocation = CAR_RENTAL;
            busLocation = ON_THE_WAY;
            break;
        case CAR_RENTAL:
            event_schedule(sim_time + timeTerminal2ToCarRental, EVENT_BUS_CAR_RENTAL);
            busNextLocation = TERMINAL_1;
            busLocation = ON_THE_WAY;
            break;
    }
}


/*
    return the current number of passanger in the bus, from all lines of passanger in the bus
*/
int currentNumberOfPassangerInBus () {
    return list_size[LINE_PASSANGER_IN_BUS_TO_TERMINAL_1] + list_size[LINE_PASSANGER_IN_BUS_TO_TERMINAL_2] + list_size[LINE_PASSANGER_IN_BUS_TO_CAR_RENTAL];
}