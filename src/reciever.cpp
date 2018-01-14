#include <iostream>
#include <bitset>
#include "reciever.h"

using namespace std;
 
reciever::reciever()
{  
  peer = (elink *)0;
  issync = false;
  syncpos = 0;
  synchead = sampa_head().build_sync();
  curhead = 0;
  user_handler = 0;
}

reciever::reciever(elink *p)
{  
  peer = p;
  issync = false;
  synchead = sampa_head().build_sync();
  syncpos = 0;
  curhead = 0;
}
void reciever::set_userhandler(void (*foo)(int,int,int,int,int,short *))
{
  user_handler=foo;
}
void reciever::handle_packet(const uint64_t header,int len,uint16_t *buffer)
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
				 
                             
}

void reciever::process()
{  
  if (peer == (elink *)0) return;
  while (peer->serial_available()) 
  {
    if (issync)
    {
      if (headcd !=0)
      {
        curhead = ((curhead <<1) + peer->get_serial()) & 0x3ffffffffffff;
        headcd --;
	if (headcd ==0)
	{ 
	  // header fully loaded, get playload length
	  payload_length = (curhead >> 30) & 0xff; 
          //cout << syncpos << " "<< std::bitset<50>( curhead ) << "payload length " << payload_length << endl;
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
	    cur_len = 0; cur_bit = 9; 
	  }
	}
      }
      else
      {
        //cout << "normal data reception "<< endl;	
        // data reception
	*_wpointer =  ((*_wpointer) << 1)+peer->get_serial();
	//peer->get_serial();
	cur_bit--;	
#if 1
        if (cur_bit<0)
        {
	   //cout << "complete word received " << endl;
           // all bits in current word have been sent out
            _wpointer++;
            *_wpointer = 0;
           cur_bit = 9;
	   cur_len++;
	   if (cur_len == payload_length) 
	   {
	     headcd = 50; // 50 bit of header to be read
	     //cout << "complete data packet received" << endl;
             //for (int i=0;i<cur_len;i++) cout << _frame[i] << " " ; 
             //cout << endl;
	     handle_packet(curhead,payload_length,_frame);
	   }
         }
#endif	
      }  
    }
    else
    {
      syncpos++;
      curhead = ((curhead <<1) + peer->get_serial()) & 0x3ffffffffffff;
 //     cout << syncpos << " "<< std::bitset<50>( curhead )<< " " <<
 //     std::bitset<50> (synchead) << endl;
      if (curhead == synchead)
      {
        cout << syncpos << " "<< std::bitset<50>( curhead ) << "payload length " << payload_length << endl;
        issync= true;
	cout <<"Reciever: Synchronisation trouvee au bit" << (syncpos-50) << endl;   
	headcd = 50; // 50 bit of header to be read
      }	
    }
  }  
}