#ifndef MAIN_H
#define MAIN_H
// DEFINITION FROM THE PROBLEM
// define stream. follows what the book told
#define STREAM_PASSENGER_INTERARRIVAL_TERMINAL_1 1
#define STREAM_PASSENGER_INTERARRIVAL_TERMINAL_2 2
#define STREAM_PASSENGER_INTERARRIVAL_CAR_RENTAL 3
#define STREAM_UNLOADING_TIME 4
#define STREAM_LOADING_TIME 5
#define STREAM_CAR_RENTAL_TO_TERMINAL_DESTINATION 6

double busSpeed = 30; // 30 miles per hour
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
double probDistribOfPassangerDestinationFromCarRental[3] = {0, 0.583, 1.0};

int simulationRunTime = 8; // 8 hours

float unloadBottomRange = 16; // seconds
float unloadTopRange = 24; // seconds

float loadBottomRange = 15; // seconds
float loadTopRange = 25; // seconds



// create boolean type
#define FALSE 0
#define TRUE 1
#define boolean int
#endif