#ifndef RECEIVER
#define RECEIVER
#include <cstdint>
#include <thread>
#include "SampaHead.h"
#include "Elink.h"
#include "GbtR.h"

typedef void (ClusterSumPacketHandler)(void *ui,int addr,int ch,int nbsamp,int ts,int val);
typedef void (PacketHandler)(void *ui,int addr,int ch,int nbsamp,int ts,short *buff);
typedef void (RawPacketHandler)(void *ui,int addr,int ch,int len,short *buff);
typedef void (StartOfPacketHandler)(void *ui,uint64_t header);
typedef void (EndOfPacketHandler)(void *ui);
class ReceiverHandler 
{
public:
  virtual void  RecHandler(int addr,int ch,int nbsamp,int ts,int len, short *buff) = 0;
};

/*!
 * \brief Received packet parser
 */

class Receiver : public RecInterface
{

  /*!
   * \brief statistic recording class
   */

  class Stats
  {
  public:
    Stats();
    void ResetStats();
    void DisplayStats();
    
    /*! \brief total number of received packets                      */
    uint32_t  mPacketCount;
    /*! \brief total number of received packets per type             */
    uint32_t  mPacketCountByType[8];
    /*! \brief total number of received word per packet type         */
    uint32_t  mWordsCountByPacketType[8];
  };
  
public:
  Receiver();
  Receiver(Elink &provider);
  Receiver(Elink &provider,void *ui,
                           StartOfPacketHandler *sop,
                           PacketHandler *ph,
			   EndOfPacketHandler *eop);
  Receiver(Elink &provider,void *ui,
                           StartOfPacketHandler *sop,
                           ClusterSumPacketHandler *ph,
			   EndOfPacketHandler *eop);
  Receiver(Elink &provider,void *ui,
                           StartOfPacketHandler *sop,
                           RawPacketHandler *ph,
			   EndOfPacketHandler *eop);
  Receiver(int port,GbtR &provider);
  Receiver(int port,GbtR &provider,void *ui,
                                   StartOfPacketHandler *sop,
                                   PacketHandler *ph,
			           EndOfPacketHandler *eop);
  Receiver(int port,GbtR &provider,void *ui,
                                   StartOfPacketHandler *sop,
                                   ClusterSumPacketHandler *ph,
			           EndOfPacketHandler *eop);
  Receiver(int port,GbtR &provider,void *ui,
                                   StartOfPacketHandler *sop,
                                   RawPacketHandler *ph,
			           EndOfPacketHandler *eop);
  void SetUserHandler(void (*foo)(int,int,int,int,int,short *));
  void SetUserHandler(ReceiverHandler* handler);

  void Start();
  void Join();
  bool Joinable();

  void Push(uint8_t data);
  void Process();
  void ProcessData(uint8_t word);
  void DisplayStats();
  void ResetStats();
private:
  /*! \brief remote party                                            */
  Elink     &mPeer;
  /*! \brief total number of received packets                      */
  /*! \brief remote port number on GbtR                              */
  int       mPort;

  /*! \brief pre generated sync packet header in 50 bit width format */
  uint64_t mSynchead;
  /*! \brief current header under construction or decoding           */
  uint64_t mCurHead;


  /*! \brief Internal packet handling routine                        */
  /*! \brief Cluster sum packet user handler (zero supressed)        */
  void HandlePacket(const uint64_t header,int len,uint16_t *buffer);
  
  /*! \brief synch packet found (stream in sync)                     */
  bool      mIsSynced;
  /*! \brief first detected sync packet position in the bit stream   */
  int       mSyncPos;  
  /*! \brief next bit position of the currenly build header          */
  int       mHeadcd;
  /*! \brief next bit position curent payload word                   */
  int       mCurBit;
  /*! \brief current word under construction                         */
  uint16_t  mCurLen;
  /*! \brief pointer in the reception word buffer                    */
  uint16_t  *mWPointer;
  /*! \brief reception word buffer                                   */
  uint16_t  mFrame[1024];
  
  /*! \brief thread object                                           */
  std::thread  *TheThread;
  
  // statistic records
  /*! \brief statistic collection record                             */
  Stats     mStats;
  void (*user_handler)(int,int,int,int,int,short *);
  ReceiverHandler         *mRecHandl;
  /*! \brief User provided identifier                                */
  void                    *mUi;
  /*! \brief Begin of packet user handler                            */
  StartOfPacketHandler    *mStartOfPacketHandler;
  /*! \brief Normal packet user handler (zero supressed)             */
  PacketHandler           *mPacketHandler;
  /*! \brief Cluster sum packet user handler (zero supressed)        */
  ClusterSumPacketHandler *mClusterSumPacketHandler;
  /*! \brief Raw packet (non zero suppressed user handler)           */
  RawPacketHandler        *mPacketRawHandler;
  /*! \brief End of packet user handler                              */
  EndOfPacketHandler      *mEndOfPacketHandler;
  
  int      payloadLength;

};
#endif
