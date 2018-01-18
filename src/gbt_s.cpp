#include <iostream>
#include "gbt_s.h"
using namespace std;
gbt_s::gbt_s() 
{

  m_send_list.clear();
  // reset elink map
  for (int i=0;i<maxsocket;i++) m_elinkmap[i] = 0;
  m_nb_rec =0;
}
 
void gbt_s::plug_elink(const uint8_t socket,elink *peer)
{
  if (!(socket<maxsocket)) return;
  m_elinkmap[socket] = peer;
  m_nb_rec +=1;
}

void gbt_s::process()
{
int     active_responder;
uint8_t bit1;
uint8_t bit2;

  while (active_responder != 0) {
    // reset nb responder
    active_responder = 0;
    for (int i=0;i<maxsocket;i++) {
      if (m_elinkmap[i] != 0) {
	if ((m_elinkmap[i]->serial_available()) == false) { 
	  continue;
	}
	bit1 = m_elinkmap[i]->get_serial();
	if ((m_elinkmap[i]->serial_available()) == false) { 
	  continue;
	}
	bit2 = m_elinkmap[i]->get_serial();
	m_cur_word[i*2]   = bit1 & 1;
	m_cur_word[i*2+1] = bit2 & 1;
	active_responder++;
      }
      else {
	m_cur_word[i*2]   = 0;
	m_cur_word[i*2+1] = 0;
      }
    }
    if (active_responder == m_nb_rec) {
      //
      // all link have returned data, push word for sending
      //
      m_send_list.push_back(m_cur_word);
    }
  } // end while (active....
}

bool gbt_s::gbtword_available()
{
int     active_responder = 0;
uint8_t bit1;
uint8_t bit2;

  for (int i=0;i<maxsocket;i++) {
    if (m_elinkmap[i] != 0) {
      //
      // a sampa is pluged on this socket
      // read two bits if available
      if (!(m_elinkmap[i]->serial_available())) continue;
      bit1 = m_elinkmap[i]->get_serial();
      if (!(m_elinkmap[i]->serial_available())) continue;
      bit2 = m_elinkmap[i]->get_serial();
      m_cur_word[i*2]   = bit1 & 1;
      m_cur_word[i*2+1] = bit2 & 1;
      active_responder++;
    }
    else {
      m_cur_word[i*2]   = 0;
      m_cur_word[i*2+1] = 0;
    }
  }
  if (active_responder == m_nb_rec) {
    //
    // all link have returned data, push word for sending
    //
    m_send_list.push_back(m_cur_word);
  }
  return (active_responder == m_nb_rec);
}

std::bitset<128> gbt_s::get_word()
{
std::bitset<128>   word;

  if (m_send_list.size() ==0) return -1;
  word = m_send_list.front() ;
  m_send_list.pop_front();
  return word;
}
