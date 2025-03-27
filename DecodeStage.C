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
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"


/*
 * doClockLow:
 * Performs the Decode stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (DecodeStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    D *dreg = (D *)pregs[DREG];
    E *ereg = (E *)pregs[EREG];

    uint64_t icode = 0, ifun = 0, valA = 0, valB = 0, valC = 0, valP = 0;
    uint64_t stat = SAOK, rA = RNONE, rB = RNONE, dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE;

    stat = dreg->getstat()->getOutput();
    icode = dreg->geticode()->getOutput();
    ifun = dreg->getifun()->getOutput();
    rA = dreg->getrA()->getOutput();
    rB = dreg->getrB()->getOutput();
    valC = dreg->getvalC()->getOutput();
    valP = dreg->getvalP()->getOutput();

    setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);

    return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    E *ereg = (E *)pregs[EREG];

    ereg->getstat()->normal();
    ereg->geticode()->normal();
    ereg->getifun()->normal();
    ereg->getvalC()->normal();
    ereg->getvalA()->normal();
    ereg->getvalB()->normal();
    ereg->getdstE()->normal();
    ereg->getdstM()->normal();
    ereg->getsrcA()->normal();
    ereg->getsrcB()->normal();
}

uint64_t DecodeStage::d_srcA(D * dreg, uint64_t D_rA)
{
    uint64_t D_icode = dreg->geticode()->getOutput();

    if (D_icode == IRRMOVQ || D_icode == IRMMOVQ || D_icode == IOPQ || D_icode == IPUSHQ)
    {
        return D_rA;
    }
    else if (D_icode == IPOPQ || D_icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::d_srcB(D * dreg, uint64_t D_rB)
{
    uint64_t D_icode = dreg->geticode()->getOutput();

    if (D_icode == IOPQ || D_icode == IRMMOVQ || D_icode == IMRMOVQ)
    {
        return D_rB;
    }
    else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::d_dstE(D * dreg, uint64_t D_rB)
{
    uint64_t D_icode = dreg->geticode()->getOutput();

    if (D_icode == IRRMOVQ || D_icode == IIRMOVQ || D_icode == IOPQ)
    {
        return D_rB;
    }
    else if (D_icode == IPUSHQ || D_icode == IPOPQ || D_icode == ICALL || D_icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

uint64_t DecodeStage::d_dstM(D * dreg, uint64_t D_rA)
{
    uint64_t D_icode = dreg->geticode()->getOutput();

    if (D_icode == IMRMOVQ || D_icode == IPOPQ)
    {
        return D_rA;
    }
    else
    {
        return RNONE;
    }
}



/* setInput
 * provides the input to potentially be stored in the E register
 * during doClockHigh
 *
*/
void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode, 
    uint64_t ifun, uint64_t valC, uint64_t valA,  uint64_t valB, 
    uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB)
{
    ereg->getstat()->setInput(stat);
    ereg->geticode()->setInput(icode);
    ereg->getifun()->setInput(ifun);
    ereg->getvalC()->setInput(valC);
    ereg->getvalA()->setInput(valA);
    ereg->getvalB()->setInput(valB);
    ereg->getdstE()->setInput(dstE);
    ereg->getdstM()->setInput(dstM);
    ereg->getsrcA()->setInput(srcA);
    ereg->getsrcB()->setInput(srcB);
}
