//class to perform the combinational logic of
//the Decode stage
class DecodeStage: public Stage
{
   private:
      void setEInput(E * ereg, uint64_t stat, uint64_t icode, 
         uint64_t ifun, uint64_t valC, uint64_t valA,  uint64_t valB, 
         uint64_t dstE, uint64_t dstM, uint64_t srcA, uint64_t srcB);
      uint64_t d_srcA(D * dreg, uint64_t D_rA, uint64_t D_icode);
      uint64_t d_srcB(D * dreg, uint64_t D_rB, uint64_t D_icode);
      uint64_t d_dstE(D * dreg, uint64_t D_rB, uint64_t D_icode);
      uint64_t d_dstM(D * dreg, uint64_t D_rA, uint64_t D_icode);
      uint64_t d_valA(uint64_t d_srcA);
      uint64_t d_valB(uint64_t d_srcB);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
