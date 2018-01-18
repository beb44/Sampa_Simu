#ifndef SAMPA
#define SAMPA
#include <cstdint>
#include <list>
#include "Elink.h"
/*!
 *  \brief sampa chip emulator
 *
 *  This class provides a emulation artefacts for sampa chip serialisation mechanism.\n
 *  it prodives methods to build dsp generated build frames and methods to emulate serial link.
 */
class sampa  : public elink
{
public:
  explicit sampa(uint16_t addr); 
  ~sampa();
  void select_channel(const uint8_t chid);
  void reset_frame();
  void add_data(int const len, uint16_t const *data);
 
  void disp_frame();
  void send_frame();

  void make_sync();
 
  bool    serial_available(); 
  uint8_t get_serial();
  
private:

  void make_data_header();

  uint16_t  m_frame[1024+5];
  uint16_t  m_wpointer;
  bool      m_hasheader;
  uint8_t   m_haddr;
  uint8_t   m_channel;
  
  std::list<uint16_t *>  send_list;
  
  bool      m_data_available;
  uint16_t  *m_cur_send_frame;
  int       m_cur_word;
  int       m_cur_bit;
  uint16_t  m_cur_len;
  
};
#endif
