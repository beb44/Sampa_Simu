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
sampa::sampa(uint16_t addr) : m_haddr(addr)
{
  reset_frame();  // make room for header
  m_channel = 0;    // channel 0 is default
  m_data_available = false;        // no serial data available;
  m_cur_send_frame = (uint16_t *)0; // no current frame being sent
}

sampa::~sampa()
{
  cout <<"Sampa " << (int)m_haddr << " deleted " << endl; 
}

void sampa::select_channel(const uint8_t chid)
{
  m_channel = chid;
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
  m_wpointer = 5;  // make room for header
  m_hasheader = false;
}

/*!
 *  \brief Add a data chunk to current frame
 *
 *  \param len  length of the data chunk
 *  \param data adresse of the buffer 
 */
void sampa::add_data(int const len, uint16_t const *data)
{
  // append data to the crrent frame
  memcpy(m_frame+m_wpointer,data,sizeof(data)*len);
  m_wpointer += len;
}

void sampa::make_sync()
{
sampa_head header;
uint64_t   res;


   res= header.build_sync();
   m_frame[0]= (res >> 0  ) & 0x3ff;
   m_frame[1]= (res >> 10 ) & 0x3ff;
   m_frame[2]= (res >> 20 ) & 0x3ff;
   m_frame[3]= (res >> 30 ) & 0x3ff;
   m_frame[4]= (res >> 40 ) & 0x3ff;
   m_hasheader =  true;
}

void sampa::make_data_header()
{
sampa_head header;
uint64_t   res;

   header.fHammingCode          = 0x3f;
   header.fHeaderParity         = 0;
   header.fPkgType              = 4;
   header.fNbOf10BitWords       = m_wpointer-5; 
   header.fChipAddress          = m_haddr; 
   header.fChannelAddress       = m_channel ,
   header.fBunchCrossingCounter = 0xBAE2;
   header.fPayloadParity        = 0; 
   res= header.build();
   m_frame[0]= (res >> 0 ) & 0x3ff;
   m_frame[1]= (res >> 10 ) & 0x3ff;
   m_frame[2]= (res >> 20 ) & 0x3ff;
   m_frame[3]= (res >> 30 ) & 0x3ff;
   m_frame[4]= (res >> 40  ) & 0x3ff;
   m_hasheader =  true;
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
  if ((m_wpointer == 5) && (m_hasheader == false))
    return;
    
  // if the buffer is not yet built, make it
  if (m_hasheader == false)
    make_data_header();
    
  // 
  // clone the current buffer and link it to the sending list
  //
  _fp= (uint16_t  *) malloc(sizeof(uint16_t)*m_wpointer);
  memcpy(_fp,m_frame,sizeof(uint16_t)*m_wpointer); 
  send_list.push_back(_fp);
} 

/*!
 * \brief Display the frame being hold in the internal buffer in binary format
 *
 * Debugging tool for displaying the internal buffer contents. It automatically generates the header if needed.
 */
void sampa::disp_frame()
{

uint16_t  *_fp = m_frame;

  // if no data and no header build, just ignore
  if ((m_wpointer == 5) && (m_hasheader == false))
    return;
  if (m_hasheader == false)
    make_data_header();
  cout << "Header "<< endl;
  for (int i=0;i<5;i++)
    cout << std::bitset<10> (*(_fp++) )<< endl;
  if (m_wpointer != 5) {
    cout << "Payload " << endl;
    for (int i=5;i<m_wpointer;i++)
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
    m_data_available = true;
  return m_data_available;
}

uint8_t sampa::get_serial()
{
uint8_t  res;

  if (m_cur_send_frame == (uint16_t *)0) {
    // no frame being sent for now, check for waiting ones
    if (send_list.size() == 0) 
      return 0xff; 
    // set internal variable
    m_cur_send_frame = send_list.front();
    send_list.pop_front();
    m_cur_word = 0;
    m_cur_bit  = 0;
    m_cur_len  = m_cur_send_frame[1]+5; // get size from the header
  }
  // get the data
  res =  ((m_cur_send_frame[m_cur_word] >> m_cur_bit) & 1);
  // increment pointer
  m_cur_bit++;
  if (m_cur_bit==10) {
    // all bits in current word have been sent out
    m_cur_word ++;
    m_cur_bit = 0;
    if (m_cur_word == m_cur_len) {
      // all words have been sent out
      m_data_available = false;
      // free memory 
      m_cur_word = 0;
      m_cur_len  = 0;
      m_cur_bit  = 0; 
      free(m_cur_send_frame);
      m_cur_send_frame = (uint16_t *)0;
    } 
  }
  return res;
}


