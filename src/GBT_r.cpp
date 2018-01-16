#include <cstdint>
#include "GBT_r.h"

GBT_elink::GBT_elink(int port,GBT_gen *gbt)
{
  _port   = port;
  _gbt    = gbt;
  _sample = 0;
}
bool GBT_elink::serial_available()
{
  _gbt->fetch(_port,_sample);
  return true;
}

uint8_t  GBT_elink::get_serial()
{
  return _gbt->read(_port,_sample);
}
GBT_r::GBT_r(gbtlink *provider)
{
  data_provider = provider;
  elink_map.clear();
  started = false;
}


elink * GBT_r::get_elink(int port)
{
  if (started) throw;
  if (elink_map.find(port) != elink_map.end()) throw;
  elink_map[port] = new GBT_elink(port,this);
  nb_links++;
  return elink_map[port];
}

bool GBT_r::fetch(int port,int sample)
{
  if ((sample/2) == cur_sample)
  {
    return true;
  }
  nb_sample_readers--;
  if (nb_sample_readers != 0)
  {
    // suspend task
  }
  else
  {
    // fecth a new word
    if (data_provider->gbtword_available())
    {
      data_provider->get_word();
      cur_sample++;
      // free all waiting thread
      return true;
    }
    else
      return false;
  }
}

uint8_t GBT_r::read(int port,int sample)
{
  return curword[port+(sample & 1)];
}
