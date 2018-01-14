#ifndef RECEIVER
#define RECEIVER
#include "sampa_head.h"
#include "Elink.h"
class receiver
{
public:
  receiver();
  receiver(elink *p);
  void set_userhandler(void (*foo)(int,int,int,int,int,short *));
  void process();
private:
  void (*user_handler)(int,int,int,int,int,short *);


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
  
};
#endif
