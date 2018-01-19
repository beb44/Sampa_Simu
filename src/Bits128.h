#ifndef BITS128
#define BITS128
#include <cstdint>
class Bits128
{
public:
  Bits128();
  Bits128(uint32_t m3,uint32_t m2,uint32_t m1,uint32_t m0);
  uint8_t Get(int pos);
  void Set(int pos,uint8_t val);
private: 
  uint32_t  w[4];
};
#endif
