#The compiler
CC = gcc
#The libraries
LIBRARIES = CLI.c ApplicationLayer.c Alarm.c GlobalVariable.c FieldProcessor.c StateMachine.c LinkLayer.c Transmission.c
#All the files that will be compiled
all: main
main:
	$(CC) -Wall $(LIBRARIES) main.c -o serialPort
clean:
	rm serialPort
