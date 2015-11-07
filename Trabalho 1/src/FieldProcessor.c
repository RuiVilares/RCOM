#include "FieldProcessor.h"

char getREJfromSN(char sequenceNumber) {
	switch (sequenceNumber) {
		case MSG_0_CONTROL_FIELD:
			return REJ_0_CONTROL_FIELD;
		case MSG_1_CONTROL_FIELD:
			return REJ_1_CONTROL_FIELD;
		default:
			fprintf(stderr, "Sequence number wrong: %d\n", sequenceNumber);
			return EXIT_ERROR;
	}
}

char getRRfromSN(char sequenceNumber) {
	switch (sequenceNumber) {
		case MSG_0_CONTROL_FIELD:
			return RR_0_CONTROL_FIELD;
		case MSG_1_CONTROL_FIELD:
			return RR_1_CONTROL_FIELD;
		default:
			fprintf(stderr, "Sequence number wrong: %d\n", sequenceNumber);
			return EXIT_ERROR;
	}
}

void generate4InitialBytesLLWR(char *field, int control) {

	field[FLAG_INITIAL_INDEX] = FLAG;
	field[ADDRESS_FIELD_INDEX] = CMD_TRA_ANS_REC_ADDRESS_FIELD;
	field[CONTROL_FIELD_INDEX] = control;
	field[BCC1_FIELD_INDEX] = CMD_TRA_ANS_REC_ADDRESS_FIELD ^ control;
}

void generateInitialFromRR(char *field, char sequenceNumber) {
	
	generate4InitialBytesLLWR(field, getRRfromSN(sequenceNumber));

	field[FLAG_SU_FINAL_INDEX] = FLAG;
}

void generateInitialFromREJ(char *field, char sequenceNumber) {
	
	generate4InitialBytesLLWR(field, getREJfromSN(sequenceNumber));
	
	field[FLAG_SU_FINAL_INDEX] = FLAG;
}

bool checkIntegrityAndControlRRandREJ(char *buffer, int size, char sequenceNumber) {
	
	if (size > FLAG_INITIAL_INDEX && buffer[FLAG_INITIAL_INDEX] != FLAG) {
		return false;
	}

	if (size > ADDRESS_FIELD_INDEX && buffer[ADDRESS_FIELD_INDEX] != CMD_TRA_ANS_REC_ADDRESS_FIELD) {
		return false;
	}

	if (size > BCC1_FIELD_INDEX &&
		buffer[BCC1_FIELD_INDEX] != (buffer[ADDRESS_FIELD_INDEX] ^ buffer[CONTROL_FIELD_INDEX])) {
		return false;
	}

	if (buffer[CONTROL_FIELD_INDEX] == RR_0_CONTROL_FIELD ||
		buffer[CONTROL_FIELD_INDEX] == RR_1_CONTROL_FIELD)
	{
		if (getRRfromSN(sequenceNumber) != buffer[CONTROL_FIELD_INDEX]) {
			return true;
		}
	}
	
	rejNumGlobal++;
	return false;
}