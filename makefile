CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = yess.o PipeReg.o PipeRegField.o Simulate.o FetchStage.o DecodeStage.o ExecuteStage.o \
MemoryStage.o WritebackStage.o F.o D.o E.o M.o W.o Memory.o Tools.o RegisterFile.o \
Loader.o ConditionCodes.o 

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)

yess.o: Debug.h Memory.h Loader.h RegisterFile.h ConditionCodes.h \
	PipeReg.h Stage.h Simulate.h

PipeReg.o: PipeReg.h

PipeRegField.o: PipeRegField.h

Simulate.o: Simulate.h Stage.h W.h M.h D.h F.h E.h PipeReg.h PipeRegField.h \
	RegisterFile.h Status.h Debug.h ExecuteStage.h MemoryStage.h DecodeStage.h FetchStage.h WritebackStage.h

FetchStage.o: FetchStage.h

DecodeStage.o: DecodeStage.h

ExecuteStage.o: ExecuteStage.h 

MemoryStage.o: MemoryStage.h 

WritebackStage.o: WritebackStage.h 

F.o: F.h PipeReg.h PipeRegField.h

D.o: D.h Status.h PipeRegField.h PipeReg.h RegisterFile.h Instructions.h

E.o: E.h RegisterFile.h Instructions.h PipeRegField.h PipeReg.h E.h Status.h

M.o: M.h PipeReg.h PipeRegField.h RegisterFile.h Instructions.h Status.h

W.o: W.h PipeReg.h PipeRegField.h RegisterFile.h Instructions.h Status.h

Loader.o: Loader.h Memory.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: Tools.h RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

clean:
	rm $(OBJ) yess
	rm -rf Outputs

run:
	make clean
	make yess
	./run.sh

removeR:
	sed -i -e 's/\r$$//' *

