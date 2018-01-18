#include <iostream>
#include <bitset>
#include <thread>
#include "Receiver.h" 

using namespace std;
#if 0 
Receiver::Receiver()
{  
  mPeer = (elink *)0;
  mIsSynced = false;
  mSyncPos = 0;
  mSyncHead = sampa_head().build_sync();
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
#ifdef STATS
  reset_stats();
#endif
}
#endif
Receiver::Receiver(Elink &p) : mPeer(p)
{  
  mIsSynced = false;
  mSynchead = sampa_head().build_sync();
  mSyncPos = 0;
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
#ifdef STATS
  reset_stats();
#endif
}

Receiver::Receiver(int port,gbt_r &p): mPeer(p.get_elink(port)), mPort(port) 
{
  mSynchead = sampa_head().build_sync();
  mSyncPos = 0;
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
#ifdef STATS
  reset_stats();
#endif
}

void Receiver::Start()
{
  try {
  TheThread = new std::thread(&Receiver::Process,this);
  }
  catch (const std::exception& e) {
    cout << "execption catched" << endl;
    throw;
  }
}

void Receiver::Join()
{
  TheThread->join();
}
bool Receiver::Joinable()
{
  return TheThread->joinable();
}
void Receiver::SetUserHandler(void (*foo)(int,int,int,int,int,short *))
{
  user_handler=foo;
}

void Receiver::SetUserHandler(ReceiverHandler *handler)
{
  mRecHandl=handler;
}

void Receiver::HandlePacket(const uint64_t header,int len,uint16_t *buffer)
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
				 
  if (mRecHandl) mRecHandl->RecHandler(head_decoder.fChipAddress,
  			         head_decoder.fChannelAddress,
				 0,
				 buffer[0],
				 buffer[1],
				 (short *)&buffer[2]);
				
}
void Receiver::Process()
{  
int      payload_length;

//  if (mPeer == (elink *)0) return;
  while (mPeer.SerialAvailable()) {
    if (mIsSynced) {
      if (mHeadcd !=0) {
        mCurHead = ((mCurHead >>1) + (((uint64_t)(mPeer.GetSerial()))<<49)) & 0x3ffffffffffff;
        mHeadcd --;
	if (mHeadcd ==0) { 
	  // header fully loaded, get playload length
	  payload_length = sampa_head().get_nbwords(mCurHead); 
#ifdef STATS
          packetcount_by_type[sampa_head().get_packet_type(m_curhead)]++;
	  packetcount++;
#endif	
	  if (payload_length == 0) {
	    // zero length packet, catch next header
	    mHeadcd = 50; // 50 bit of header to be read
	  }
	  else {
	    // non empty packet, process
	    mWPointer = &mFrame[0];
            *mWPointer = 0;
	    mCurLen = 0; mCurBit = 0; 
	  }
	}
      }
      else {
        // data reception
	*mWPointer =  ((*mWPointer) )+((uint64_t)(mPeer.GetSerial())<<mCurBit);
	mCurBit++;	
        if (mCurBit==10) {
          // all bits in current word have been sent out
          mWPointer++;
          *mWPointer = 0;
          mCurBit = 0;
	  mCurLen++;
	  if (mCurLen == payload_length)  {
	     mHeadcd = 50; // 50 bit of header to be read
	     HandlePacket(mCurHead,payload_length,mFrame);
	   }
         }
       }  
    }
    else {
      mSyncPos++;
      mCurHead = ((mCurHead >>1) + (((uint64_t)(mPeer.GetSerial()))<<49)) & 0x3ffffffffffff;
      if (mCurHead == mSynchead) {
	//m_curhead << "payload length " << payload_length << endl;
        mIsSynced= true;
	mHeadcd = 50; // 50 bit of header to be read
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

void Receiver::reset_stats()
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
