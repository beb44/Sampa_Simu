#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "Elink.h"

class GBT_s
{
public:
  GBT_s();
  void plug_elink(const uint8_t socket,elink *peer);
  void process();
  
  bool              gbtword_available();
  std::bitset<128>  get_word();
private:
  static const int   maxsocket = 40;
  elink              *elinkmap[maxsocket];
  std::bitset<128>   cur_word;
  std::list<std::bitset<128>>  send_list;
   
};
#endif
