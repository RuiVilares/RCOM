#include "Alarm.h"
#include <signal.h>

void clearAlarms(){
	alarm(0);
}

void createAlarm(){
	alarm(DEFAULT_TIME);
}

void createAlarm(int time){
	alarm(time);
}
