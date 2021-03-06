#ifndef SAMPA
#define SAMPA
#include <stdlib.h>   
#include <cstdint>
#include <string.h>
#include <list>
#include "SampaElink.h"
/*!
 *  \brief sampa chip emulator
 *
 *  This class provides a emulation artefacts for sampa chip serialisation mechanism.\n
 *  it prodives methods to build dsp generated build frames and methods to emulate serial link.
 */
class Sampa  : public SampaElink
{
  class Queue
  {
  struct InternalItem 
  {
    uint16_t            *buffer;
    struct InternalItem *next;
  } ;
  
  public:
    Queue():QueueH(NULL),
            QueueT(NULL),
	    mSize(0)
    {
    }
    
    void push_back(uint16_t *bf)
    {
      InternalItem *ni = new InternalItem;
      ni->buffer = bf;
      ni->next = NULL;
      if (QueueH == NULL) {
        QueueH = ni;
        QueueT = ni;
      }
      else {
       QueueT->next = ni;
       QueueT = ni;
      }
      mSize++;
    }
     void pop_front()
    {
      InternalItem *ret = QueueH; 
      QueueH = QueueH->next;
      if (QueueH == NULL) QueueT = NULL;
      free(ret);
      mSize--;
      return ;
    }
    
    uint16_t *front()
    {
      return QueueH->buffer;
    }
    
    int size()
    {
      return (mSize);
    }
    private:
      /*! \brief Sending queue head                                    */
      InternalItem        *QueueH;
      /*! \brief Sending queue tail                                    */
      InternalItem        *QueueT;
      int                 mSize;
  };
    
public:
  explicit Sampa(uint16_t addr); 
  ~Sampa();
  void SelectChannel(const uint8_t chid);
  void ResetFrame();
  void AddData(int const len, uint16_t const *data);
 
  void DispFrame();
  void SendFrame();

  void MakeSync();
 
  bool    SerialAvailable(); 
  uint8_t GetSerial();
  
private:

  void MakeDataHeader();
  /*! \brief hardware address of the chip                            */
  uint8_t   mHaddr;
  /*! \brief current channel                                         */
  uint8_t   mChannel;

  /*! \brief current frame being built                               */
  uint16_t  mFrame[1024+5];

  /*! \brief next word to be written in the frame                    */
  uint16_t  mWPointer;

  /*! \brief header has been already computed                        */
  bool      mHasHeader;
  
  /*! \brief sending queue                                           */
  //std::list<uint16_t *>  mSendList;
  Queue                    mSendList;
  
  /*! \brief true if some data is available                          */
  bool      mDataAvailable;

  /*! \brief frame being readout                                     */
  uint16_t  *mCurSendFrame;

  /*! \brief current word being read out                             */
  int       mCurWord;

  /*! \brief current bit in the current word  being read out         */
  int       mCurBit;
  /*! \brief length of the frame being read out                      */
  uint16_t  mCurLen;
  
};
#endif
