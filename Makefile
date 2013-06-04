all: project

OBJ=application.o callbacks.o interface.o location.o main.o profile.o support.o system.o winaddptr.o winmain.o winvalue.o
SRCS=$(OBJ:%.o=%.cpp)

project: $(OBJ)
	gcc -o $@ $(OBJ)

%.o: %.c
	gcc -Iglib -Wall -g -c -o $@ $< 

clean:
	del *.o

