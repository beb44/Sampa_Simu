#include <iostream>
#include <cstdint>
#include <sys/types.h>
#include <sys/sem.h>
#include "GBT_r.h"
using namespace std;

GBT_elink::GBT_elink(int port,GBT_gen *gbt)
{
  _port   = port;
  _gbt    = gbt;
  _sample = 0;
}
bool GBT_elink::serial_available()
{
bool res = _gbt->fetch(_port,_sample);
  return res;
}

uint8_t  GBT_elink::get_serial()
{
  return _gbt->read(_port,_sample++);
}
GBT_r::GBT_r(gbtlink *provider)
{
  data_provider = provider;
  elink_map.clear();
  started = false;
  cur_sample = -1;
  _portmask = 0;
  sem_init(&_wait_sem, 0, 0);
}


elink * GBT_r::get_elink(int port)
{
  if (started) throw;
  if (elink_map.find(port) != elink_map.end()) throw;
  elink_map[port] = new GBT_elink(port,this);
  nb_links++;
  nb_sample_readers = nb_links;
  _portmask = _portmask + (1<<port);
  cur_portmask = _portmask;
  return elink_map[port];
}

bool GBT_r::fetch(int port,int sample)
{
  if (abs(elink_map[0]->_sample-elink_map[1]->_sample) >3)
  {
    int t1 = elink_map[0]->_sample;
    int t2 = elink_map[1]->_sample;
    cout << t1<< " " << t2 <<endl;
    throw(" .............");
  }
  _mutex.lock();
   cout << "-------" << port<<"----"<<sample<<"-------------- " << cur_sample << "-----"<<  nb_sample_readers<< endl;
   if ((sample/2) == cur_sample)
  {
    
    _mutex.unlock();
    return true;
  }
  nb_sample_readers--;
  if (nb_sample_readers != 0)
  {
      // suspend task
     _mutex.unlock();
      //cout << " thread port " << port << " will be suspended" << endl;
      sem_wait(&_wait_sem); 
      //if (port != 0)
        //cout << " thread port " << port << " will be resumed" << endl;
      return data_available;
  }
  else
  {
    // fecth a new word
    data_available= data_provider->gbtword_available();
    if (data_available)
    {
      curword = data_provider->get_word();
      cout << "@@@@@ " << port << "   " << nb_sample_readers << " "  << cur_sample<< endl;
      cur_sample++;
      nb_sample_readers = nb_links;
    }
    // free all waiting thread
    //cout << " thread port " << port << " will be wake up all threads" << endl;
    for (int i=0; i< nb_links-1;i++) sem_post(&_wait_sem); 
    _mutex.unlock(); // end on critical section
    return data_available;
  }
}

uint8_t GBT_r::read(int port,int sample)
{
  cout << " port " << port << "  " << sample << " curword " <<curword[3]<<curword[2]<<curword[1]<<curword[0] << endl;
  return curword[port*2+(sample & 1)];
}
