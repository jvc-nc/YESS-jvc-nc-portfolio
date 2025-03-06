CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab5.o Memory.o Tools.o RegisterFile.o \
Loader.o ConditionCodes.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab5: $(OBJ)

lab5.o: Memory.h RegisterFile.h ConditionCodes.h \
	Loader.h

Loader.0: Loader.h Memory.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: Tools.h RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

clean:
	rm $(OBJ) lab5

run:
	make clean
	make lab5
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *

