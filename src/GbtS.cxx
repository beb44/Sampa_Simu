#include <iostream>
#include "GbtS.h"
#include "Bits128.h"
using namespace std;
GbtS::GBits128 *GbtS::Queue::QueueF = NULL;
 

/*!
 * \brief constructor
 */
GbtS::GbtS() : mNbRec(0)
{

  // reset elink map
  for (int i=0;i<mMaxSocket;i++) mElinkMap[i] = NULL;
}

/*!
 * \brief connect an Elink of a free port
 *
 * \param socket socket number from 0 to mMaxSocket-1 (0-39)
 * \param peer of the Elink interface
 */

void GbtS::PlugElink(const uint8_t socket,Elink *peer)
{
  if (!(socket<mMaxSocket)) return;
  mElinkMap[socket] = peer;
  mNbRec +=1;
}

/*!
 * \brief main processing methods
 *
 * collects on each connected Sampa/Dualsampa a new bit.\n
 * if no bit is available on a given Sampa/Dualsampa, 0 value is used.\n
 * if no bit is available on ALL Sampa/Dualsampa, the processing ends.
 * All collected bits are gathered into one GbtWord and push in the
 * transmission queue.
 */

void GbtS::Process()
{
int     active_responder;
uint8_t bit1;
uint8_t bit2;
GBits128 *mCurWord = MyQueue.Alloc();

  while (active_responder != 0) {
    // reset nb responder
    active_responder = 0;
    for (int i=0;i<mMaxSocket;i++) {
      if (mElinkMap[i] != 0) {
	if ((mElinkMap[i]->SerialAvailable()) == false) { 
	  continue;
	}
	bit1 = mElinkMap[i]->GetSerial();
	if ((mElinkMap[i]->SerialAvailable()) == false) { 
	  continue;
	}
	bit2 = mElinkMap[i]->GetSerial();
        mCurWord->word.Set(i*2,bit2 & 1);
        mCurWord->word.Set(i*2+1, bit1 & 1);
	active_responder++;
      } 
      else {
        //mCurWord->word.Set(i*2,0);
        //mCurWord->word.Set(i*2+1,0);
      }
    }
    if (active_responder == mNbRec) {
      //
      // all link have returned data, push word for sending
      //
      MyQueue.EnQueue(mCurWord);
    }
    else {
      MyQueue.Free(mCurWord);
    }
  } // end while (active....
}

/*!
 * \brief Check is something is present on Elinks
 *
 * collects on each connected Sampa/Dualsampa a new bit.\n
 * if no bit is available on a given Sampa/Dualsampa, 0 value is used.\n
 * if no bit is available on ALL Sampa/Dualsampa, the processing ends.
 * All collected bits are gathered into one GbtWord and push in the
 * transmission queue.
 *
 * \return True is some data is available
 */

bool GbtS::GbtWordAvailable()
{
int     active_responder = 0;
uint8_t bit1;
uint8_t bit2;
GBits128 *mCurWord = MyQueue.Alloc();
  //int j=0;
  for (int i=0;i<mMaxSocket;i++) {
    if (mElinkMap[i] != 0) {
      //
      // a sampa is pluged on this socket
      // read two bits if available
      if (!(mElinkMap[i]->SerialAvailable())) continue;
      bit1 = mElinkMap[i]->GetSerial();
      if (!(mElinkMap[i]->SerialAvailable())) continue;
      bit2 = mElinkMap[i]->GetSerial();
      //mCurWord.w[i*2]= bit2;
      //mCurWord.w[i*2+1]= bit1;
     
      mCurWord->word.Set(i*2,bit2 & 1);
      mCurWord->word.Set(i*2+1, bit1 & 1);
      active_responder++;
    }
  } 
  if (active_responder == mNbRec) {
    //
    // all link have returned data, push word for sending
    //
    MyQueue.EnQueue(mCurWord);
  }
  else {
    free(mCurWord);
  }
  return (active_responder == mNbRec);
}

/*!
 * \brief Returns the first GbtWord in the sending queue (and remove it fromm
 *        the queue)
 *
 * If not GbtWord is present , the 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF word
 * is returned.
 *
 * \return Next GbtWord if present, -1 otherwise
 */

Bits128 GbtS::GetWord()
{
Bits128   word;
GBits128  *tmp;

  if (MyQueue.IsEmpty()) return Bits128(0xff,0xff,0xff,0xff);
  tmp = MyQueue.DeQueue();
  word = tmp->word; 
  MyQueue.Free(tmp);
  return word;
}
