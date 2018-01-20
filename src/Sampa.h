#ifndef SAMPA
#define SAMPA
#include <cstdint>
#include <list>
#include "SampaElink.h"
/*!
 *  \brief sampa chip emulator
 *
 *  This class provides a emulation artefacts for sampa chip serialisation mechanism.\n
 *  it prodives methods to build dsp generated build frames and methods to emulate serial link.
 */
class Sampa  : public SampaElink
{
public:
  explicit Sampa(uint16_t addr); 
  ~Sampa();
  void SelectChannel(const uint8_t chid);
  void ResetFrame();
  void AddData(int const len, uint16_t const *data);
 
  void DispFrame();
  void SendFrame();

  void MakeSync();
 
  bool    SerialAvailable(); 
  uint8_t GetSerial();
  
private:

  void MakeDataHeader();

  uint16_t  mFrame[1024+5];
  uint16_t  mWPointer;
  bool      mHasHeader;
  uint8_t   mHaddr;
  uint8_t   mChannel;
  
  std::list<uint16_t *>  mSendList;
  
  bool      mDataAvailable;
  uint16_t  *mCurSendFrame;
  int       mCurWord;
  int       mCurBit;
  uint16_t  mCurLen;
  
};
#endif
