#ifndef RECEIVER
#define RECEIVER
#include <thread>
#include "SampaHead.h"
#include "Elink.h"
#include "GbtR.h"

class ReceiverHandler
{
public:
  virtual void  RecHandler(int addr,int ch,int nbsamp,int ts,int len, short *buff) = 0;
};

/*!
 * \brief Received packet parser
 */

class Receiver
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
  Receiver(int port,GbtR &provider);
  void SetUserHandler(void (*foo)(int,int,int,int,int,short *));
  void SetUserHandler(ReceiverHandler* handler);
  void Start();
  void Join();
  bool Joinable();
  void Process();
#ifdef STATS
  void DisplayStats();
  void ResetStats();
#endif
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
  ReceiverHandler     *mRecHandl;

};
#endif
