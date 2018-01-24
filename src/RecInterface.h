#ifndef RECINTERFACE
#define RECINTERFACE
#include <cstdint>
class RecInterface {
public:

  virtual void Push(uint8_t data)=0;
};
#endif
