#ifndef GBTS
#define GBTS
#include <cstdint>
#include <bitset>
#include <list>
#include "GbtLink.h"
#include "Elink.h"
#include "Bits128.h"

/*!
 * \brief Emission GBT side emulation
 */


class GbtS: public gbtlink
{
  

struct GBits128
{
  Bits128    word;
  GBits128    *next;
};

  class Queue
  {
  public:
    Queue():QueueH(NULL),
            QueueT(NULL)
    {
    }
    GBits128 *Alloc()
    {
      if (QueueF==NULL)
        return new GBits128;
      GBits128 *ret = QueueF;
      QueueF = QueueF->next;
      return ret;
    }
    void Free(GBits128 *ret)
    {
      ret->next = QueueF;
      QueueF = ret;
    }   
    
    void EnQueue(GBits128 *ret)
    {
      if (QueueH == NULL) {
        QueueH = ret;
        QueueT = ret;
	ret->next = NULL;
      }
      else {
       QueueT->next = ret;
       QueueT = ret;
     }
    }
    
    GBits128 *DeQueue()
    {
      GBits128 *ret = QueueH; 
      QueueH = QueueH->next;
      if (QueueH == NULL) QueueT = NULL;
      return ret;
    }
    
    bool IsEmpty()
    {
      return (QueueH == NULL);
    }
    private:
      /*! \brief Sending queue head                                    */
      GBits128            *QueueH;
      /*! \brief Sending queue tail                                    */
      GBits128            *QueueT;
      static GBits128     *QueueF;
  };
  
public:
  GbtS();
  void PlugElink(const uint8_t socket,Elink *peer);
  void Process();
  
  bool              GbtWordAvailable();
  Bits128           GetWord();
private:
  /*! \brief Max number of Elinks ports                              */
  static const int    mMaxSocket = 40;
  /*! \brief Address of the Elink data provider connected to a port  */
  Elink               *mElinkMap[mMaxSocket];
  /*! \brief Number of active connected ports                        */
  int                 mNbRec;
  /*! \brief Sending queue head                                      */
  Queue               MyQueue;
  /*! \brief Sending queue tail                                      */
  GBits128            *QueueT;
};
#endif
