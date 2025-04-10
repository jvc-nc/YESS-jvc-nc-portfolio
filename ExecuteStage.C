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
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"

/*
 * doClockLow:
 * Performs the Execute stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (ExecuteStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool ExecuteStage::doClockLow(PipeReg **pregs, Stage **stages)
{
   E *ereg = (E *)pregs[EREG];
   M *mreg = (M *)pregs[MREG];

   uint64_t icode = ereg->geticode()->getOutput();
   uint64_t ifun = ereg->getifun()->getOutput();
   uint64_t valC = ereg->getvalC()->getOutput();
   uint64_t valA = ereg->getvalA()->getOutput();
   uint64_t valB = ereg->getvalB()->getOutput();
   uint64_t dstE = ereg->getdstE()->getOutput();
   uint64_t dstM = ereg->getdstM()->getOutput();

   uint64_t val_aluA = aluA(icode, valA, valC);
   uint64_t val_aluB = aluB(icode, valB);
   uint64_t val_alufun = alufun(icode, ifun);
   uint64_t valE = alu(val_aluA, val_aluB, val_alufun);

   cc(icode, valE, val_aluA, val_aluB, val_alufun);

   uint64_t e_Cnd = 0;
   if (icode == IRRMOVQ || icode == IJXX)
   {
      ConditionCodes *ccInstance = ConditionCodes::getInstance();
      bool error = false;
      e_Cnd = ccInstance->getConditionCode(ifun, error);
   }

   e_dstE_ = e_dstE(icode, e_Cnd, dstE);
   e_valE_ = valE;

   setMInput(mreg, ereg->getstat()->getOutput(), icode, e_Cnd, valE, valA, e_dstE_, dstM);

   return false;
}

uint64_t ExecuteStage::gete_dstE()
{
   return e_dstE_;
}

uint64_t ExecuteStage::gete_valE()
{
   return e_valE_;
}

uint64_t ExecuteStage::aluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC)
{
   if (E_icode == IRRMOVQ || E_icode == IOPQ)
   {
      return E_valA;
   }
   else if (E_icode == IIRMOVQ || E_icode == IRMMOVQ || E_icode == IMRMOVQ)
   {
      return E_valC;
   }
   else if (E_icode == ICALL || E_icode == IPUSHQ)
   {
      return -8;
   }
   else if (E_icode == IRET || E_icode == IPOPQ)
   {
      return 8;
   }
   else
   {
      return 0;
   }
}

uint64_t ExecuteStage::aluB(uint64_t E_icode, uint64_t E_valB)
{
   if (E_icode == IRMMOVQ || E_icode == IMRMOVQ || E_icode == IOPQ || E_icode == ICALL || E_icode == IPUSHQ || E_icode == IRET || E_icode == IPOPQ)
   {
      return E_valB;
   }
   else if (E_icode == IRRMOVQ || E_icode == IIRMOVQ)
   {
      return 0;
   }
   else
   {
      return 0;
   }
}

uint64_t ExecuteStage::alufun(uint64_t E_icode, uint64_t E_ifun)
{
   if (E_icode == IOPQ)
   {
      return E_ifun;
   }
   else
   {
      return ADDQ;
   }
}

bool ExecuteStage::set_cc(uint64_t E_icode)
{
   if (E_icode == IOPQ)
   {
      return true;
   }
   else
   {
      return false;
   }
}

uint64_t ExecuteStage::e_dstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE)
{
   if (E_icode == IRRMOVQ && !e_Cnd)
   {
      return RNONE;
   }
   else
   {
      return E_dstE;
   }
}

void ExecuteStage::cc(uint64_t E_icode, uint64_t result, uint64_t opA, uint64_t opB, uint64_t alufun)
{
   if (set_cc(E_icode))
   {
      ConditionCodes *ccInstance = ConditionCodes::getInstance();
      bool error = false;

      ccInstance->setConditionCode(result == 0, ZF, error);
      ccInstance->setConditionCode(Tools::sign(result), SF, error);

      if (alufun == ADDQ)
      {
         bool overflow = Tools::addOverflow(opA, opB);
         ccInstance->setConditionCode(overflow, OF, error);
      }
      if (alufun == SUBQ)
      {
         bool overflow = Tools::subOverflow(opA, opB);
         ccInstance->setConditionCode(overflow, OF, error);
      }
   }
}

uint64_t ExecuteStage::alu(uint64_t opA, uint64_t opB, uint64_t alufun)
{
   uint64_t result = 0;

   if (alufun == ADDQ)
   {
      result = opA + opB;
   }
   else if (alufun == SUBQ)
   {
      result = opB - opA;
   }
   else if (alufun == XORQ)
   {
      result = opA ^ opB;
   }
   else if (alufun == ANDQ)
   {
      result = opA & opB;
   }
   else
   {
      result = 0;
   }

   return result;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register instances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg **pregs)
{
   M *mreg = (M *)pregs[MREG];

   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();
}

/* setInput
 * provides the input to potentially be stored in the M register
 * during doClockHigh
 */
void ExecuteStage::setMInput(M *mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                             uint64_t valE, uint64_t valA,
                             uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}
