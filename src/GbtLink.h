#ifndef GBTLINK
#define GBTLINK
#include <cstdint>
#include <bitset>
#include <list>
class gbtlink
{
public:
  virtual ~gbtlink() {}
  virtual bool GbtWordAvailable()=0; 
  virtual std::bitset<128> GetWord()=0;
};
#endif
