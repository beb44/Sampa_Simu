#include <iostream>
#include "GbtS.h"
#include "Bits128.h"
using namespace std;
GbtS::GbtS() 
{

  mSendList.clear();
  // reset elink map
  for (int i=0;i<mMaxSocket;i++) mElinkMap[i] = 0;
  mNbRec =0;
}

void GbtS::PlugElink(const uint8_t socket,Elink *peer)
{
  if (!(socket<mMaxSocket)) return;
  mElinkMap[socket] = peer;
  mNbRec +=1;
}

void GbtS::Process()
{
int     active_responder;
uint8_t bit1;
uint8_t bit2;

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
        mCurWord.Set(i*2,bit2 & 1);
        mCurWord.Set(i*2+1, bit1 & 1);
	active_responder++;
      }
      else {
        mCurWord.Set(i*2,0);
        mCurWord.Set(i*2+1,0);
      }
    }
    if (active_responder == mNbRec) {
      //
      // all link have returned data, push word for sending
      //
      mSendList.push_back(mCurWord);
    }
  } // end while (active....
}

bool GbtS::GbtWordAvailable()
{
int     active_responder = 0;
uint8_t bit1;
uint8_t bit2;

  for (int i=0;i<mMaxSocket;i++) {
    if (mElinkMap[i] != 0) {
      //
      // a sampa is pluged on this socket
      // read two bits if available
      if (!(mElinkMap[i]->SerialAvailable())) continue;
      bit1 = mElinkMap[i]->GetSerial();
      if (!(mElinkMap[i]->SerialAvailable())) continue;
      bit2 = mElinkMap[i]->GetSerial();
      mCurWord.Set(i*2,bit2 & 1);
      mCurWord.Set(i*2+1, bit1 & 1);
      active_responder++;
    }
    else {
      mCurWord.Set(i*2,0);
      mCurWord.Set(i*2+1,0);
    }
  }
  if (active_responder == mNbRec) {
    //
    // all link have returned data, push word for sending
    //
    mSendList.push_back(mCurWord);
  }
  return (active_responder == mNbRec);
}

Bits128 GbtS::GetWord()
{
Bits128   word;

  if (mSendList.size() ==0) return Bits128(0xff,0xff,0xff,0xff);  
  word = mSendList.front() ;
  mSendList.pop_front();
  return word;
}
