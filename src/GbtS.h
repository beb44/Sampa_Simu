#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "GbtLink.h"
#include "Elink.h"

class GbtS: public gbtlink
{
public:
  GbtS();
  void PlugElink(const uint8_t socket,Elink *peer);
  void Process();
  
  bool              GbtWordAvailable();
  std::bitset<128>  GetWord();
private:
  static const int   mMaxSocket = 40;
  Elink                        *mElinkMap[mMaxSocket];
  std::bitset<128>             mCurWord;
  std::list<std::bitset<128>>  mSendList;
  int                          mNbRec;
   
};
#endif
