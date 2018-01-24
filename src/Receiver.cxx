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
Receiver::Receiver(Elink &provider) : mPeer(provider),
                                      mPort(0),
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(NULL),
		                      mStartOfPackerHandler(NULL),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(NULL)

{  
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructor
 *
 *  The receiver is configured in normal zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reference
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */
Receiver::Receiver(Elink &provider,void *ui,
                                   StartOfPackerHandler *sop,
                                   PacketHandler *ph,
			           EndOfPackerHandler *eop) 
                                    : mPeer(provider),
                                      mPort(0),
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(sop),
		                      mPacketHandler(ph),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(eop)
{  
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructor
 *
 *  The receiver is configured in Cluster sum zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reférence
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */
Receiver::Receiver(Elink &provider,void *ui,
                                   StartOfPackerHandler *sop,
                                   ClusterSumPacketHandler *ph,
			           EndOfPackerHandler *eop) 
                                    : mPeer(provider),
                                      mPort(0),
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(sop),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(ph),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(eop)
{  
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructor
 *
 *  The receiver is configured in non zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reférence
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */
Receiver::Receiver(Elink &provider, void *ui,
                                   StartOfPackerHandler *sop,
                                   RawPacketHandler *ph,
			           EndOfPackerHandler *eop) 
                                    : mPeer(provider),
                                      mPort(0),
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(sop),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(ph),
		                      mEndOfPackerHandler(eop)
{  
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructeur
 *
 *  \param port : port on which the remote Sampa/DualSampa is connected
 *  \param provider : reference of the 'remote' Gbtr
 */
Receiver::Receiver(int port,GbtR &provider): 
                                      mPeer(provider.GetElink(this,port)), 
				      mPort(port), 
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(NULL),
		                      mStartOfPackerHandler(NULL),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(NULL)
{
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructor
 *
 *  The receiver is configured in normal zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param port      Port on which the remote Sampa/DualSampa is connected
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reférence
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */
Receiver::Receiver(int port,GbtR &provider,void *ui,
                                           StartOfPackerHandler *sop,
                                           PacketHandler        *ph,
			                   EndOfPackerHandler   *eop): 
                                      mPeer(provider.GetElink(this,port)), 
				      mPort(port), 
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(NULL),
		                      mPacketHandler(ph),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(NULL)
{
  user_handler = 0;
  mRecHandl = 0;
}

/*!
 *  \brief Constructor
 *
 *  The receiver is configured in Cluster sum zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param port      Port on which the remote Sampa/DualSampa is connected
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reférence
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */
Receiver::Receiver(int port,GbtR &provider,void *ui,
                                           StartOfPackerHandler     *sop,
                                           ClusterSumPacketHandler *ph,
			                   EndOfPackerHandler       *eop): 
                                      mPeer(provider.GetElink(this,port)), 
				      mPort(port), 
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(NULL),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(ph),
		                      mPacketRawHandler(NULL),
		                      mEndOfPackerHandler(NULL)
{
  user_handler = 0;
  mRecHandl = 0;
}
/*!
 *  \brief Constructor
 *
 *  The receiver is configured in non zero supressed mode packets 
 *  (i.e. the remote Sampa/DualSampa is assumed to be configured
 *  accordingly)
 *
 *  \param port      Port on which the remote Sampa/DualSampa is connected
 *  \param provider  Reference of the 'remote' Elink
 *  \param ui        User provided reférence
 *  \param sop       Start Of Packet user handler
 *  \param ph        Packet user handler
 *  \param eop       Etart Of Packet user handler
 */

Receiver::Receiver(int port,GbtR &provider,void *ui,
                                           StartOfPackerHandler *sop,
                                           RawPacketHandler     *ph,
			                   EndOfPackerHandler   *eop): 
                                      mPeer(provider.GetElink(this,port)), 
				      mPort(port), 
				      mSynchead(SampaHead().BuildSync()),
				      mCurHead(0),
                                      mIsSynced(false),
				      mSyncPos(0),
				      mUi(ui),
		                      mStartOfPackerHandler(NULL),
		                      mPacketHandler(NULL),
		                      mClusterSumPacketHandler(NULL),
		                      mPacketRawHandler(ph),
		                      mEndOfPackerHandler(NULL)
{
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
 *  \param header packet header in a 50 bit width format
 *  \param len payload length (number of 10 bit words )
 *  \param buffer payload 
 */
void Receiver::HandlePacket(const uint64_t header,int len,uint16_t *buffer)
{
SampaHead  head_decoder(header);

  head_decoder.Decode(); 
  //head_decoder.display();
  if (mStartOfPackerHandler) mStartOfPackerHandler(mUi,header);
  if (mPacketHandler)
  {
    // parse packet
    int curpos = 0;
    //int len;
    while (curpos < len)
    {
       len = buffer[curpos]+2;
       mPacketHandler(mUi,
                      head_decoder.mFChipAddress,
  		      head_decoder.mFChannelAddress,
		      buffer[curpos],
		      buffer[curpos+1],
		     (short *)&buffer[curpos+2]);
       curpos += len;
    }
  }
  if (mClusterSumPacketHandler)
  {
    // parse packet
    int curpos = 0;
    while (curpos < len)
    {
       mClusterSumPacketHandler(mUi,
                                head_decoder.mFChipAddress,
  		                head_decoder.mFChannelAddress,
		                buffer[curpos],
		                buffer[curpos+1],
		                buffer[curpos+2]*1024+buffer[curpos+3]); 
       curpos += 4;
    }
  }
  if (mPacketRawHandler)
  {
    mPacketRawHandler(mUi,
                      head_decoder.mFChipAddress,
  		      head_decoder.mFChannelAddress,
		      len,
		     (short *)&buffer[2]);
  }  
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
				
  if (mEndOfPackerHandler) mEndOfPackerHandler(mUi);

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
//int      payloadLength=0;
//int      packetType;
  payloadLength = 0;
  while (mPeer.SerialAvailable()) {
    ProcessData(mPeer.GetSerial());
  }
}
//  if (mPeer == (elink *)0) return;
void Receiver::ProcessData(uint8_t word)
{  
int      packetType;
//  while (mPeer.SerialAvailable()) {
    if (mIsSynced) {
      if (mHeadcd !=0) {
        mCurHead = ((mCurHead >>1) + (((uint64_t)(word))<<49)) & 0x3ffffffffffff;
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
	*mWPointer =  ((*mWPointer) )+((uint64_t)(word)<<mCurBit);
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
      mCurHead = ((mCurHead >>1) + (((uint64_t)(word))<<49)) & 0x3ffffffffffff;
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
//  }  
}

void Receiver::Push(uint8_t data)
{
  ProcessData(data);
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

