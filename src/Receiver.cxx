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
  mSyncHead = SampaHead().BuildSync();
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
#ifdef STATS
  reset_stats();
#endif
}
#endif
/*!
 *  \brief Constructor
 *
 *  \param provider : reference of the 'remote' Elink
 */

Receiver::Receiver(Elink &provider) : mPeer(provider)
{  
  mIsSynced = false;
  mSynchead = SampaHead().BuildSync();
  mSyncPos = 0;
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
}
/*!
 *  \brief Constructeur
 *
 *  \param port : port on which the remote Sampa/DualSampa is connected
 *  \param provider : reference of the 'remote' Gbtr
 */

Receiver::Receiver(int port,GbtR &provider): mPeer(provider.GetElink(port)), mPort(port) 
{
  mSynchead = SampaHead().BuildSync();
  mSyncPos = 0;
  mCurHead = 0;
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Start the receiver processing loop in a thread
 *
 */

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

/*!
 *  \brief Waits for the receiver processing loop to terminate
 *
 */
void Receiver::Join()
{
  TheThread->join();
}

/*!
 *  \brief Check the thread still exist
 *
 */
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

/*!
 *  \brief Handle and dispatch incomming packets
 *
 *  \params header packet header in a 50 bit width format
 *  \params len payload length (number of 10 bit words )
 *  \params buffer payload 
 */
void Receiver::HandlePacket(const uint64_t header,int len,uint16_t *buffer)
{
SampaHead  head_decoder(header);

  head_decoder.Decode(); 
  //head_decoder.display();
  if (user_handler) user_handler(head_decoder.mFChipAddress,
  			         head_decoder.mFChannelAddress,
				 buffer[0],
				 buffer[1],
				 len,
				 (short *)&buffer[2]);
				 
  if (mRecHandl) mRecHandl->RecHandler(head_decoder.mFChipAddress,
  			         head_decoder.mFChannelAddress,
				 buffer[0],
				 buffer[1],
				 len,
				 (short *)&buffer[2]);
				
}
/*!
 *  \brief main processing loop
 *
 * reads the remote party serial link, search for synchronisation and
 * parse incoming packets.\n
 * the processing ends when no more data is available on the serial link.
 *
 */

void Receiver::Process()
{  
int      payloadLength;
int      packetType;

//  if (mPeer == (elink *)0) return;
  while (mPeer.SerialAvailable()) {
    if (mIsSynced) {
      if (mHeadcd !=0) {
        mCurHead = ((mCurHead >>1) + (((uint64_t)(mPeer.GetSerial()))<<49)) & 0x3ffffffffffff;
        mHeadcd --;
	if (mHeadcd ==0) { 
	  // header fully loaded, get playload length
	  payloadLength = SampaHead().GetNbWords(mCurHead);
	  packetType    = SampaHead().GetPacketType(mCurHead);
          mStats.mPacketCountByType[packetType]++;
          mStats.mWordsCountByPacketType[packetType] += payloadLength+5;
	  mStats.mPacketCount++;
	  if (payloadLength == 0) {
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
	  if (mCurLen == payloadLength)  {
	     mHeadcd = 50; // 50 bit of header to be read
	     HandlePacket(mCurHead,payloadLength,mFrame);
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
	payloadLength = SampaHead().GetNbWords(mCurHead);
	packetType    = SampaHead().GetPacketType(mCurHead);
        mStats.mPacketCountByType[packetType]++;
        mStats.mWordsCountByPacketType[packetType] += payloadLength+5;
	mStats.mPacketCount++;
      }	
    }
  }  
}

// statistic handling
/*!
 *  \brief Constructor
 *
 */ 
Receiver::Stats::Stats() :mPacketCount(0)
    {
      for (int i=0;i<8;i++) mPacketCountByType[i]= 0;
      for (int i=0;i<8;i++) mWordsCountByPacketType[i]= 0;
    }

/*!
 *  \brief Reset stats counters
 *
 */ 

void Receiver::Stats::ResetStats()
{
  mPacketCount = 0;
  for (int i=0;i<8;i++) mPacketCountByType[i]= 0;
  for (int i=0;i<8;i++) mWordsCountByPacketType[i]= 0;
}

/*!
 *  \brief Display stats counters
 *
 */ 

void Receiver::Stats::DisplayStats()
{
  cout << "Received packets statistics" << endl;
  cout << "Total received packets : " << mPacketCount<< endl;
  for (int i=0;i<8;i++) {
    cout <<  "Received packets type "<<i<< " : " << mPacketCountByType[i]<< endl;
  }
}

