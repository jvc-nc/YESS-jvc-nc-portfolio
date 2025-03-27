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
#include "FetchStage.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"
#include "Tools.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   f_pc = selectPC(freg, mreg, wreg);
   Memory * mem = Memory::getInstance();
   bool error = false;
   uint64_t readByte = mem->getByte(f_pc, error);

   if (error)
   {
      stat = SADR;
   }
   else
   {
      icode = Tools::getBits(readByte, 4, 7);
      ifun = Tools::getBits(readByte, 0, 3);

      bool need_regId = needRegIds(icode);
      bool need_valC = needValC(icode);
      
      valP = PCincrement(f_pc, need_regId, need_valC);

      freg->getpredPC()->setInput(predictPC(icode, valC, valP));

      getRegIds(f_pc, icode, rA, rB, need_regId);
      buildValC(f_pc, icode, valC, need_regId, need_valC);
   }

   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);

   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register instances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   freg->getpredPC()->normal();
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
}

u_int64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg)
{
   uint64_t M_icode = mreg->geticode()->getOutput();
   uint64_t M_Cnd = mreg->getCnd()->getOutput();
   uint64_t M_valA = mreg->getvalA()->getOutput();
   uint64_t W_icode = wreg->geticode()->getOutput();
   uint64_t W_valM = wreg->getvalM()->getOutput();
   uint64_t F_predPC = freg->getpredPC()->getOutput();

   if (M_icode == IJXX && !M_Cnd)
   {
      return M_valA;
   }
   else if (W_icode == IRET)
   {
      return W_valM;
   }
   else
   {
      return F_predPC;
   }
}

bool FetchStage::needRegIds(uint64_t f_icode)
{
   if (f_icode == IRRMOVQ || f_icode == IOPQ || f_icode == IPUSHQ || f_icode == IPOPQ || 
      f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ)
   {
      return true;
   }
   return false;
}

void FetchStage::getRegIds(uint64_t f_pc, uint64_t icode, uint64_t & rA, uint64_t & rB, bool need_regId)
{
   if (need_regId)
   {
      bool error = false;
      uint64_t regByte = Memory::getInstance()->getByte(f_pc + 1, error);
      rA = Tools::getBits(regByte, 4, 7);
      rB = Tools::getBits(regByte, 0, 3);
   }
}

bool FetchStage::needValC(uint64_t f_icode)
{
   if (f_icode == IRRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || 
      f_icode == IJXX || f_icode == ICALL)
   {
      return true;
   }
   return false;
}

void FetchStage::buildValC(uint64_t f_pc, uint64_t icode, uint64_t & valC, bool need_regId, bool need_valC)
{
   if (need_valC)
   {
      if (need_regId)
      {
         f_pc += 2;
      }
      else 
      {
         f_pc++;
      }

      bool error = false;
      valC = Memory::getInstance()->getLong(f_pc, error);
   }
}

uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
   if (f_icode == IJXX || f_icode == ICALL)
   {
      return f_valC;
   }
   return f_valP;
}

uint64_t FetchStage::PCincrement(uint64_t f_pc, bool needRegIds, bool needValC)
{
   if (needRegIds && needValC)
   {
      return f_pc + 10;
   }
   else if (!needRegIds && needValC)
   {
      return f_pc + 9;
   }
   else if (needRegIds && !needValC)
   {
      return f_pc + 2;
   }
   else 
   {
      return f_pc + 1;
   }
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}
