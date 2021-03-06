#ifndef GBTR
#define GBTR
#include <bitset>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>
#include "RecInterface.h"
#include "GbtRInterface.h"
#include "Bits128.h"
#include "Elink.h"
#include "GbtElink.h"
#include "GbtLink.h"
/*!
 * \brief Reception GBT side emulation
 */
class GbtR: public GbtRInterface
{

class RecRecord
{
public:
  RecRecord(int port,RecInterface *rec): mPort(port),mRec(rec){};
  int          mPort;
  RecInterface *mRec;
};

public:
  GbtR(gbtlink &provider);
  Elink   &GetElink(RecInterface *rec,int const port);
  bool    Fetch(int const port,int const sample) ;
  uint8_t Read(int const port,int const sample)
  {
    return mCurWord[(sample>>1)-mOffset].Get((port<<1)+((~sample) & 1));
  };
  void    Push(Bits128 word);
  void Start();
  void Join();
  bool Joinable();
  void Process();
private:
  /*! \brief Reference of the data provider (i.e. remote GbtS)       */
  gbtlink                      &mDataProvider; 

  /*! \brief map of the remote pseudo Elinks (connecting Receivers)  */
  std::map<int,GbtElink *>     mElinkMap;

  /*! \brief Index number of the current GBT word                    */
  int                          mCurSample;
  
  /*! \brief Total number of ports 'in use'                          */
  int                          mNbLinks;

  /*! \brief number of readers waiting for the next GBT word         */
  int                          mNbSampleReaders;

  /*! \brief current GBT word                                        */
  static const int             mWindowsize = 8192;
  Bits128  	               mCurWord[mWindowsize];

  int                          mCurindex;
  int                          mOffset;
  int                          mPOffset;
  /*! \brief True if GBT word has been readout                       */
  bool                         mDataAvailable;

  /*! \brief mCurSample protection mutex                             */
  std::mutex		       mMutex;
  //std::map<int,RecInterface *> mRec;
  RecRecord                    *mRec[40];
  int                          mRecNumber;  
  /*! \brief thread object                                           */
  std::thread  *TheThread;
};
#endif
