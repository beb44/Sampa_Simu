#include <iostream>
#include <string.h>
#include <cstdint>
#include <bitset>
#include "sampa_head.h"
#include "sampa.h"
using namespace std; 

 /*!
 *  \brief Constructeur
 *
 *  \param addr : hardware address of the sampa chip
 */
sampa::sampa(uint16_t addr)
{
  cout <<"Sampa " << (int)addr << " created " << endl;
  Haddr = addr;
  reset_frame();  // make room for header
  Channel = 0;    // channel 0 is default
  _data_available = false;        // no serial data available;
  cur_send_frame = (uint16_t *)0; // no current frame being sent
}

sampa::~sampa()
{
  cout <<"Sampa " << (int)Haddr << " deleted " << endl; 
}

void sampa::select_channel(const uint8_t chid)
{
  Channel = chid;
}

/*!
 *  \brief starts a new frame
 *
 *  internal buffer is reset and ready to receives new data samples.\n
 *  the frame header will be automatically build when frame will be 'sent out'.\n
 *  If the previous frame has not been sent, it will be lost
 *  
 */
void sampa::reset_frame()
{
  _wpointer = 5;  // make room for header
  _hasheader = false;
}

/*!
 *  \brief Add a data chunk to current frame
 *
 *  \param len  length of the data chunk
 *  \param data adresse of the buffer 
 */
void sampa::add_data(int len, uint16_t *data)
{
  // append data to the crrent frame
  memcpy(_frame+_wpointer,data,sizeof(data)*len);
  _wpointer += len;
}

void sampa::make_sync()
{
sampa_head header;
uint64_t   res;


   res= header.build_sync();
   _frame[0]= (res >> 0  ) & 0x3ff;
   _frame[1]= (res >> 10 ) & 0x3ff;
   _frame[2]= (res >> 20 ) & 0x3ff;
   _frame[3]= (res >> 30 ) & 0x3ff;
   _frame[4]= (res >> 40 ) & 0x3ff;
   _hasheader =  true;
}

void sampa::make_data_header()
{
sampa_head header;
uint64_t   res;

   header.fHammingCode          = 0x3f;
   header.fHeaderParity         = 0;
   header.fPkgType              = 4;
   header.fNbOf10BitWords       = _wpointer-5; 
   header.fChipAddress          = Haddr; 
   header.fChannelAddress       = Channel ,
   header.fBunchCrossingCounter = 0xBAE2;
   header.fPayloadParity        = 0; 
   res= header.build();
   _frame[0]= (res >> 0 ) & 0x3ff;
   _frame[1]= (res >> 10 ) & 0x3ff;
   _frame[2]= (res >> 20 ) & 0x3ff;
   _frame[3]= (res >> 30 ) & 0x3ff;
   _frame[4]= (res >> 40  ) & 0x3ff;
   _hasheader =  true;
}


/*!
 *  \brief make the current frame available for serial link
 *
 *  The current frame is 'cloned' and linked in the ready list of serial link.\n
 *  If the frame is empty and no header is present, the frame will not been sent.\n
 *  If header has not been built yet, it will be constructed and preponed to the frame.\n
 *
 *  The frame will not been 'reset' and can ben reused.
 *  
 */
void sampa::send_frame()
{

uint16_t  *_fp;

  // if no data and no header build, just ignore
  if ((_wpointer == 5) && (_hasheader == false))
    return;
    
  // if the buffer is not yet built, make it
  if (_hasheader == false)
    make_data_header();
    
  // clone the current buffer and link it to the sending list
  _fp= (uint16_t  *) malloc(sizeof(uint16_t)*_wpointer);
  memcpy(_fp,_frame,sizeof(uint16_t)*_wpointer); 
  send_list.push_back(_fp);
}

/*!
 * \brief Display the frame being hold in the internal buffer in binary format
 *
 * Debugging tool for displaying the internal buffer contents. It automatically generates the header if needed.
 */
void sampa::disp_frame()
{

uint16_t  *_fp = _frame;

  // if no data and no header build, just ignore
  if ((_wpointer == 5) && (_hasheader == false))
    return;
  if (_hasheader == false)
    make_data_header();
  cout << "Header "<< endl;
  for (int i=0;i<5;i++)
    cout << std::bitset<10> (*(_fp++) )<< endl;
  if (_wpointer != 5)
  {
    cout << "Payload " << endl;
    for (int i=5;i<_wpointer;i++)
      cout << std::bitset<10> (*(_fp++) ) << endl;   
  }
}
/*!
 * \brief indicates wether or not some data is available for serial link
 *
 * \return True is fome data is available, false otherwise
 *
 */
bool sampa::serial_available()
{
  if (send_list.size() != 0)
    _data_available = true;
  return _data_available;
}

uint8_t sampa::get_serial()
{
uint8_t  res;
  if (cur_send_frame == (uint16_t *)0)
  {
    // no frame being sent for now, check for waiting ones
    if (send_list.size() == 0) 
      return 0xff; 
    // set internal variable
    cur_send_frame = send_list.front();
    send_list.pop_front();
    cur_word = 0;
    cur_bit  = 0;
    cur_len  = cur_send_frame[1]+5; // get size from the header
  }
  // get the data
  res =  ((cur_send_frame[cur_word] >> cur_bit) & 1);
  // increment pointer
  cur_bit++;
  if (cur_bit==10)
  {
    // all bits in current word have been sent out
    cur_word ++;
    cur_bit = 0;
    if (cur_word == cur_len)
    {
      // all words have been sent out
      _data_available = false;
      // free memory 
      cur_word = 0;
      cur_len  = 0;
      cur_bit  = 0; 
      free(cur_send_frame);
      cur_send_frame = (uint16_t *)0;
    } 
  }
  return res;
}


