#include "Alarm.h"

#include <stdio.h>
#include <signal.h>

void clearAlarms(){
	alarm(0);
	printf("Alarm stopped");
}

void createAlarm(){
	alarm(DEFAULT_TIME);
	printf("Alarm with %d seconds was turn on", DEFAULT_TIME);
}

void createAlarm(int time){
	alarm(time);
	printf("Alarm with %d seconds was turn on", time);
}
