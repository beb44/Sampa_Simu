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
  receiver(elink *p);
  receiver(int port,GBT_r *p);
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
  uint64_t synchead;
  uint64_t curhead;
 
  elink    *peer;
  bool     issync;
  int      syncpos;  
  int      headcd;
  int      payload_length;
  int       cur_bit;
  uint16_t  cur_len;
  uint16_t  *_wpointer;
  uint16_t  _frame[1024];
  
  std::thread  *TheThread;
  
  // statistic records
#ifdef STATS
  uint32_t  packetcount;
  uint32_t  packetcount_by_type[8];
#endif  
};
#endif
