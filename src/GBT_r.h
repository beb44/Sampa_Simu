#ifndef GBTR
#define GBTR
#include <bitset>
#include <map>
#include "Elink.h"
#include "GBTlink.h"
class GBT_gen
{
public:
   virtual bool    fetch(int port,int sample)=0;
   virtual uint8_t read(int port,int sample)=0;
};
class GBT_elink : public elink
{
public:
  GBT_elink(int port,GBT_gen *gbt);
  bool serial_available(); 
  uint8_t get_serial(); 
private:
  GBT_gen *_gbt;
  int   _port;
  int   _sample;
};

class GBT_r: public GBT_gen
{
public:
  GBT_r(gbtlink *provider);
  elink   *get_elink(int port);
  bool    fetch(int port,int sample);
  uint8_t read(int port,int sample);
private:
  bool                         started;
  static const int             maxsocket = 40;
  gbtlink                      *data_provider;
  std::map<int,GBT_elink *>    elink_map;
  int                          cur_sample;
  int                          nb_sample_readers;
  int                          nb_links;
  std::bitset<128>  		curword;
};
#endif
