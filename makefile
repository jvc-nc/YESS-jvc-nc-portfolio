CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab4.o MemoryTester.o Memory.o Tools.o RegisterFile.o \
RegisterFileTester.o ConditionCodes.o ConditionCodesTester.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab4: $(OBJ)

lab4.o: Memory.h RegisterFile.h MemoryTester.h \
	RegisterFileTester.h ConditionCodes.h ConditionCodesTester.h

MemoryTester.o: Memory.h MemoryTester.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: Tools.h RegisterFile.h

RegisterFileTester.o: RegisterFile.h RegisterFileTester.h

ConditionCodes.o: ConditionCodes.h Tools.h

ConditionCodesTester.o: ConditionCodes.h ConditionCodesTester.h

clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *

