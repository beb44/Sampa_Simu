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


class Receiver
{
public:
  Receiver();
  Receiver(Elink &p);
  Receiver(int port,GbtR &p);
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
  void (*user_handler)(int,int,int,int,int,short *);
  ReceiverHandler     *mRecHandl;

  void HandlePacket(const uint64_t header,int len,uint16_t *buffer);
  
  uint64_t mSynchead;
  uint64_t mCurHead;
 
  Elink     &mPeer;
  bool      mIsSynced;
  int       mSyncPos;  
  int       mHeadcd;
  int       mCurBit;
  uint16_t  mCurLen;
  uint16_t  *mWPointer;
  uint16_t  mFrame[1024];
  
  std::thread  *TheThread;
  int       mPort;
  
  // statistic records
#ifdef STATS
  uint32_t  mPacketCount;
  uint32_t  mPacketCountByType[8];
#endif  
};
#endif
