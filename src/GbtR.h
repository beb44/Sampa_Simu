#ifndef GBTR
#define GBTR
#include <bitset>
#include <map>
#include <mutex>
#include <condition_variable>
#include <sys/types.h>
#include <semaphore.h>
#include "elink.h"
#include "gbtlink.h"
class gbt_gen
{
public:
   virtual bool    fetch(int port,int sample)=0;
   virtual uint8_t read(int port,int sample)=0;
};
class gbt_elink : public elink
{
public:
  gbt_elink(int port,gbt_gen& gbt);
  bool serial_available(); 
  uint8_t get_serial(); 
  void  lock();
  void  unlock();
private:
  gbt_gen&    m_gbt;
  int         m_port;
  int         m_sample;
  sem_t	      m_wait_sem;
};

class gbt_r: public gbt_gen
{
public:
  gbt_r(gbtlink &provider);
  elink   &get_elink(int const port);
  bool    fetch(int const port,int const sample) ;
  uint8_t read(int const port,int const sample);
private:
  bool                         m_started;
  static const int             m_maxsocket = 40;
  gbtlink                      &m_data_provider;
  std::map<int,gbt_elink *>    m_elink_map;
  int                          m_cur_sample;
  int                          m_nb_sample_readers;
  int                          m_nb_links;
  std::bitset<128>  	       m_curword;
  bool                         m_data_available;
  std::mutex		       m_mutex;
 
};
#endif
