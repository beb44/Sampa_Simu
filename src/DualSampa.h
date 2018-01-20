#ifndef DUALSAMPA
#define DUALSAMPA
#include <cstdint>
#include "Sampa.h"
#include "Elink.h"
/*!
 *  \brief dualsampa chip emulator
 *
 *  This class provides a emulation artefacts for dualsampa chip serialisation mechanism.\n
 *  
 */
class DualSampaHandler
{
public:
  virtual void DspHandler(int ref) = 0;
};

class DualSampa  : public Elink
{
public:
  explicit DualSampa(uint16_t addr1,uint16_t addr2);
  ~DualSampa();
  void SetInternalRef(int ref);
  void SetDataProvider(void (*uh)(int));
  void SetDataProvider(DualSampaHandler &handler);
  void SelectChannel(const uint8_t sId,const uint8_t chid);
  void ResetFrames();
  void ResetFrame(const uint8_t sId);
  void AddData(const uint8_t sId,int len, uint16_t *data);
  void SendFrame(const uint8_t sId);
  void SendFrames();
  void RegenerateData();

  void MakeSync(const uint8_t sId);
  
  bool    SerialAvailable() ; 
  uint8_t GetSerial() ;

private:
   
   DualSampa(){};       // makes constructor unavailable
   Sampa             *mSampas[2];
   
   uint64_t          mSyncPacket;
   int               mSyncCountdown;
   int               mSendCredit;
   
   int	             mInternalRef;
   void              (*m_c_data_provider)(int);
   DualSampaHandler  *mHandlerDsp;
   bool              mDataRegenerated;
};
#endif
