#ifndef DEFAULT_VALUES
#define DEFAULT_VALUES

//Ficheiro
#define MIN_SIZE 100
#define CHOICE_MAX 10000
#define MAX_SIZE 20000
#define FIELD_MAX_SIZE 45000 //tem que ser maior que o dobro de MAX_SIZE
#define FILE_MAX_SIZE 25000000 //bytes
#define FILE_PERMISSIONS 0777

//Probabilidades de erro
#define ERROR_ODD 0 //0 a 50
#define ERROR_MAX 101 //100%

//Máquina de estados da Application Layer
#define SET_UP 0
#define TRANSFER 1
#define CLOSE 2
#define FINISH_PROGRAM 3

//Máquina de estados dentro da transferência
#define TRANSFER_INITIAL 0
#define TRANSFER_DATA 1
#define TRANSFER_END 2

//transmissor ou recetor
#define RECEIVER_ARGS 2
#define TRANSMITTER_ARGS 3
#define PORT_NAME_ARG 1
#define PATH_ARG 2

//Pacote de controlo
#define START 1
#define END 2
#define FILE_SIZE_INDEX 0
#define NAME_INDEX 1
#define PACKET_SIZE_INDEX 2
#define PACKET_CONTROL_FIELD_INDEX 0
#define FIELD_1 1

//Pacote de dados
#define DATA 0
#define SN_INDEX 1
#define L2_INDEX 2
#define L1_INDEX 3
#define PACKET_DATA_INFO_SIZE 4
#define PACKET_DATA_START_INDEX 4
#define MOD_SN 255

//Alarme
#define CANCEL_ALARM 0

//Caminhos das Portas
#define PORT0 "/dev/ttyS0"
#define PORT1 "/dev/ttyS1"

//Valores da Baud Rate
#define BAUDRATE_DEFAULT B38400
#define BAUDRATE_300 B300
#define BAUDRATE_600 B600
#define BAUDRATE_1200 B1200
#define BAUDRATE_2400 B2400
#define BAUDRATE_4800 B4800
#define BAUDRATE_9600 B9600
#define BAUDRATE_19200 B19200
#define BAUDRATE_38400 B38400
#define BAUDRATE_57600 B57600
#define BAUDRATE_115200 B115200

#define PORT_PERMISSIONS 0777
#define DEFAULT_TIMEOUT 3
#define DEFAULT_NUM_TRANSMISSIONS 3
#define ERROR 0xFF
#define TRANSMITTER 0
#define RECEIVER 1
#define CMD 0
#define ANS 1

//Retornos default de algumas funções
#define SAME_MESSAGE 0
#define EXIT_ERROR -1
#define DISC_EXIT -2
#define SET_EXIT -3
#define SUCCESS_EXIT 1
#define WRITE_AGAIN_INTERVAL 1

//Link Layer Defaults
#define FLAG 0x7E
#define CMD_TRA_ANS_REC_ADDRESS_FIELD 0x03 //comandos enviados pelo Emissor (TRAnsmitter) e respostas enviadas pelo Recetor (RECeiver)
#define CMD_REC_ANS_TRA_ADDRESS_FIELD 0X01 //comandos enviados pelo Recetor e repostas enviadas pelo Emissor
#define FIRST_ESCAPE 0x7D
#define FLAG_ESCAPE 0x5E
#define SECOND_ESCAPE 0x5D

//Indices comuns a ambas as tramas
#define FLAG_INITIAL_INDEX 0
#define ADDRESS_FIELD_INDEX 1
#define CONTROL_FIELD_INDEX 2
#define BCC1_FIELD_INDEX 3

//Tramas de informação
#define MSG_0_CONTROL_FIELD 0x00 //campo de controlo com valor 0
#define MSG_1_CONTROL_FIELD 0x20 //campo de controlo com valor 1
#define DATA_START_INDEX 4

//Tramas de supervisão e não numeradas
#define LENGTH_SU 5
#define FLAG_SU_FINAL_INDEX 4
#define SET_CONTROL_FIELD 0x07 //set up
#define DISC_CONTROL_FIELD 0X0B //disconnect
#define UA_CONTROL_FIELD 0x03 //unnumbered acknowledgment
#define RR_0_CONTROL_FIELD 0x01 //receiver ready/positive ACK com valor 0
#define RR_1_CONTROL_FIELD 0x21 //receiver ready/positive ACK com valor 1
#define REJ_0_CONTROL_FIELD 0x05 //reject/negative ACK com valor 0
#define REJ_1_CONTROL_FIELD 0x25 //reject/negative ACK com valor 1

//Valores predefenidos struct termios
#define INTER_CHARACTER_TIMER_UNUSED 1
#define BLOCKING_READ_CHAR 0
#define NON_CANONICAL_INPUT 0

//Máquina de estados
#define INITIAL_SM -1
#define FLAG_INITIAL_SM_INDEX 0
#define ADDRESS_SM_INDEX 1
#define CONTROL_SM_INDEX 2
#define BCC_SM_INDEX 3
#define FLAG_FINAL_SM_INDEX 4

#endif