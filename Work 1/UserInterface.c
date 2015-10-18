#include "UserInterface.h"
#include <stdio.h>

void initalValues(UserInterface *interface){
	interface->framesTransmitted=0;
	interface->framesReceived=0;
	interface->timeOutOccurrences=0;
	interface->rejReceived=0;
	interface->rejTransmitted=0;
}

void incFramesTransmitted(UserInterface *interface){
	interface->framesTransmitted+=1;
}

void incFramesReceived(UserInterface *interface){
	interface->framesReceived+=1;
}

void incTimeOutOccurrences(UserInterface *interface){
	interface->timeOutOccurrences+=1;
}

void incRejReceived(UserInterface *interface){
	interface->rejReceived+=1;
}

void incRejTransmitted(UserInterface *interface){
	interface->rejTransmitted+=1;
}

void printValues(UserInterface *interface){
	printf("VALUES\n");
	printf("Frames Transmitted: %d\n", interface->framesTransmitted);
	printf("Frames Received: %d\n", interface->framesReceived);
	printf("Time Out Occurences: %d\n", interface->timeOutOccurrences);
	printf("Rej Transmitted: %d\n", interface->rejTransmitted);
	printf("Rej Received: %d\n", interface->rejReceived);
	
}
