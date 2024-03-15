#include "simlib.h"
// DEFINITION FROM THE PROBLEM
// define stream. follows what the book told
#define STREAM_INTERARRIVAL_TERMINAL_1 1
#define STREAM_INTERARRIVAL_TERMINAL_2 2
#define STREAM_INTERARRIVAL_CAR_RENTAL 3
#define STREAM_UNLOADING_TIME 4
#define STREAM_LOADING_TIME 5
#define STREAM_CAR_RENTAL_TO_TERMINAL_DESTINATION 6


int busSpeed = 30; // 30 miles per hour
const float arrivalRateTerminal1 = 14; // 14 per hour
const float arrivalRateTerminal2 = 10; // 10 per hour
const float arrivalRateCarRental = 24; // 24 per hour
const int busCapacity = 30;
const int busWaitingTime = 5; // 5 minutes;

// bus initially at car rental, next location is terminal 1
// bus helper variable
#define TERMINAL_1 1
#define TERMINAL_2 2
#define CAR_RENTAL 3
#define ON_THE_WAY 4
int busLocation = CAR_RENTAL;
int busNextLocation = TERMINAL_1;

const float distanceCarRentalToTerminal1 = 4.5; // miles
const float distanceTerminal1ToTerminal2 = 1; // miles
const float distanceTerminal2ToCarRental = 4.5; // miles

const float probabilityCarRentalToTerminal1 = 0.583;
const float probabilityCarRentalToTerminal2 = 0.417;

const int simulationRunTime = 8; // 8 hours

float unloadBottomRange = 16; // seconds
float unloadTopRange = 24; // seconds

float loadBottomRange = 15; // seconds
float loadTopRange = 25; // seconds

// DEFINITION FROM OURSELVES
// define events
#define EVENT_BUS_TERMINAL_1 1 // bus arrive at terminal 1, from car rental
#define EVENT_BUS_TERMINAL_2 2 // bus arrive at terminal 2, from terminal 1
#define EVENT_BUS_CAR_RENTAL 3 // bus arrive at car rental, from or terminal 2
#define EVENT_BUS_DEPARTURE 4 // bus is on the way from any location to another location

#define EVENT_ARRIVAL_TERMINAL_1 5 // passenger arrive at terminal 1
#define EVENT_ARRIVAL_TERMINAL_2 6 // passenger arrive at terminal 2
#define EVENT_ARRIVAL_CAR_RENTAL 7 // passenger arrive at car rental

#define EVENT_DEPARTURE_TERMINAL_1 8 // passenger depart from terminal 1 using bus to car rental
#define EVENT_DEPARTURE_TERMINAL_2 9 // passenger depart from terminal 2 using bus to car rental
#define EVENT_DEPARTURE_CAR_RENTAL 10 // passenger depart from car rental using bus to terminal 1 or terminal 2
#define EVENT_PASSANGER_LOADED 11 // bus passanger X loaded
#define EVENT_PASSANGER_UNLOADED 12 // bus passanger X unloaded

#define EVENT_END_SIMULATION 13

#define LINE_TERMINAL_1 1 // for list_file. represents the line at terminal 1
#define LINE_TERMINAL_2 2 // for list_file. represents the line at terminal 2
#define LINE_CAR_RENTAL 3 // for list_file. represents the line at car rental
#define LINE_BUS 4 // for list_file. represents the bus

#define LINE_LOAD 5 // for list_file. represents the bus unloading passanger

// create boolean type
#define FALSE 0
#define TRUE 1
#define boolean int


float meanArrivalTerminal1, meanArrivalTerminal2, meanArrivalCarRental;
double timeCarRentalToTerminal1, timeTerminal1ToTerminal2, timeTerminal2ToCarRental; 

void busGoNow();

void arrivalTerminal1();
void arrivalTerminal2();
void arrivalCarRental();

void busArriveAtTerminal1();
void busArriveAtTerminal2();
void busArriveAtCarRental();


/* Main function. */
int main(){
    // for consistency, everything is going to be converted to minutes and miles
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
    
    printf("Hello, World!\n"); // temporary, just to make sure the program is running
    
    init_simlib();

     /* Schedule the first arrivals */

    event_schedule(expon(meanArrivalTerminal1, STREAM_INTERARRIVAL_TERMINAL_1),EVENT_ARRIVAL_TERMINAL_1);
    event_schedule(expon(meanArrivalTerminal2, STREAM_INTERARRIVAL_TERMINAL_2),EVENT_ARRIVAL_TERMINAL_2);
    event_schedule(expon(meanArrivalCarRental, STREAM_INTERARRIVAL_CAR_RENTAL),EVENT_ARRIVAL_CAR_RENTAL);

    // as said in the problem, bus leaves immediately from car rental to terminal 1
    busGoNow();

    // schedule simulation end
    event_schedule(simulationRunTime, EVENT_END_SIMULATION);

    boolean endSimulation = FALSE;
    while(!endSimulation){
        
        timing();

        switch (next_event_type) {
            case EVENT_ARRIVAL_TERMINAL_1:
                arrivalTerminal1();
                break;
            case EVENT_ARRIVAL_TERMINAL_2:
                arrivalTerminal2();
                break;
            case EVENT_ARRIVAL_CAR_RENTAL:
                arrivalCarRental();
                break;
            case EVENT_BUS_TERMINAL_1:
                busArriveAtTerminal1();
                break;
            case EVENT_BUS_TERMINAL_2:
                busArriveAtTerminal2();
                break;
            case EVENT_BUS_CAR_RENTAL:
                busArriveAtCarRental();
                break;
            case EVENT_BUS_DEPARTURE:
                busGoNow();
                break;
            case EVENT_END_SIMULATION:
                endSimulation = TRUE; // will stop the simulation, no matter the condition of the other events. we can choose to stop the arrival of new passengers instead and continue till all passengers are transported
                break;
        }
    }
    return 0;
}

/*
    TODO: handle passanger arrival at Terminal 1
    1. get in line. terminal line is
    2. if the bus is not yet full and the bus is there, postpone the departure of the bus and get in the bus.
*/
void arrivalTerminal1(){
    // schedule the next arrival
    event_schedule(sim_time + expon(meanArrivalTerminal1, STREAM_INTERARRIVAL_TERMINAL_1), EVENT_ARRIVAL_TERMINAL_1);

    // get in line
    list_file(LAST, LINE_TERMINAL_1);

    // if the bus is there, has enough capacity, and there's no one else on the queue, get in the bus 
    if(busLocation == TERMINAL_1){
        if(list_size[LINE_BUS] < busCapacity){
            list_file(LAST, LINE_BUS);
            event_schedule(sim_time + uniform(loadBottomRange, loadTopRange, STREAM_LOADING_TIME), EVENT_PASSANGER_LOADED);
            // postpone the departure of the bus
            event_cancel(EVENT_BUS_DEPARTURE);
            event_schedule(sim_time + busWaitingTime, EVENT_BUS_DEPARTURE);
        }
    }
}

/*
    TODO: handle passanger arrival at Terminal 2
*/
void arrivalTerminal2(){
}

/*
    TODO: handle passanger arrival at Car Rental
*/
void arrivalCarRental(){

}

/*
    TODO: handle bus arrival at terminal 1
    1. unload the passengers. bye-bye to passanger whoare going to terminal 1
    2. load the passengers
    3. if full or no more passengers, schedule departure
*/
void busArriveAtTerminal1(){
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