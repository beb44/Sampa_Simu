#include "GBT_s.h"

GBT_s::GBT_s() 
{

  send_list.clear();
  // reset elink map
  for (int i=0;i<maxsocket;i++) elinkmap[i] = 0;
}
 
void GBT_s::plug_elink(const uint8_t socket,elink *peer)
{
  if (!(socket<maxsocket)) return;
  elinkmap[socket] = peer;
}

void GBT_s::process()
{
int     active_responder = 40;
uint8_t bit1;
uint8_t bit2;
  while (active_responder != 0)
  {
    // reset 
    active_responder = 0;
    for (int i=0;i<maxsocket;i++)
    {
      if (elinkmap[i] != 0)
      {
        // a sampa is pluged on this socket
        // read two bits if available
	if (!(elinkmap[i]->serial_available())) continue;
	bit1 = elinkmap[i]->get_serial();
	if (!(elinkmap[i]->serial_available())) continue;
	bit2 = elinkmap[i]->get_serial();
	cur_word[i*2]   = bit2 & 1;
	cur_word[i*2+1] = bit1 & 1;
	active_responder++;
      }
      else
      {
	cur_word[i*2]   = 0;
	cur_word[i*2+1] = 0;
      }
    }
    if (active_responder !=0)
    {
      // at least one active responder, queue the current
      // word for sanding
      send_list.push_back(cur_word);
    }
  } // end while (active....
}

bool GBT_s::gbtword_available()
{
bool     active_responder = false;
uint8_t bit1;
uint8_t bit2;
  for (int i=0;i<maxsocket;i++)
  {
    if (elinkmap[i] != 0)
    {
      // a sampa is pluged on this socket
      // read two bits if available
      if (!(elinkmap[i]->serial_available())) continue;
      bit1 = elinkmap[i]->get_serial();
      if (!(elinkmap[i]->serial_available())) continue;
      bit2 = elinkmap[i]->get_serial();
      cur_word[i*2]   = bit2 & 1;
      cur_word[i*2+1] = bit1 & 1;
      active_responder= true;
    }
    else
    {
      cur_word[i*2]   = 0;
      cur_word[i*2+1] = 0;
    }
  }
  if (active_responder) send_list.push_back(cur_word);
  return (active_responder !=0);
}

std::bitset<128> GBT_s::get_word()
{
std::bitset<128>   word;

  if (send_list.size() ==0) return 0;
  word = send_list.front() ;
  send_list.pop_front();
  return word;
}
