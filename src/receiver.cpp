#include <iostream>
#include <bitset>
#include <thread>
#include "receiver.h" 

using namespace std;
#if 0 
receiver::receiver()
{  
  m_peer = (elink *)0;
  m_is_synced = false;
  m_syncpos = 0;
  m_synchead = sampa_head().build_sync();
  m_curhead = 0;
  user_handler = 0;
  rec_handl = 0;
#ifdef STATS
  reset_stats();
#endif
}
#endif
receiver::receiver(elink &p) : m_peer(p)
{  
  m_is_synced = false;
  m_synchead = sampa_head().build_sync();
  m_syncpos = 0;
  m_curhead = 0;
  user_handler = 0;
  rec_handl = 0;
#ifdef STATS
  reset_stats();
#endif
}

receiver::receiver(int port,GBT_r &p): m_peer(p.get_elink(port)), m_port(port) 
{
  m_synchead = sampa_head().build_sync();
  m_syncpos = 0;
  m_curhead = 0;
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
  catch (const std::exception& e) {
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
int      payload_length;

//  if (m_peer == (elink *)0) return;
  while (m_peer.serial_available()) {
    if (m_is_synced) {
      if (m_headcd !=0) {
        m_curhead = ((m_curhead >>1) + (((uint64_t)(m_peer.get_serial()))<<49)) & 0x3ffffffffffff;
        m_headcd --;
	if (m_headcd ==0) { 
	  // header fully loaded, get playload length
	  payload_length = sampa_head().get_nbwords(m_curhead); 
#ifdef STATS
          packetcount_by_type[sampa_head().get_packet_type(m_curhead)]++;
	  packetcount++;
#endif	
          cout << m_syncpos << " "<< std::bitset<50>( m_curhead ) << "payload length " << payload_length << endl;
	  if (payload_length == 0) {
	    // zero length packet, catch next header
	    m_headcd = 50; // 50 bit of header to be read
	  }
	  else {
	    // non empty packet, process
	    m_wpointer = &m_frame[0];
            *m_wpointer = 0;
	    m_cur_len = 0; m_cur_bit = 0; 
	  }
	}
      }
      else {
        //cout << "normal data reception "<< endl;	
        // data reception
	*m_wpointer =  ((*m_wpointer) )+((uint64_t)(m_peer.get_serial())<<m_cur_bit);
	//cout << "partial word received " << std::bitset<10> (*m_wpointer)  << endl;
	//m_peer->get_serial();
	m_cur_bit++;	
        if (m_cur_bit==10) {
	  //cout << "complete word received " << std::bitset<10> (*m_wpointer)  << endl;
          // all bits in current word have been sent out
          m_wpointer++;
          *m_wpointer = 0;
          m_cur_bit = 0;
	  m_cur_len++;
	  if (m_cur_len == payload_length)  {
	     m_headcd = 50; // 50 bit of header to be read
	     cout << "complete data packet received " << m_cur_len << endl;
             for (int i=0;i<m_cur_len;i++) cout << m_frame[i] << " " ; 
             cout << endl;
	     handle_packet(m_curhead,payload_length,m_frame);
	   }
         }
       }  
    }
    else {
      m_syncpos++;
      m_curhead = ((m_curhead >>1) + (((uint64_t)(m_peer.get_serial()))<<49)) & 0x3ffffffffffff;
      if (m_curhead == m_synchead) {
        // cout << m_syncpos << " "<< std::bitset<50>( m_curhead )<< "-"   << std::hex <<
	//m_curhead << "payload length " << payload_length << endl;
        m_is_synced= true;
	cout <<"receiver: Synchronisation trouvee au bit" << (m_syncpos-50) << endl;   
	m_headcd = 50; // 50 bit of header to be read
#ifdef STATS
        packetcount_by_type[sampa_head().get_packet_type(m_curhead)]++;
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
  for (int i=0;i<8;i++) {
    cout <<  "Received packets type "<<i<< " : " << packetcount_by_type[i]<< endl; packetcount_by_type[i]=0;
  }
}
#endif
