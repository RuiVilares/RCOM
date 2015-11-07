#include "GlobalVariable.h"

int numTransmissionsGlobal = DEFAULT_NUM_TRANSMISSIONS;

int storedNumTransmissionsValueGlobal = DEFAULT_NUM_TRANSMISSIONS;

bool alarmGlobal = false;

int infoFieldGlobal = 0;
int infoFieldReSentGlobal = 0;
int timeOutNumberGlobal = 0;
int rejNumGlobal = 0;

void setUpGlobalTransmissionNum(int nTimes) {
	numTransmissionsGlobal = nTimes;
	storedNumTransmissionsValueGlobal = nTimes;
	resetAlarmGlobal();
}

void decNumTransmissionsGlobal() {
	numTransmissionsGlobal--;
}

bool numTransmissionsGlobalGreaterThanZero() {
	return numTransmissionsGlobal > 0;
}

void resetNumTransmissionsGlobal() {
	numTransmissionsGlobal = storedNumTransmissionsValueGlobal;
}

void resetAlarmGlobal() {
	alarmGlobal = false;
}

void setAlarmGlobal() {
	alarmGlobal = true;
}