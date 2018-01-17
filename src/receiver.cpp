#include <iostream>
#include <bitset>
#include <thread>
#include "receiver.h"

using namespace std;
 
receiver::receiver()
{  
  peer = (elink *)0;
  issync = false;
  syncpos = 0;
  synchead = sampa_head().build_sync();
  curhead = 0;
  user_handler = 0;
  rec_handl = 0;
#ifdef STATS
  reset_stats();
#endif
}

receiver::receiver(elink *p)
{  
  peer = p;
  issync = false;
  synchead = sampa_head().build_sync();
  syncpos = 0;
  curhead = 0;
  user_handler = 0;
  rec_handl = 0;
#ifdef STATS
  reset_stats();
#endif
}

receiver::receiver(int port,GBT_r *p)
{
  peer = p->get_elink(port);
  synchead = sampa_head().build_sync();
  syncpos = 0;
  curhead = 0;
  user_handler = 0;
  rec_handl = 0;
#ifdef STATS
  reset_stats();
#endif
}

void receiver::start()
{
  try {
  TheThread = new std::thread(&receiver::process,this);
  }
  catch (const std::exception& e)
  {
    cout << "execption catched" << endl;
    throw;
  }
}

void receiver::join()
{
  TheThread->join();
}
bool receiver::joinable()
{
  return TheThread->joinable();
}
void receiver::set_userhandler(void (*foo)(int,int,int,int,int,short *))
{
  user_handler=foo;
}

void receiver::set_userhandler(receiver_handler *handler)
{
  rec_handl=handler;
}

void receiver::handle_packet(const uint64_t header,int len,uint16_t *buffer)
{
sampa_head  head_decoder(header);

  head_decoder.decode();
  //head_decoder.display();
  if (user_handler) user_handler(head_decoder.fChipAddress,
  			         head_decoder.fChannelAddress,
				 0,
				 buffer[0],
				 buffer[1],
				 (short *)&buffer[2]);
				 
  if (rec_handl) rec_handl->rec_handler(head_decoder.fChipAddress,
  			         head_decoder.fChannelAddress,
				 0,
				 buffer[0],
				 buffer[1],
				 (short *)&buffer[2]);
				
}
void receiver::process()
{  
  if (peer == (elink *)0) return;
  while (peer->serial_available()) 
  {
    if (issync)
    {
      if (headcd !=0)
      {
        curhead = ((curhead >>1) + (((uint64_t)(peer->get_serial()))<<49)) & 0x3ffffffffffff;
        headcd --;
	if (headcd ==0)
	{ 
	  // header fully loaded, get playload length
	  payload_length = sampa_head().get_nbwords(curhead); 
#ifdef STATS
          packetcount_by_type[sampa_head().get_packet_type(curhead)]++;
	  packetcount++;
#endif	
          cout << syncpos << " "<< std::bitset<50>( curhead ) << "payload length " << payload_length << endl;
	  if (payload_length == 0)
	  {
	    // zero length packet, catch next header
	    headcd = 50; // 50 bit of header to be read
	  }
	  else
	  {
	    // non empty packet, process
	    _wpointer = &_frame[0];
            *_wpointer = 0;
	    cur_len = 0; cur_bit = 0; 
	  }
	}
      }
      else
      {
        //cout << "normal data reception "<< endl;	
        // data reception
	*_wpointer =  ((*_wpointer) )+((uint64_t)(peer->get_serial())<<cur_bit);
	//cout << "partial word received " << std::bitset<10> (*_wpointer)  << endl;
	//peer->get_serial();
	cur_bit++;	
#if 1
        if (cur_bit==10)
        {
	   //cout << "complete word received " << std::bitset<10> (*_wpointer)  << endl;
           // all bits in current word have been sent out
            _wpointer++;
            *_wpointer = 0;
           cur_bit = 0;
	   cur_len++;
	   if (cur_len == payload_length) 
	   {
	     headcd = 50; // 50 bit of header to be read
	     cout << "complete data packet received " << cur_len << endl;
             for (int i=0;i<cur_len;i++) cout << _frame[i] << " " ; 
             cout << endl;
	     handle_packet(curhead,payload_length,_frame);
	   }
         }
#endif	
      }  
    }
    else
    {
      syncpos++;
      curhead = ((curhead >>1) + (((uint64_t)(peer->get_serial()))<<49)) & 0x3ffffffffffff;
      //cout << syncpos << " "<< std::bitset<50>( curhead )<< " " <<
      //std::bitset<50> (synchead) << endl;
      if (curhead == synchead)
      {
        // cout << syncpos << " "<< std::bitset<50>( curhead )<< "-"   << std::hex <<
	//curhead << "payload length " << payload_length << endl;
        issync= true;
	// cout <<"receiver: Synchronisation trouvee au bit" << (syncpos-50) << endl;   
	headcd = 50; // 50 bit of header to be read
#ifdef STATS
        packetcount_by_type[sampa_head().get_packet_type(curhead)]++;
	packetcount++;
#endif	
      }	
    }
  }  
}

#ifdef STATS
// statistic handling

void receiver::reset_stats()
{
  packetcount = 0;
  for (int i=0;i<8;i++) packetcount_by_type[i]=0;
}
void receiver::display_stats()
{
  cout << "Received packets statistics" << endl;
  cout << "Total received packets : " << packetcount<< endl;
  for (int i=0;i<8;i++)
  {
    cout <<  "Received packets type "<<i<< " : " << packetcount_by_type[i]<< endl; packetcount_by_type[i]=0;
  }
}
#endif
