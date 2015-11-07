#include "LinkLayer.h"

int stuffDataBCC(char *data, int length, char *stuffedData) {
	int dataI = 0;
	//os campos atrás já foram ocupados
	int stuffedDataI = DATA_START_INDEX;
	char bcc;

	for (; dataI < length; dataI++) {
		bcc = bcc ^ data[dataI];
		switch(data[dataI])
		{
			case FLAG:
				stuffedData[stuffedDataI] = FIRST_ESCAPE;
				stuffedDataI++;
				stuffedData[stuffedDataI] = FLAG_ESCAPE;
				break;
			case FIRST_ESCAPE:
				stuffedData[stuffedDataI] = FIRST_ESCAPE;
				stuffedDataI++;
				stuffedData[stuffedDataI] = SECOND_ESCAPE;
				break;
			default:
				stuffedData[stuffedDataI] = data[dataI];
				break;
		}
		
		stuffedDataI++;
	}

	switch(bcc)
		{
			case FLAG:
				stuffedData[stuffedDataI] = FIRST_ESCAPE;
				stuffedDataI++;
				stuffedData[stuffedDataI] = FLAG_ESCAPE;
				break;
			case FIRST_ESCAPE:
				stuffedData[stuffedDataI] = FIRST_ESCAPE;
				stuffedDataI++;
				stuffedData[stuffedDataI] = SECOND_ESCAPE;
				break;
			default:
				stuffedData[stuffedDataI] = bcc;
				break;
		}
		stuffedDataI++;

	return stuffedDataI;
}

int llwrite(char *buffer, int length, linkLayer *linkStruct) {

	resetNumTransmissionsGlobal();

    generate4InitialBytesLLWR(linkStruct->frame, linkStruct->sequenceNumber);
    
    linkStruct->frameSize = stuffDataBCC(buffer, length, linkStruct->frame) + 1;

    linkStruct->frame[linkStruct->frameSize-1] = FLAG;

    int charWrite = 0;

    int sizeReadMsg;
    char readMsg[FIELD_MAX_SIZE];

    while (numTransmissionsGlobalGreaterThanZero()) {

		charWrite = sendInformation(linkStruct->fd, linkStruct->frame, linkStruct->frameSize);

		if (stateMachineReadWrite(linkStruct->fd, linkStruct->timeout,
									readMsg, &sizeReadMsg,
									linkStruct->sequenceNumber)) {
			linkStruct->sequenceNumber = (char) (linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD);
			return charWrite;
		}
		infoFieldReSentGlobal++;
    }

    resetNumTransmissionsGlobal();
    return EXIT_ERROR;
}

int llread(char *buffer, linkLayer *linkStruct) {

	resetNumTransmissionsGlobal();

	srand(time(NULL));
	int sizeRet;
	while (numTransmissionsGlobalGreaterThanZero()) {
		if (stateMachineRead(&sizeRet, buffer, linkStruct) && !((rand() % ERROR_MAX) < ERROR_ODD)) {
			if (checkDisc(linkStruct->frame, linkStruct->frameSize)) {
				return DISC_EXIT;
			}

			if (checkSet(linkStruct)) {
				sendUA(linkStruct->fd);
				return SET_EXIT;
			}

			linkStruct->sequenceNumber = linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD;
			sendRR(linkStruct->fd, linkStruct->sequenceNumber);

			return sizeRet;
		}
		else {
			processDamagedDataI(linkStruct);
		}
	}

	if (!numTransmissionsGlobalGreaterThanZero()) {
		return EXIT_ERROR;
	}

	return SAME_MESSAGE;
}

void processDamagedDataI(linkLayer *linkStruct) {
	if (linkStruct->frameSize < (DATA_START_INDEX - 1)) {
		sendREJ(linkStruct->fd, linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD);
		return;
	}

	if (!verifyIntegrityOfBeginning(linkStruct->frame[FLAG_INITIAL_INDEX], 
									linkStruct->frame[ADDRESS_FIELD_INDEX],
									linkStruct->frame[CONTROL_FIELD_INDEX], 
									linkStruct->frame[BCC1_FIELD_INDEX]))
	{
		sendREJ(linkStruct->fd, linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD);
		return;
	}

	if (linkStruct->frame[ADDRESS_FIELD_INDEX] != CMD_TRA_ANS_REC_ADDRESS_FIELD) {
		sendREJ(linkStruct->fd, linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD);
		return;
	}

	if (linkStruct->frame[CONTROL_FIELD_INDEX] != linkStruct->sequenceNumber) {
		sendRR(linkStruct->fd, linkStruct->sequenceNumber);
		return;
	}
	else {
		sendREJ(linkStruct->fd, linkStruct->sequenceNumber ^ MSG_1_CONTROL_FIELD);
		return;
	}

	return;
}

bool verifyIntegrityOfBeginning(char flag, char address, char control, char bcc) {
	return (flag == FLAG && bcc == (address ^ control));
}

void sendREJ(int fd, char sequenceNumber) {
	char frame[10];

	rejNumGlobal++;
	generateInitialFromREJ(frame, sequenceNumber);

	sendInformation(fd, frame, LENGTH_SU);
}

void sendRR(int fd, char sequenceNumber) {
	char frame[10];

	generateInitialFromRR(frame, sequenceNumber);

	sendInformation(fd, frame, LENGTH_SU);
}

bool checkDisc(char *frame, int frameSize) {
	if (frameSize < LENGTH_SU) {
		return false;
	}

	if (frame[FLAG_INITIAL_INDEX] != FLAG) {
		return false;
	}

	if (frame[ADDRESS_FIELD_INDEX] != CMD_TRA_ANS_REC_ADDRESS_FIELD) {
		return false;
	}

	if (frame[BCC1_FIELD_INDEX] !=
		(frame[ADDRESS_FIELD_INDEX] ^ frame[CONTROL_FIELD_INDEX]))
	{
		return false;
	}

	if (frame[CONTROL_FIELD_INDEX] == DISC_CONTROL_FIELD) {
		return true;
	}

	return false;
}

bool checkSet(linkLayer *linkStruct) {
	if (linkStruct->frameSize < LENGTH_SU) {
		return false;
	}

	if (linkStruct->frame[FLAG_INITIAL_INDEX] != FLAG) {
		return false;
	}

	if (linkStruct->frame[ADDRESS_FIELD_INDEX] != CMD_TRA_ANS_REC_ADDRESS_FIELD) {
		return false;
	}

	if (linkStruct->frame[BCC1_FIELD_INDEX] !=
		(linkStruct->frame[ADDRESS_FIELD_INDEX] ^ linkStruct->frame[CONTROL_FIELD_INDEX]))
	{
		return false;
	}

	if (linkStruct->frame[CONTROL_FIELD_INDEX] == SET_CONTROL_FIELD) {
		return true;
	}

	return false;
}

bool llopen(char *portName, int option, linkLayer *linkStruct) {

	resetNumTransmissionsGlobal();

	int fd = openPort(portName);
	if (fd == EXIT_ERROR) {
		return false;
	}

	linkStruct->fd = fd;

	setUpPort(linkStruct);

	if (option == TRANSMITTER) {
		sendSet(fd);
		if (!stateMachineUA(linkStruct->fd, linkStruct->timeout, TRANSMITTER)) {
			return false;
		}
	}
	else {
		if (!stateMachineSet(linkStruct->fd, linkStruct->timeout)) {
			return false;
		}
		sendUA01(linkStruct->fd);
	}

	return true;
}

int llclose(linkLayer *linkStruct, int option) {

	int ret;
	resetNumTransmissionsGlobal();

	if (option == TRANSMITTER) {
		ret = closeTransmitter(linkStruct);
	}
	else {
		ret = closeReceiver(linkStruct);
	}

	closePort(linkStruct->fd, linkStruct->savetio);

	return ret;
}

int closeTransmitter(linkLayer *linkStruct) {
	int ret;
	while (numTransmissionsGlobalGreaterThanZero()) {
		if (stateMachineDisc(linkStruct->fd, linkStruct->timeout)) {
			sendUA01(linkStruct->fd);
			ret = SUCCESS_EXIT;
			break;
		}
		else {
			ret = EXIT_ERROR;
		}
	}

	return ret;
}

int closeReceiver(linkLayer *linkStruct) {
	sendDiscReceiver(linkStruct->fd);
	int ret;
	while (numTransmissionsGlobalGreaterThanZero()) {
		if (stateMachineUA(linkStruct->fd, linkStruct->timeout, RECEIVER)) {
			ret = SUCCESS_EXIT;
			break;
		}
		else {
			ret = EXIT_ERROR;
		}
	}

	return ret;
}