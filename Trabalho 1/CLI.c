#include "CLI.h"

void makeChoices(linkLayer *linkStruct) {

	chooseBaudRate(linkStruct);

	chooseRetransmissionsMax(linkStruct);

	chooseTimeOut(linkStruct);
}

void chooseBaudRate(linkLayer *linkStruct) {
	int choice;

	printf("Baud Rate options: (0 for default)\n");
	printf("300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200\n");
	printf("Baud Rate: ");

	if (scanf("%d", &choice) < 0) {
		printf("Will be used the default baud rate: 38400\n");
		linkStruct->baudRate = BAUDRATE_DEFAULT;
		return;
	}

	switch(choice) {
		case 300:
		linkStruct->baudRate = BAUDRATE_300;
		return;
		case 600:
		linkStruct->baudRate = BAUDRATE_600;
		return;
		case 1200:
		linkStruct->baudRate = BAUDRATE_1200;
		return;
		case 2400:
		linkStruct->baudRate = BAUDRATE_2400;
		return;
		case 4800:
		linkStruct->baudRate = BAUDRATE_4800;
		return;
		case 9600:
		linkStruct->baudRate = BAUDRATE_9600;
		return;
		case 19200:
		linkStruct->baudRate = BAUDRATE_19200;
		return;
		case 38400:
		linkStruct->baudRate = BAUDRATE_38400;
		return;
		case 57600:
		linkStruct->baudRate = BAUDRATE_57600;
		return;
		case 115200:
		linkStruct->baudRate = BAUDRATE_115200;
		return;
		default:
		printf("Will be used the default baud rate: 38400\n");
		linkStruct->baudRate = BAUDRATE_DEFAULT;
		return;
	}
}

void chooseDataPacketsSize(applicationLayer *applicationStruct) {
	int choice;

	printf("Packets size: ");

	if (scanf("%d", &choice) < 0) {
		printf("Will be used the default packet size: 100\n");
		applicationStruct->maxPacketSize = CHOICE_MAX;
		return;
	}

	if (choice >= MIN_SIZE && choice <= (CHOICE_MAX)) {
		applicationStruct->maxPacketSize = choice;
	}
	else {
		printf("Will be used the default packet size: 10000\n");
		applicationStruct->maxPacketSize = CHOICE_MAX;
	}
}

void chooseRetransmissionsMax(linkLayer *linkStruct) {
	int choice;

	printf("Maximum Retransmissions: ");

	if (scanf("%d", &choice) < 0) {
		printf("Will be used the default retransmissions number: 3\n");
		linkStruct->numTransmissions = DEFAULT_NUM_TRANSMISSIONS;
		return;
	}

	if (choice < 0) {
		printf("Will be used the default retransmissions number: 3\n");
		linkStruct->numTransmissions = DEFAULT_NUM_TRANSMISSIONS;
	}
	else {
		linkStruct->numTransmissions = choice;
	}
}

void chooseTimeOut(linkLayer *linkStruct) {
	int choice;

	printf("Time Out: ");

	if (scanf("%d", &choice) < 0) {
		printf("Will be used the default time out: 3\n");
		linkStruct->timeout = DEFAULT_TIMEOUT;
		return;
	}

	if (choice < 0) {
		printf("Will be used the default time out: 3\n");
		linkStruct->timeout = DEFAULT_TIMEOUT;
	}
	else {
		linkStruct->timeout = choice;
	}
}

void startValues(applicationLayer *applicationStruct, linkLayer *linkStruct) {
	linkStruct->sequenceNumber = MSG_0_CONTROL_FIELD;
	applicationStruct->endFile = false;
	applicationStruct->sequenceNumber = 0;
	applicationStruct->fileTransfer = 0;
}
