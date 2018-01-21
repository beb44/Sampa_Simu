#ifndef DUALSAMPA
#define DUALSAMPA
#include <cstdint>
#include "Sampa.h"
#include "SampaElink.h"
class DualSampaHandler
{
public:
  virtual void DspHandler(int ref) = 0;
};

/*!
 *  \brief dualsampa chip emulator
 *
 *  This class provides a emulation artefacts for dualsampa chip serialisation mechanism.\n
 *  
 */

class DualSampa  : public SampaElink
{
public:
  explicit DualSampa(uint16_t addr1,uint16_t addr2);
  ~DualSampa();
  void SetInternalRef(int ref);
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

  void SetDataProvider(void (*uh)(int));
  void SetDataProvider(DualSampaHandler &handler);


private:
   
  DualSampa(){};       // makes constructor unavailable

  /*! \brief Max number of Elinks ports                              */
  Sampa              *mSampas[2];
   
  /*! \brief Current bit of sync packet being read out               */
  int               mSyncCountdown;

  /*! \brief number of bits still to go in the time window           */
  int               mSendCredit;

  /*! \brief prebuild sync packet                                    */
  uint64_t          mSyncPacket;
   
  /*! \brief internal reference                                      */
  int	             mInternalRef;
  
  /*! \brief data provider address (pure C interface)                */
  void              (*m_c_data_provider)(int);

  /*! \brief data provider object address (C++ interface)            */
  DualSampaHandler  *mHandlerDsp;

  /*! \brief data regenerated or not                                 */
  bool              mDataRegenerated;
};
#endif
