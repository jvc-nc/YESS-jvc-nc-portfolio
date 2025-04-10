//class to perform the combinational logic of
//the Execute stage
class ExecuteStage: public Stage
{
   private:
      uint64_t e_valE_;
      uint64_t e_dstE_;
      void setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
         uint64_t e_valE_, uint64_t valA, 
         uint64_t e_dstE_, uint64_t dstM);
      uint64_t aluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC);
      uint64_t aluB(uint64_t E_icode, uint64_t E_valB);
      uint64_t alufun(uint64_t E_icode, uint64_t E_ifun);
      bool set_cc(uint64_t E_icode);
      uint64_t e_dstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE);
      void cc(uint64_t E_icode, uint64_t result, uint64_t opA, uint64_t opB, uint64_t alufun);
      uint64_t alu(uint64_t opA, uint64_t opB, uint64_t alufun);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t gete_valE();
      uint64_t gete_dstE();
};
