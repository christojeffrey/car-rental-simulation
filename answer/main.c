#include "simlib.h"

// define stream. follows what the book told
#define STREAM_INTERARRIVAL_TERMINAL_1 1
#define STREAM_INTERARRIVAL_TERMINAL_2 2
#define STREAM_INTERARRIVAL_CAR_RENTAL 3
#define STREAM_UNLOADING_TIME 4
#define STREAM_LOADING_TIME 5
#define STREAM_CAR_RENTAL_TO_TERMINAL_DESTINATION 6

// define events
#define EVENT_BUS_TERMINAL_1 1
#define EVENT_BUS_TERMINAL_2 2
#define EVENT_BUS_CAR_RENTAL 3

#define EVENT_ARRIVAL_TERMINAL_1 4
#define EVENT_ARRIVAL_TERMINAL_2 5
#define EVENT_ARRIVAL_CAR_RENTAL 6

#define EVENT_DEPARTURE_TERMINAL_1 7
#define EVENT_DEPARTURE_TERMINAL_2 8
#define EVENT_DEPARTURE_CAR_RENTAL 9

#define EVENT_END_SIMULATION 10
// for consistency, everything is converted to minutes and miles
const int busSpeed = 30 / 60; // 30 miles per hour
const float arrivalRateTerminal1 = 14 / 60; // 14 per hour
const float arrivalRateTerminal2 = 10 / 60; // 10 per hour
const float arrivalRateCarRental = 24 / 60; // 24 per hour
const int busCapacity = 30;

#define TERMINAL_1 1
#define TERMINAL_2 2
#define CAR_RENTAL 3
// bus initially at car rental, next location is terminal 1
const int busLocation = CAR_RENTAL;
const int busNextLocation = TERMINAL_1;

const float distanceCarRentalToTerminal1 = 4.5; // miles
const float distanceTerminal1ToTerminal2 = 1; // miles
const float distanceTerminal2ToCarRental = 4.5; // miles

const float probabilityCarRentalToTerminal1 = 0.583;
const float probabilityCarRentalToTerminal2 = 0.417;

const int simulationRunTime = 8 * 60; // 8 hours
/* Main function. */


#define FALSE 0
#define TRUE 1
int main(){
    printf("Hello, World!\n");
    init_simlib();
    // schedule simulation end
    event_schedule(simulationRunTime, EVENT_END_SIMULATION);

    int endSimulation = FALSE;
    while(!endSimulation){
        timing();
        switch (next_event_type) {
            case EVENT_END_SIMULATION:
                endSimulation = TRUE;
                break;
        }
    }
    return 0;
}