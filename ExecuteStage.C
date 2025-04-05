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
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   E *ereg = (E *)pregs[EREG];
   M *mreg = (M *)pregs[MREG];
   W *wreg = (W *)pregs[WREG];

   uint64_t icode = 0, ifun = 0, valA = 0, valB = 0, valC = 0, valE = 0;
   bool e_Cnd = false;
   uint64_t stat = SAOK, dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE;

   stat = ereg->getstat()->getOutput();
   icode = ereg->geticode()->getOutput();
   ifun = ereg->getifun()->getOutput();
   valC = ereg->getvalC()->getOutput();
   valA = ereg->getvalA()->getOutput();
   valB = ereg->getvalB()->getOutput();
   dstE = ereg->getdstE()->getOutput();
   dstM = ereg->getdstM()->getOutput();
   srcA = ereg->getsrcA()->getOutput();
   srcB = ereg->getsrcB()->getOutput();

   uint64_t aluA_val = aluA(icode, valA, valC);
   uint64_t aluB_val = aluB(icode, valB);
   uint64_t alufun_val = alufun(icode, ifun);

   e_valE_ = alu(aluA_val, aluB_val, alufun_val);
   valE = e_valE_;

   uint64_t m_stat = mreg->getstat()->getOutput();
   uint64_t W_stat = wreg->getstat()->getOutput();

   if (set_cc(icode)) 
   {
      cc(icode, ifun, valA, valB);
   }
   e_Cnd = true;

   e_dstE_ = e_dstE(icode, e_Cnd, dstE);

   setMInput(mreg, stat, icode, (uint64_t)e_Cnd, e_valE_, valA, e_dstE_, dstM);
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
   if (E_icode == IRMMOVQ || E_icode == IMRMOVQ || E_icode == IOPQ 
      || E_icode == ICALL || E_icode == IPUSHQ || E_icode == IRET 
      || E_icode == IPOPQ)
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
   return E_icode == IOPQ;
}

uint64_t ExecuteStage::e_dstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE)
{
   if (E_icode == IRRMOVQ &&  !e_Cnd)
   {
      return RNONE;
   }
   else return E_dstE;
}

void ExecuteStage::cc(uint64_t E_icode, uint64_t E_ifun, uint64_t E_valA, uint64_t E_valB)
{
   bool error = false;
   uint64_t result = 0;
   uint64_t cc = 0;

   if (set_cc(E_icode))
   {
      result = alu(aluA(E_icode, E_valA, E_valB), aluB(E_icode, E_valB), alufun(E_icode, E_ifun));
      ConditionCodes *ccInstance = ConditionCodes::getInstance();
      cc = ccInstance->getConditionCode(result, error);
      ccInstance->setConditionCode(cc, E_icode, error);
   }
}

uint64_t ExecuteStage::alu(uint64_t opA, uint64_t opB, uint64_t alufun)
{
   switch (alufun)
   {
      case ADDQ:
         return opA + opB;
      case SUBQ:
         return opA - opB;
      case XORQ:
         return opA ^ opB;
      case ANDQ:
         return opA & opB;
      default:
         return 0;
   }
}


/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
   M * mreg = (M *) pregs[MREG];

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
 *
*/
void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
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
