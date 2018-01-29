#include <iostream>
#include <stdlib.h>  
#include <time.h>       
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>
#include <array>
#include <list>
#define BOOST_TEST_INCLUDED
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> 
#include <boost/test/framework.hpp>

#include "../src/GbtR.h"
BOOST_AUTO_TEST_SUITE(gbtr_test)

class GbtRChecker : public gbtlink
{
public:
  bool GbtWordAvailable()
  {
    return (mNbWords != 0);
  }
  Bits128 GetWord()
  {
    mNbWords--;
    return Bits128(0xffffffff,0xAAAAAAAA,0x55555555,0);
  }
  void setNbWords(int nb) {mNbWords = nb; }
  int   mNbWords;
};

class GbtRCheckerIntegrity : public GbtRChecker
{
public:
  Bits128 GetWord()
  {
    mNbWords--;
    return Bits128(0xffffffff,0xAAAAAAAA,0x55555555,0);
  }
};
class GBTR_rec : public RecInterface
{
public:
  GBTR_rec(Elink &ref): mRef(ref),mNbBits(0) {}
  void Start()
  {
    try {
    TheThread = new std::thread(&GBTR_rec::Process,this);
    }
    catch (const std::exception& e) {
      std::cout << "execption catched" << std::endl;
      throw;
    }
  }
  void Join()
  {
    TheThread->join();
  }
  void Process()
  {
    mNbBits = 0;
    while (mRef.SerialAvailable()) {
      mRef.GetSerial();
      mNbBits ++;
    }
  }
  void Push(uint8_t data)
  {
    mNbBits ++;
  }

  Elink &mRef;
  std::thread *TheThread;
  int mNbBits;
};

class GBTR_recInt : public GBTR_rec
{
public:
  GBTR_recInt(Elink &ref): GBTR_rec(ref) {}
  void Process()
  {
    mNbBits = 0;
    while (mRef.SerialAvailable()) {
      if (mMode == 0) BOOST_ASSERT(mRef.GetSerial() == 0);
      if (mMode == 1) BOOST_ASSERT(mRef.GetSerial() == ((~mNbBits) &1));
      if (mMode == 2) BOOST_ASSERT(mRef.GetSerial() == (mNbBits &1));
      if (mMode == 3) BOOST_ASSERT(mRef.GetSerial() == 1);
      mNbBits ++;
    }
  }
  void SetMode(int mode) {mMode=mode;}
  int mMode;

};

class GBTR_push : public RecInterface
{
public:
  GBTR_push() : mNbBits(0) {}
  void Push(uint8_t data)
  {
    mNbBits ++;
  }
  int mNbBits;

};

class GBTR_pushInt : public RecInterface
{
public:
  GBTR_pushInt(int mode) : mNbBits(0),mMode(mode) {}
  void Push(uint8_t data)
  {
    mNbBits ++;
    if (mMode == 0) BOOST_ASSERT(data == 0);
    if (mMode == 1) BOOST_ASSERT(data == ((~mNbBits) &1));
    if (mMode == 2) BOOST_ASSERT(data == (mNbBits &1));
    if (mMode == 3) BOOST_ASSERT(data == 1);
  }
  int mNbBits;
  int mMode;

};

GbtRChecker GBTR_checker;
GbtRCheckerIntegrity GBTR_checkerInt;

BOOST_AUTO_TEST_CASE(GbtRCreation)
{

 GBTR_checker.setNbWords(0);
 GbtR gbtr(GBTR_checker);
  BOOST_ASSERT((true));   
}



BOOST_AUTO_TEST_CASE(GbtROneReceiverNoData)
{

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(0);
  Elink &MyLink = gbtr.GetElink(0,NULL);
  BOOST_ASSERT(MyLink.SerialAvailable() == false);   
}


BOOST_AUTO_TEST_CASE(GbtRoneReceiverFullPage)
{
int nbrecbit;

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(8192);
  Elink &MyLink = gbtr.GetElink(0,NULL);
  nbrecbit =0;
  while (MyLink.SerialAvailable()) {
    MyLink.GetSerial();
    nbrecbit++;
  }
  BOOST_ASSERT(nbrecbit == 16384);   
}

BOOST_AUTO_TEST_CASE(GbtRoneReceiverFullPagePushMode)
{
GbtR gbtr(GBTR_checker);
GBTR_push MyLink; 

  GBTR_checker.setNbWords(8192);
  gbtr.GetElink(&MyLink,0);
  gbtr.Start();
  BOOST_ASSERT(MyLink.mNbBits == 16384);   
}

BOOST_AUTO_TEST_CASE(GbtRoneReceiverFullPagePlusone)
{
int nbrecbit;

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(8193);
  Elink &MyLink = gbtr.GetElink(0,NULL);
  nbrecbit =0;
  while (MyLink.SerialAvailable()) {
    MyLink.GetSerial();
    nbrecbit++;
  }
  BOOST_ASSERT(nbrecbit == 16386);   
}

BOOST_AUTO_TEST_CASE(GbtRoneReceiverFullPageMinusOne)
{
int nbrecbit;

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(8191);
  Elink &MyLink = gbtr.GetElink(0,NULL);
  nbrecbit =0;
  while (MyLink.SerialAvailable()) {
    MyLink.GetSerial();
    nbrecbit++;
  }
  BOOST_ASSERT(nbrecbit == 16382);   
}

BOOST_AUTO_TEST_CASE(GbtR40ReceiverNoData)
{

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(0);

  GBTR_rec *MyLink[40];
  for (int i=0;i<40;i++) 
  {
    MyLink[i] = new GBTR_rec(gbtr.GetElink(NULL,i)) ;
  }
  for (int i=0;i<40;i++) MyLink[i]->Start();
  for (int i=0;i<40;i++) MyLink[i]->Join();
   
  for (int i=0;i<40;i++) BOOST_ASSERT(MyLink[i]->mNbBits == 0);   
   
  for (int i=0;i<40;i++) delete MyLink[i];
}

BOOST_AUTO_TEST_CASE(GbtR40ReceiverFullPage)
{

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(0);
  GBTR_checker.setNbWords(8192);

  GBTR_rec *MyLink[40];
  for (int i=0;i<40;i++) 
  {
    MyLink[i] = new GBTR_rec(gbtr.GetElink(NULL,i)) ;
  }
  for (int i=0;i<40;i++) MyLink[i]->Start();
  for (int i=0;i<40;i++) MyLink[i]->Join();
   
  for (int i=0;i<40;i++) BOOST_ASSERT(MyLink[i]->mNbBits == 16384);   
   
  for (int i=0;i<40;i++) delete MyLink[i];
}


BOOST_AUTO_TEST_CASE(GbtR40ReceiverNoDataPushMode)
{

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(0);

  GBTR_push MyLink[40];
  for (int i=0;i<40;i++) 
  {
    gbtr.GetElink(&MyLink[i],i);
  }
  gbtr.Start();
     
  for (int i=0;i<40;i++) BOOST_ASSERT(MyLink[i].mNbBits == 0);   
   
}

BOOST_AUTO_TEST_CASE(GbtR40ReceiverFullPagePushMode)
{

GbtR gbtr(GBTR_checker);

  GBTR_checker.setNbWords(8192);

  GBTR_push MyLink[40];
  for (int i=0;i<40;i++) 
  {
    gbtr.GetElink(&MyLink[i],i);
  }
  gbtr.Start();
     
  for (int i=0;i<40;i++) BOOST_ASSERT(MyLink[i].mNbBits == 16384);      
}

BOOST_AUTO_TEST_CASE(GbtR40ReceiverDataIntegrity)
{

GbtR gbtr(GBTR_checkerInt);

  GBTR_checkerInt.setNbWords(1000);

  GBTR_recInt *MyLink[40];
  for (int i=0;i<40;i++) 
  {
    MyLink[i] = new GBTR_recInt(gbtr.GetElink(NULL,i)) ;
    MyLink[i]->SetMode(i/16);
  }
  for (int i=0;i<40;i++) MyLink[i]->Start();
  for (int i=0;i<40;i++) MyLink[i]->Join();
   
  for (int i=0;i<40;i++) delete MyLink[i];
   
}


BOOST_AUTO_TEST_CASE(GbtR40ReceiverDataIntegrityPushMode)
{

GbtR gbtr(GBTR_checkerInt);

  GBTR_checkerInt.setNbWords(1000);

  GBTR_pushInt *MyLink[40];
  for (int i=0;i<40;i++) 
  {
    MyLink[i] = new GBTR_pushInt(i/16);
    gbtr.GetElink(MyLink[i],i);
  }
  gbtr.Start();
     
  for (int i=0;i<40;i++) delete MyLink[i];
   
}


BOOST_AUTO_TEST_SUITE_END()

