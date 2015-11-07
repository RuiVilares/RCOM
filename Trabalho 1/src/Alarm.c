#include "Alarm.h"

void alarmHandler() {
	decNumTransmissionsGlobal();
	setAlarmGlobal();
	timeOutNumberGlobal++;
	printf("Alarm generated!\n");
}

void setUpAlarmSignal() {
	(void) signal(SIGALRM, alarmHandler);  // instala  rotina que atende interrupcao
}