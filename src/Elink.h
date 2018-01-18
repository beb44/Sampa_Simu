#ifndef ELINK
#define ELINK
#include <cstdint>
class Elink
{
public:
  virtual ~Elink() {}
  virtual bool SerialAvailable()=0; 
  virtual uint8_t GetSerial()=0;
};
#endif
