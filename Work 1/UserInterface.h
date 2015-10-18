#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

struct UserInterface {
int framesTransmitted;
int framesReceived;
int timeOutOccurrences;
int rejReceived;
int rejTransmitted;
}


void initalValues(UserInterface *interface);

void incFramesTransmitted(UserInterface *interface);

void incFramesReceived(UserInterface *interface);

void incTimeOutOccurrences(UserInterface *interface);

void incRejReceived(UserInterface *interface);

void incRejTransmitted(UserInterface *interface);

void printValues(UserInterface *interface);


#endif
