#ifndef RECEIVER
#define RECEIVER
#include <thread>
#include "sampa_head.h"
#include "Elink.h"
#include "GBT_r.h"

class receiver_handler
{
public:
  virtual void  rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff) = 0;
};


class receiver
{
public:
  receiver();
  receiver(elink &p);
  receiver(int port,GBT_r &p);
  void set_userhandler(void (*foo)(int,int,int,int,int,short *));
  void set_userhandler(receiver_handler* handler);
  void start();
  void join();
  bool joinable();
  void process();
#ifdef STATS
  void display_stats();
  void reset_stats();
#endif
private:
  void (*user_handler)(int,int,int,int,int,short *);
  receiver_handler     *rec_handl;

  void handle_packet(const uint64_t header,int len,uint16_t *buffer);
  
  uint64_t m_synchead;
  uint64_t m_curhead;
 
  elink     &m_peer;
  bool      m_is_synced;
  int       m_syncpos;  
  int       m_headcd;
  int       m_cur_bit;
  uint16_t  m_cur_len;
  uint16_t  *m_wpointer;
  uint16_t  m_frame[1024];
  
  std::thread  *TheThread;
  int       m_port;
  
  // statistic records
#ifdef STATS
  uint32_t  m_packetcount;
  uint32_t  m_packetcount_by_type[8];
#endif  
};
#endif
