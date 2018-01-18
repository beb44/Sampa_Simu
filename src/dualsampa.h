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
class dualsampa_handler
{
public:
  virtual void dsp_handler(int ref) = 0;
};

class dualsampa  : public elink
{
public:
  dualsampa(){};       // makes constructor unavailable
  explicit dualsampa(uint16_t addr1,uint16_t addr2);
  ~dualsampa();
  void set_internal_ref(int ref);
  void set_data_provider(void (*uh)(int));
  void set_data_provider(dualsampa_handler *handler);
  void select_channel(const uint8_t sId,const uint8_t chid);
  void reset_frames();
  void reset_frame(const uint8_t sId);
  void add_data(const uint8_t sId,int len, uint16_t *data);
  void send_frame(const uint8_t sId);
  void send_frames();
  void regenerate_data();

  void make_sync(const uint8_t sId);
  
  bool    serial_available() ; 
  uint8_t get_serial() ;

private:
   
   sampa             *m_sampas[2];
   
   uint64_t          m_syncpacket;
   int               m_sync_countdown;
   int               m_send_credit;
   
   int	             m_internal_ref;
   void              (*m_c_data_provider)(int);
   dualsampa_handler *handler_dsp;
   bool              m_data_regenerated;
};
#endif
