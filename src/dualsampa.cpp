#include <iostream>
#include <cstdint>
#include "dualsampa.h" 
#include "sampa_head.h"
using namespace std;

dualsampa::dualsampa(uint8_t addr1,uint8_t addr2)
{
  // creates the two samaps
  sampas[0] = new sampa(addr1);
  sampas[1] = new sampa(addr2);  
  
  syncpacket     = sampa_head().build_sync();
  sync_countdown = 0; // no sync packet to be sent (for the moment)
  send_credit    = 8192; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
  internal_ref = 0;
  user_handler = 0;
  data_regenerated = false;
}

dualsampa::~dualsampa()
{
  cout <<"DualSampa deleted " << endl; 
  delete sampas[0];
  delete sampas[1];
}
void dualsampa::set_internal_ref(int ref)
{
  internal_ref= ref;
}
void dualsampa::set_user_handler(void (*uh)(int))
{
  user_handler = uh;
  // call user handler to initiate process
  uh(internal_ref);
}
void dualsampa::select_channel(const uint8_t sId,const uint8_t chid)
{
   if ((sId == 0) || (sId == 1))
  {
    sampas[sId]->select_channel(chid);
  }
 
}
void dualsampa::reset_frames()
{
  sampas[0]->reset_frame();
  sampas[1]->reset_frame();
}

void dualsampa::reset_frame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1))
  {
    sampas[sId]->reset_frame();
  }
}
void dualsampa::add_data(const uint8_t sId,int len, uint16_t *data)
{
  if ((sId == 0) || (sId == 1))
  {
    sampas[sId]->add_data(len,data); 
  }
}

void dualsampa::send_frame(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1))
  {
    sampas[sId]->send_frame();
  }
}
void dualsampa::send_frames()
{
  sampas[0]->send_frame();
  sampas[1]->send_frame();
}

void dualsampa::regenerate_data()
{
  data_regenerated = true;
  // reset credit
  send_credit    = 8192; // 8192 bit to be sent per window 
                         //(10 mhz sampling rate)
}
void dualsampa::make_sync(const uint8_t sId)
{
  if ((sId == 0) || (sId == 1))
  {
    sampas[sId]->make_sync();
  }
}
bool dualsampa::serial_available()
{
  // check credit exhausted
   // check if credit is exhausted
  if (send_credit == 0) 
  {
    // if user handler available, call it to regenerate date
    if (user_handler) 
    {  
      if ((data_regenerated))
      {
        user_handler(internal_ref);
        data_regenerated = false;
      }	
    }
    return false;
  }
 //if (send_credit == 0) return false;
  //return (sampas[0]->serial_available() || sampas[1]->serial_available());
  return true;
}

uint8_t dualsampa::get_serial()
{
  // check if credit is exhausted
  if (send_credit == 0)  return 0xff;
  send_credit --;
  // check if sync packet is being transmitted
  if (sync_countdown)
  {
    sync_countdown--;
    return (syncpacket>>sync_countdown) & 1;
  }
  // check normal data packets
  if (sampas[0]->serial_available()) return sampas[0]->get_serial();
  if (sampas[1]->serial_available()) return sampas[1]->get_serial();
  // no dta packet available, send out sync packet
  sync_countdown = 49; // after send first sync bit, there will remain
                      // 49 bits
  return (syncpacket>>sync_countdown) & 1;
}
