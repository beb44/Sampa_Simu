#ifndef DUALSAMPA
#define DUALSAMPA
#include <cstdint>
#include "sampa.h"
#include "Elink.h"
/*!
 *  \brief dualsampa chip emulator
 *
 *  This class provides a emulation artefacts for dualsampa chip serialisation mechanism.\n
 *  
 */
class dualsampa final : public elink
{
public:
  dualsampa(uint8_t addr1,uint8_t addr2);
  ~dualsampa();
  void set_internal_ref(int ref);
  void set_user_handler(void (*uh)(int));
  void select_channel(const uint8_t sId,const uint8_t chid);
  void reset_frames();
  void reset_frame(const uint8_t sId);
  void add_data(const uint8_t sId,int len, uint16_t *data);
  void send_frame(const uint8_t sId);
  void send_frames();
  void regenerate_data();

  void make_sync(const uint8_t sId);
  
  bool serial_available(); 
  uint8_t get_serial();

private:
   dualsampa();       // makes constructor unavailable
   sampa  *sampas[2];
   
   uint64_t   syncpacket;
   int        sync_countdown;
   int        send_credit;
   
   int	      internal_ref;
   void       (*user_handler)(int);
   bool       data_regenerated;
};
#endif
