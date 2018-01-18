#include <iostream>
#include <cstdint>
#include <sys/types.h>
#include <sys/sem.h>
#include "gbt_r.h" 
using namespace std;

gbt_elink::gbt_elink(int port,gbt_gen& gbt): m_gbt(gbt), m_port(port)
{
  m_sample = 0;
  sem_init(&m_wait_sem, 0, 0);
}
bool gbt_elink::serial_available()
{
bool res = m_gbt.fetch(m_port,m_sample);
  return res;
}

uint8_t  gbt_elink::get_serial()
{
  return m_gbt.read(m_port,m_sample++);
}


void gbt_elink::lock()
{
  sem_wait(&m_wait_sem);
}

void gbt_elink::unlock()
{
  sem_post(&m_wait_sem);
}


gbt_r::gbt_r(gbtlink &provider) :m_data_provider(provider)
{
  m_elink_map.clear();
  m_started = false;
  m_cur_sample = -1;
}

elink &gbt_r::get_elink(int const port)
{
  if (m_started) throw;
  if (m_elink_map.find(port) != m_elink_map.end()) throw;
  m_elink_map[port] = new gbt_elink(port,*this);
  m_nb_links++;
  m_nb_sample_readers = m_nb_links;
  return *m_elink_map[port];
}

bool gbt_r::fetch(int const port,int const sample) 
{
  m_mutex.lock();
  if ((sample/2) == m_cur_sample) {
    m_mutex.unlock();
    return true;
  }
  
  m_nb_sample_readers--;
  if (m_nb_sample_readers != 0) {
    // suspend task
    m_mutex.unlock();
    m_elink_map[port]->lock() ;
    return m_data_available;
  }
  else {
    //
    // fecth a new word
    //
    m_data_available= m_data_provider.gbtword_available();
    if (m_data_available) {
      m_curword = m_data_provider.get_word();
      m_cur_sample++;
      m_nb_sample_readers = m_nb_links;
    }
    //
    // free all waiting thread
    //
    for (int i=0; i< m_nb_links;i++) { 
      if (i != port) 
        m_elink_map[i]->unlock() ; 
    }
    m_mutex.unlock(); // end on critical section
    return m_data_available;
  }
}

uint8_t gbt_r::read(int const port,int const sample)
{
  return m_curword[port*2+(sample & 1)];
}
