#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "gbtlink.h"
#include "elink.h"

class gbt_s: public gbtlink
{
public:
  gbt_s();
  void plug_elink(const uint8_t socket,elink *peer);
  void process();
  
  bool              gbtword_available();
  std::bitset<128>  get_word();
private:
  static const int   maxsocket = 40;
  elink                        *m_elinkmap[maxsocket];
  std::bitset<128>             m_cur_word;
  std::list<std::bitset<128>>  m_send_list;
  int                          m_nb_rec;
   
};
#endif
