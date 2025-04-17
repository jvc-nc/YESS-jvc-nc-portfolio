#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"


/*
 * doClockLow:
 * Performs the Execute stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (ExecuteStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M *mreg = (M *)pregs[MREG];
   W *wreg = (W *)pregs[WREG];

   uint64_t icode = 0, valE = 0, valM = 0 , valA = 0;
   uint64_t stat = SAOK, dstE = RNONE, dstM = RNONE;

   stat = mreg->getstat()->getOutput();
   icode = mreg->geticode()->getOutput();
   valE = mreg->getvalE()->getOutput();
   valA = mreg->getvalA()->getOutput();
   dstE = mreg->getdstE()->getOutput();
   dstM = mreg->getdstM()->getOutput();
   
   uint64_t mem_address = addr(mreg);
   bool read = mem_read(mreg);
   bool write = mem_write(mreg);

   bool error;
   if (read)
   {
      valM = Memory::getInstance()->getLong(mem_address, error);
      m_valM = valM;
   }
   else if (write)
   {
      Memory::getInstance()->putLong(valA, mem_address, error);
   }
   else
   {
      valM = 0;
      m_valM = 0;
   }

   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

uint64_t MemoryStage::getvalM()
{
   return m_valM;
}


uint64_t MemoryStage::addr(M *mreg) 
{
   uint64_t icode = mreg->geticode()->getOutput();
   uint64_t valE = mreg->getvalE()->getOutput();
   uint64_t valA = mreg->getvalA()->getOutput();

   if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL || icode == IMRMOVQ) 
   {
      return valE;
   } 
   else if (icode == IPOPQ || icode == IRET) 
   {
      return valA;
   } 
   else 
   {
      return 0;
   }
}

bool MemoryStage::mem_read(M *mreg) 
{
   uint64_t icode = mreg->geticode()->getOutput();
   return (icode == IMRMOVQ || icode == IPOPQ || icode == IRET);
}

bool MemoryStage::mem_write(M *mreg) 
{
   uint64_t icode = mreg->geticode()->getOutput();
   return (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL);
}


/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];

   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, uint64_t valE, 
   uint64_t valM, uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}
