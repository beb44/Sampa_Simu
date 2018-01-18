#include <iostream>
#include <cstdint>
#include "DualSampa.h" 
#include "SampaHead.h"
using namespace std;

/*!
 *  \brief dualsampa chip emulator
 *
 *  This class provides a emulation artefacts for dualsampa chip serialisation mechanism.\n
 *  it prodives methods to build dsp generated build frames and methods to emulate serial link.
 *
 *  \param addr1 : hardware address of the first sampa chip
 *  \param addr2 : hardware address of the first sampa chip
 */

DualSampa::DualSampa(uint16_t addr1,uint16_t addr2)
{
  // creates the two samaps
  mSampas[0] = new Sampa(addr1);
  mSampas[1] = new Sampa(addr2);  
  
  mSyncPacket       = sampa_head().build_sync();
  mSyncCountdown   = 50; // no sync packet to be sent (for the moment)
  mSendCredit      = 0; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
  mInternalRef     = 0;
  m_c_data_provider  = NULL;
  mHandlerDsp        = NULL;
  mDataRegenerated = false;
} 

DualSampa::~DualSampa()
{
  cout <<"DualSampa deleted " << endl; 
  //
  // release sampas objects
  //
  delete mSampas[0];
  delete mSampas[1];
}
void DualSampa::SetInternalRef(int ref)
{
  mInternalRef= ref;
}
void DualSampa::SetDataProvider(void (*uh)(int))
{
  m_c_data_provider = uh;
}
void DualSampa::SetDataProvider(DualSampaHandler *handler)
{
  mHandlerDsp = handler;
}

void DualSampa::SelectChannel(const uint8_t sId,const uint8_t chid)
{
  if ((sId == 0) || (sId == 1)) {
    mSampas[sId]->SelectChannel(chid);
  }
 
}
/*!
 *  \brief starts a new frame
 *
 *  internal buffer of the two sampa is reset and ready to receives new data samples.\n
 *  the frame header will be automatically build when frame will be 'sent out'.\n
 *  If the previous frame has not been sent, it will be lost
 *  
 *  \param 
 */

void DualSampa::ResetFrames()
{
  mSampas[0]->ResetFrame();
  mSampas[1]->ResetFrame();
}
/*!
 *  \brief starts a new frame
 *
 *  internal buffer of a given sampa is reset and ready to receives new data samples.\n
 *  the frame header will be automatically build when frame will be 'sent out'.\n
 *  If the previous frame has not been sent, it will be lost
 *  
 *  \param sId sampa identifier
 */

void DualSampa::ResetFrame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    mSampas[sId]->ResetFrame();
  }
}

/*!
 *  \brief Add a data chunk to current frame
 *
 *  \param len  length of the data chunk
 *  \param data adresse of the buffer 
 *  
 *  \param sId sampa identifier
 */

void DualSampa::AddData(const uint8_t sId,int len, uint16_t *data)
{
  if ((sId == 0) || (sId == 1)) {
    mSampas[sId]->AddData(len,data); 
  }
}

/*!
 *  \brief send frame over serial link for the selected sampa chip
 *  
 *  \param sId sampa identifier
 */

void DualSampa::SendFrame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    mSampas[sId]->SendFrame();
  }
}

/*!
 *  \brief send frames over serial link
 *  
 */

void DualSampa::SendFrames()
{
  mSampas[0]->SendFrame();
  mSampas[1]->SendFrame();
}

void DualSampa::RegenerateData()
{
  mDataRegenerated = true;
  // reset credit
  mSendCredit    = 8192; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
}

/*!
 *  \brief force sampa to insert a sunc packet in current frame
 *  
 *  \param sId sampa identifier
 */


void DualSampa::MakeSync(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    mSampas[sId]->MakeSync();
  }
}

/*!
 *  \brief check data availability
 *  
 *  \return true is some data is available, false otherwise
 */

bool DualSampa::SerialAvailable()
{
  // check credit exhausted
   // check if credit is exhausted
  if (mSendCredit == 0)  {
    // 
    // if user handler available, call it to regenerate date
    //
    if (m_c_data_provider) {  
      mDataRegenerated = false;
      m_c_data_provider(mInternalRef);
      return mDataRegenerated;
    }
    if (mHandlerDsp)  {  
      mDataRegenerated = false;
      mHandlerDsp->DspHandler(mInternalRef);
      return mDataRegenerated;
    }
    // no more data available
    return false;	 
  }
  // data available
  return true;
}

/*!
 *  \brief fetch a data bit
 *  
 *  \return data bit in the LSB bit of returned value
 */

uint8_t DualSampa::GetSerial()
{
  // check if credit is exhausted
  if (mSendCredit == 0) 
   return 0xff; 
  mSendCredit --;
  // 
  // check if sync packet is being transmitted
  //
  if (mSyncCountdown) {
    mSyncCountdown--;
    return (mSyncPacket>>(49-mSyncCountdown)) & 1;
  }
  //
  // check normal data packets
  //
  if (mSampas[0]->SerialAvailable()) return mSampas[0]->GetSerial();
  if (mSampas[1]->SerialAvailable()) return mSampas[1]->GetSerial();
  //
  // no data packet available, send out sync packet
  //
  mSyncCountdown = 49; // after send first sync bit, there will remain
                         // 49 bits
  return (mSyncPacket>>0) & 1;
}
