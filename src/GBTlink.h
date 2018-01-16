#ifndef GBTLINK
#define GBTLINK
#include <cstdint>
#include <bitset>
#include <list>
class gbtlink
{
public:
  ~gbtlink() {}
  virtual bool gbtword_available()=0; 
  virtual std::list<std::bitset<128>> get_word()=0;
};
#endif
