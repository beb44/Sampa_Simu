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
class sampa final : public elink
{
public:
  sampa(uint16_t addr);
  ~sampa();
  void select_channel(const uint8_t chid);
  void reset_frame();
  void add_data(int len, uint16_t *data);
 
  void disp_frame();
  void send_frame();

  void make_sync();
 
  bool serial_available(); 
  uint8_t get_serial();
  
private:

  void make_data_header();

  uint16_t  _frame[1024+5];
  uint16_t  _wpointer;
  bool      _hasheader;
  uint8_t   Haddr;
  uint8_t   Channel;
  
  std::list<uint16_t *>  send_list;
  
  bool      _data_available;
  uint16_t  *cur_send_frame;
  int       cur_word;
  int       cur_bit;
  uint16_t  cur_len;
  
};
#endif
