#ifndef GBTR
#define GBTR
#include <bitset>
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
public:
  GbtR(gbtlink &provider);
  Elink   &GetElink(RecInterface *rec,int const port);
  bool    Fetch(int const port,int const sample) ;
  uint8_t Read(int const port,int const sample);
  void    Push(Bits128 word);
  void    Start();
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
  static const int             mWindowsize = 10000;
  Bits128  	               mCurWord[mWindowsize];

  int                          mCurindex;
  int                          mOffset;
  /*! \brief True if GBT word has been readout                       */
  bool                         mDataAvailable;

  /*! \brief mCurSample protection mutex                             */
  std::mutex		       mMutex;
  std::map<int,RecInterface *> mRec;
 
};
#endif
