#ifndef ELINK
#define ELINK
#include <cstdint>
class elink
{
public:
  virtual bool serial_available()=0; 
  virtual uint8_t get_serial()=0;
};
#endif
