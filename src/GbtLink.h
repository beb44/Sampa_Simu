#ifndef GBTLINK
#define GBTLINK
#include <cstdint>
#include "Bits128.h"
#include <list>
class gbtlink
{
public:
  virtual ~gbtlink() {}
  virtual bool GbtWordAvailable()=0; 
  virtual Bits128 GetWord()=0;
};
#endif
