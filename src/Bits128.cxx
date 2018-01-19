#include "Bits128.h"
Bits128::Bits128()
{
  Bits128(0,0,0,0);
}
Bits128::Bits128(uint32_t m3,uint32_t m2,uint32_t m1,uint32_t m0)
{
    w[0]= m0;
    w[1]= m1;
    w[2]= m2;
    w[3]= m3;
}
uint8_t Bits128::Get(int pos)
{
  return  (w[pos/32] >> (pos %32))& 1;
}
void Bits128::Set(int pos,uint8_t val)
{
  w[pos/32] = w[pos/32] & ~(1<<(pos %32)); // clear bit
  w[pos/32] = w[pos/32] | (val<<(pos %32)); // set value
}
