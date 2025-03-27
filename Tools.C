#include <cstdint>
#include <cstdlib>
#include <string>
#include "Tools.h"

/*
 * Hints/Notes:
 * 1) Pay attention to what the comments say. 
 * 2) Sometimes a function returns the source input if input validation
 *    fails and sometimes it returns 0.
 * 3) Pay attention to the "rules".
 * 4) You won't get credit for a method if it passes the tests but
 *    breaks the "rules".
 * 5) if the rule is no loops then you cannot use while, do-while, for
 * 6) if the rule is no conditionals then you cannot use if, if-else, 
 *    switch or ternary statements 
 * 7) The last 6 function restrict the operators that can be used. If
 *    used any other operators than those, you will not receive credit.
 * 8) If you shift a variable by an amount that is equal to or
 *    larger than its size, then the shift doesn't happen. For example, 
 *    if n is a 64 bit unsigned int then n >> 64 is n (not 0).
 * 9) If you need a 64 bit constant, you need to follow the constant
 *    by either ul (unsigned long), l (long), ull (unsigned long long),
 *    or ll (long long). For example, 3ul, 3l, 3ull, 3ll are all eight bytes.
 *    By default, 3 without the suffix is a 32 bit int.
 * 10) >> (right shift) shifts off low order bits
 * 11) << (left shift) shifts off high order bits
 * 12) right shift of a signed variable maintains the sign bit; 
 *     right shift of an unsigned variable brings in 0 as the sign bit
 *
 */

/** 
 * builds a 64-bit long out of an array of 8 bytes
 *
 * @param array of 8 bytes
 * @return uint64_t where the low order byte is bytes[0] and
 *         the high order byte is bytes[7]
 *
 * RULES: 
 *   1) no more than ten lines of code
 *   2) use a loop
*/
uint64_t Tools::buildLong(uint8_t bytes[LONGSIZE])
{
  uint64_t newLong = 0;
  for (int i = LONGSIZE - 1; i >= 0; --i) 
  {
    newLong = (newLong << LONGSIZE) | bytes[i];
  }
  return newLong;
}

/** 
 * accepts as input an uint64_t and returns the designated byte
 * within the uint64_t; returns 0 if the indicated byte number
 * is out of range 
 *
 * for example, getByte(0x1122334455667788, 7) returns 0x11
 *              getByte(0x1122334455667788, 1) returns 0x77
 *              getByte(0x1122334455667788, 8) returns 0
 *
 * @param uint64_t source that is the source data
 * @param int32_t byteNum that indicates the byte to return (0 through 7)
 *                byte 0 is the low order byte
 * @return 0 if byteNum is out of range
 *         byte 0, 1, .., or 7 of source if byteNum is within range
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
*/
uint64_t Tools::getByte(uint64_t source, int32_t byteNum)
{
  if (0 > byteNum || byteNum > 7) 
  {
    return 0;
  }
  uint8_t oneByte = (source >> byteNum * 8) & 0xFF;
  return oneByte;
}

/**
 * accepts as input an uint64_t and returns the bits low through 
 * high of the uint64_t. bit 0 is the low order bit and bit 63
 * is the high order bit. returns 0 if the low or high bit numbers 
 * are out of range
 *
 * for example, getBits(0x8877665544332211, 0, 7) returns 0x11
 *              getBits(0x8877665544332211, 4, 11) returns 0x21
 *              getBits(0x8877665544332211, 0, 63) returns 0x8877665544332211
 *              getBits(0x8877665544332211, 0, 64) returns 0
 *
 * @param uint64_t source that holds the bits to be grabbed and 
 *        returned
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be returned
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be returned
 * @return an uint64_t that holds a subset of the source bits
 *         that is returned in the low order bits; returns 0 if low or high 
 *         is out of range
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 */
uint64_t Tools::getBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
  {
    return 0;
  }
  uint64_t numBits = source << (63 - high);
  numBits = numBits >> ((63 - high) + low);
  return numBits;
}


/**
 * sets the bits of source in the range specified by the low and high
 * parameters to 1 and returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, setBits(0x1122334455667788, 0, 7) returns 0x11223344556677ff
 *              setBits(0x1122334455667788, 8, 0xf) returns 0x112233445566ff88
 *              setBits(0x1122334455667788, 8, 64) returns 0x1122334455667788
 *                      note: 64 is out of range
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 1
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 1
 * @return an uint64_t that holds the modified source
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 * 3) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::setBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
  {
    return source;
  }
  uint64_t mask = -1;
  mask = getBits(mask, low, high);
  mask = mask << low;
  return source | mask;
}

/**
 * clears the bits of source in the range low to high to 0 (clears them) and
 * returns that value. returns source if the low or high
 * bit numbers are out of range
 *
 * for example, clearBits(0x1122334455667788, 0, 7) returns 0x1122334455667700
 *              clearBits(0x1122334455667788, 8, f) returns 0x1122334455660088
 *
 * @param uint64_t source 
 * @param int32_t low that is the bit number of the lowest numbered
 *        bit to be set to 0
 * @param int32_t high that is the bit number of the highest numbered
 *        bit to be set to 0
 * @return an uint64_t that holds the modified source
 *
 * RULES:
 * 1) you can use an if to handle error checking on input
 * 2) no loops or conditionals (other than for 1) or switch
 * 3) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::clearBits(uint64_t source, int32_t low, int32_t high)
{
  if (low < 0 || high > 63 || low > high) 
  {
    return source;
  }
  uint64_t mask = -1;
  mask = getBits(mask, low, high);
  mask = mask << low;
  mask = ~mask;
  return source & mask;
}


/**
 * copies length bits from the source to a destination and returns the
 * modified destination. If low bit number of the source or 
 * dest is out of range or the calculated source or dest high bit 
 * number is out of range, then the unmodified destination is returned.
 *
 * for example,
 *   copyBits(0x1122334455667788, 0x8877665544332211, 0, 0, 8) 
 *           returns 0x8877665544332288
 *   copyBits(0x1122334455667788, 0x8877665544332211, 0, 8, 8) 
 *           returns 0x8877665544338811
 *
 * @param uint64_t source 
 * @param uint64_t dest 
 * @param int32_t srclow that is the bit number of the lowest numbered
 *        bit of the source to be copied
 * @param int32_t destlow that is the bit number of the lowest numbered
 *        bit of the destination to be modified
 * @param int32_t length that is the number of bits to be copied
 * @return uint64_t that is the modifed dest
 *
 * RESTRICTIONS: You can only use an if statement to determine whether
 *               the low and high values are valid. 
 */
uint64_t Tools::copyBits(uint64_t source, uint64_t dest, 
                         int32_t srclow, int32_t dstlow, int32_t length)
{
  if (srclow < 0 || srclow + length > 64 || dstlow < 0 || dstlow + length > 64) 
  {
    return dest;
  }
  uint64_t bitCopy = getBits(source, srclow, (srclow + length) - 1);
  dest = clearBits(dest, dstlow, (dstlow + length) - 1);
  bitCopy = bitCopy << dstlow;
  dest = bitCopy | dest; 
  return dest;
}


/**
 * sets the bits of source identfied by the byte number to 1 and
 * returns that value. if the byte number is out of range then source
 * is returned unchanged.
 *
 * for example, setByte(0x1122334455667788, 0) returns 0x11223344556677ff
 *              setByte(0x1122334455667788, 1) returns 0x112233445566ff88
 *              setByte(0x1122334455667788, 8) returns 0x1122334455667788
 *
 * @param uint64_t source 
 * @param int32_t byteNum that indicates the number of the byte to be
 *        set to 0xff; the low order byte is byte number 0
 * @return uint64_t that is source with byte byteNum set to 0xff
 *
 * RESTRICTIONS: You cannot use an if statement.  This means you
 *               need to come up with some clever method to get the
 *               code to return source if bytenum is out of range and
 *               the source otherwise.
 */
uint64_t Tools::setByte(uint64_t source, int32_t byteNum)
{
    uint64_t mask = (uint64_t) 0xFF << (byteNum * 8);
    uint64_t valid = (byteNum >= 0 && byteNum < 8) * -1;
    uint64_t result = source | (mask & valid);
    return result;
}


/**
 * assumes source contains a 64 bit two's complement value and
 * returns the sign (1 or 0)
 *
 * for example, sign(0xffffffffffffffff) returns 1
 *              sign(0x0000000000000000) returns 0
 *              sign(0x8000000000000000) returns 1
 *
 * @param uint64_t source
 * @return 1 if source is negative when treated as a two's complement 
 *         value and 0 otherwise
 *
 * RULES:
 * 1) no loops or conditionals
 * 2) you can use other functions you have written, for example, getBits
 */
uint64_t Tools::sign(uint64_t source)
{
  return (source >> 63) & 1;
}

/**
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur if they are summed
 * and false otherwise
 *
 * for example, addOverflow(0x8000000000000000, 0x8000000000000000) returns 1
 *              addOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 1
 *              addOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 0
 *
 * @param uint64_t op1 that is one of the operands of the addition
 * @param uint64_t op2 that is the other operand of the addition
 * @return true if op1 + op2 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 *
 * RULES:
 * 1) no loops 
 * 2) you can use other functions you have written, for example, sign
 * 3) no more than 10 lines of code
 */
bool Tools::addOverflow(uint64_t op1, uint64_t op2)
{
  uint64_t sum = op1 + op2;
  uint64_t op1Sign = sign(op1);
  uint64_t op2Sign = sign(op2);
  uint64_t opSignSum = sign(sum);
  return op1Sign == op2Sign && op1Sign != opSignSum;
}

/**
 * assumes that op1 and op2 contain 64 bit two's complement values
 * and returns true if an overflow would occur from op2 - op1
 * and false otherwise
 *
 * for example, subOverflow(0x8000000000000000, 0x8000000000000000) returns 0
 *              subOverflow(0x7fffffffffffffff, 0x7fffffffffffffff) returns 0
 *              subOverflow(0x8000000000000000, 0x7fffffffffffffff) returns 1
 *
 * @param uint64_t op1 that is one of the operands of the subtraction
 * @param uint64_t op2 that is the other operand of the subtraction
 * @return true if op2 - op1 would result in an overflow assuming that op1
 *         and op2 contain 64-bit two's complement values
 *
 * RULES:
 * 1) no loops 
 * 2) you can use other functions you have written, for example, sign
 * 3) you cannot use addOverflow (it doesn't work in all cases).
 * 4) no more than 10 lines of code
 */
bool Tools::subOverflow(uint64_t op1, uint64_t op2)
{
  uint64_t subtract = op2 - op1;
  uint64_t op1Sign = sign(op1);
  uint64_t opSignSubtract = sign(subtract);
  return op1Sign == opSignSubtract;
}
