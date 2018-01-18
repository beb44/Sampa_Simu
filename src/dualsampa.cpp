#include <iostream>
#include <cstdint>
#include "dualsampa.h" 
#include "sampa_head.h"
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

dualsampa::dualsampa(uint16_t addr1,uint16_t addr2)
{
  // creates the two samaps
  m_sampas[0] = new sampa(addr1);
  m_sampas[1] = new sampa(addr2);  
  
  m_syncpacket       = sampa_head().build_sync();
  m_sync_countdown   = 50; // no sync packet to be sent (for the moment)
  m_send_credit      = 0; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
  m_internal_ref     = 0;
  m_c_data_provider  = NULL;
  handler_dsp        = NULL;
  m_data_regenerated = false;
} 

dualsampa::~dualsampa()
{
  cout <<"DualSampa deleted " << endl; 
  //
  // release sampas objects
  //
  delete m_sampas[0];
  delete m_sampas[1];
}
void dualsampa::set_internal_ref(int ref)
{
  m_internal_ref= ref;
}
void dualsampa::set_data_provider(void (*uh)(int))
{
  m_c_data_provider = uh;
}
void dualsampa::set_data_provider(dualsampa_handler *handler)
{
  handler_dsp = handler;
}

void dualsampa::select_channel(const uint8_t sId,const uint8_t chid)
{
  if ((sId == 0) || (sId == 1)) {
    m_sampas[sId]->select_channel(chid);
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

void dualsampa::reset_frames()
{
  m_sampas[0]->reset_frame();
  m_sampas[1]->reset_frame();
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

void dualsampa::reset_frame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    m_sampas[sId]->reset_frame();
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

void dualsampa::add_data(const uint8_t sId,int len, uint16_t *data)
{
  if ((sId == 0) || (sId == 1)) {
    m_sampas[sId]->add_data(len,data); 
  }
}

/*!
 *  \brief send frame over serial link for the selected sampa chip
 *  
 *  \param sId sampa identifier
 */

void dualsampa::send_frame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    m_sampas[sId]->send_frame();
  }
}

/*!
 *  \brief send frames over serial link
 *  
 */

void dualsampa::send_frames()
{
  m_sampas[0]->send_frame();
  m_sampas[1]->send_frame();
}

void dualsampa::regenerate_data()
{
  m_data_regenerated = true;
  // reset credit
  m_send_credit    = 8192; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
}

/*!
 *  \brief force sampa to insert a sunc packet in current frame
 *  
 *  \param sId sampa identifier
 */


void dualsampa::make_sync(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1)) {
    m_sampas[sId]->make_sync();
  }
}

/*!
 *  \brief check data availability
 *  
 *  \return true is some data is available, false otherwise
 */

bool dualsampa::serial_available()
{
  // check credit exhausted
   // check if credit is exhausted
  if (m_send_credit == 0)  {
    // 
    // if user handler available, call it to regenerate date
    //
    if (m_c_data_provider) {  
      m_data_regenerated = false;
      m_c_data_provider(m_internal_ref);
      return m_data_regenerated;
    }
    if (handler_dsp)  {  
      m_data_regenerated = false;
      handler_dsp->dsp_handler(m_internal_ref);
      return m_data_regenerated;
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

uint8_t dualsampa::get_serial()
{
  // check if credit is exhausted
  if (m_send_credit == 0) 
   return 0xff; 
  m_send_credit --;
  // 
  // check if sync packet is being transmitted
  //
  if (m_sync_countdown) {
    m_sync_countdown--;
    return (m_syncpacket>>(49-m_sync_countdown)) & 1;
  }
  //
  // check normal data packets
  //
  if (m_sampas[0]->serial_available()) return m_sampas[0]->get_serial();
  if (m_sampas[1]->serial_available()) return m_sampas[1]->get_serial();
  //
  // no data packet available, send out sync packet
  //
  m_sync_countdown = 49; // after send first sync bit, there will remain
                         // 49 bits
  return (m_syncpacket>>0) & 1;
}
