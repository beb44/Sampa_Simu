#include <iostream>
#include <stdlib.h>  
#include <time.h>       
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>
#include <array>
#define BOOST_TEST_INCLUDED
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> 
#include <boost/test/framework.hpp>

#include "../src/DualSampa.h"

#define SYNC 0b01010101010101010101000000111100000000000100010011
BOOST_AUTO_TEST_SUITE(dualsampa_test)

BOOST_AUTO_TEST_CASE(DualSampaCreation)
{
DualSampa ds(10,12);
  BOOST_ASSERT((true));   
}

DualSampa *DS_CurrentDualSampa;

int DS_regeneration_count;

void DspHandler(int ref)
{
std::string TestCase = boost::unit_test::framework::current_test_case().p_name;
  if (TestCase == std::string("DualSampaInternalRef")) {
    BOOST_ASSERT(ref == 0xBAE2);
  }
  if (DS_regeneration_count == 0 ) return;
  DS_regeneration_count--;
  if (TestCase == std::string("DualSampaDataPacket")) {
    uint16_t data[4] = {2,1,3,4};
    DS_CurrentDualSampa->ResetFrames();
    DS_CurrentDualSampa->AddData(0,4,data);
    DS_CurrentDualSampa->SendFrames();
  }
  
  if (TestCase == std::string("DualSampaDataPacketOnChannel31")) {
    uint16_t data[4] = {2,1,3,4};
    DS_CurrentDualSampa->SelectChannel(0,31);
    DS_CurrentDualSampa->ResetFrames();
    DS_CurrentDualSampa->AddData(0,4,data);
    DS_CurrentDualSampa->SendFrames();
  }
  
  
  if (TestCase == std::string("DualSampaDataPacketOnSecondSampa")) {
    uint16_t data[4] = {2,1,3,4};
    DS_CurrentDualSampa->ResetFrames();
    DS_CurrentDualSampa->SelectChannel(1,15);
    DS_CurrentDualSampa->AddData(1,4,data);
    DS_CurrentDualSampa->SendFrames();
  }
  if (TestCase == std::string("DualSampaVeryLongDataPacket")) {
    // Warning : this very long packet spans over TWO time windows
    //           that's why DS_regeneration_count is tested so....
    if (DS_regeneration_count == 1)
    {
      uint16_t data[1000];
      DS_CurrentDualSampa->ResetFrames();
      DS_CurrentDualSampa->AddData(0,1000,data);
      DS_CurrentDualSampa->SendFrames();
    }  
  }
  
  if (TestCase == std::string("DualSampaManyDataPacket")) {
    uint16_t data[4] = {2,1,3,4};
    for (int i=0;i<32;i++)
    {
      DS_CurrentDualSampa->ResetFrames();
      DS_CurrentDualSampa->SelectChannel(i,15);
      DS_CurrentDualSampa->AddData(0,4,data);
      DS_CurrentDualSampa->SendFrames();
      DS_CurrentDualSampa->ResetFrames();
      DS_CurrentDualSampa->SelectChannel(i,15);
      DS_CurrentDualSampa->AddData(1,4,data);
      DS_CurrentDualSampa->SendFrames();
    }
  }
  DS_CurrentDualSampa->RegenerateData();
}

class DualSampaChecker
{
public:

  DualSampaChecker(DualSampa *DS):mDS(DS){}
  int GetSync()
  {
    uint64_t curhead = 0;
    int watchdog = 100;
    while (mDS->SerialAvailable())
    {
      curhead = (curhead >> 1)+((uint64_t)(mDS->GetSerial()) << 49);
      if (curhead == SYNC)return true;
      if (watchdog-- == 0) return false;
    }
    return false;
  }
  
  uint64_t ReadHeader()
  {
    uint64_t curhead=0;
    for (int i=0;i<50;i++) {
      BOOST_ASSERT(mDS->SerialAvailable());
      curhead = (curhead >> 1)+((uint64_t)(mDS->GetSerial()) << 49);
    }
    return curhead;    
  }
  int CheckSync()
  {
    uint64_t curhead = 0;
    int pos =0;
    int watchdog = 100000;
    while (mDS->SerialAvailable())
    {
      BOOST_ASSERT(watchdog != 0);
      watchdog--;
      curhead = (curhead >> 1)+((uint64_t)(mDS->GetSerial()) << 49);
      if (curhead == SYNC){
         BOOST_ASSERT(true); 
	 return pos-49;
      }
      pos++;
    }
    BOOST_ASSERT(false);
    return 0;
  }
  
  int CountBits()
  {
    int nb =0;
    while (mDS->SerialAvailable())
    {
      BOOST_ASSERT(nb++ != 10000000);  // watchdog
      mDS->GetSerial();
    }
    return nb;
  }
  
  void checkdata(uint16_t expected_length,uint16_t *expected_data)
  {
    uint64_t curhead = 0;
    uint16_t rec_data[1024];
    uint64_t curword;
    
    BOOST_ASSERT(GetSync());
    curhead = ReadHeader();
    // check packet type
    uint8_t packetType = ((curhead >> 7) & 0x07);
    BOOST_ASSERT(packetType == 0x04);
    uint16_t packetLength = ((curhead >> 10) & 0x3ff);
    BOOST_ASSERT(packetLength == expected_length);
    lastRecChip = ((curhead >> 20) & 0x0f);
    lastRecChannel = ((curhead >> 24) & 0x1f);
    for (int i=0;i<packetLength;i++) {
      curword = 0;
      for (int j=0;j<10;j++) {
        BOOST_ASSERT(mDS->SerialAvailable());
	curword =  curword +((uint64_t)(mDS->GetSerial())<<j);	
      }
      if (expected_data != NULL) BOOST_ASSERT(curword == expected_data[i]);
    }      
  }
  
  void checkseveraldata(int nb)
  {
    BOOST_ASSERT(GetSync());
    for (int i=0;i<nb;i++)
    {
      uint64_t curhead = ReadHeader();
      uint8_t packetType = ((curhead >> 7) & 0x07);
      BOOST_ASSERT(packetType == 0x04);
      uint16_t packetLength = ((curhead >> 10) & 0x3ff);
      for (int j=0;j<10*packetLength;j++) {
        BOOST_ASSERT(mDS->SerialAvailable());
	mDS->GetSerial();
      } 
    }
    uint64_t curhead = ReadHeader();
    BOOST_ASSERT(curhead == SYNC);
  }
  
  

  DualSampa   *mDS;  
  uint8_t     lastRecChannel;
  uint8_t     lastRecChip;
};

BOOST_AUTO_TEST_CASE(DualSampaSyncEmission)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=1;
  int res = DualSampaChecker(DS_CurrentDualSampa).CheckSync();
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaEmptyTimeWindow)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=0;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  BOOST_ASSERT(res == 0);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaOneTimeWindow)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=1;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  BOOST_ASSERT(res == 8192);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampamanyTimeWindows)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=10;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  BOOST_ASSERT(res == 81920);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaInternalRef)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  DS_regeneration_count=0;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaDataPacket)
{
uint16_t data[4] = {2,1,3,4};
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  DS_regeneration_count=1;
  DualSampaChecker(DS_CurrentDualSampa).checkdata(4,data);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaVeryLongDataPacket)
{
uint16_t data[4] = {2,1,3,4};
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  //
  // Warning : this very long packet spans over TWO time windows
  //           that's why DS_regeneration_count is set so....
  DS_regeneration_count=2;
  DualSampaChecker(DS_CurrentDualSampa).checkdata(1000,NULL);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaDataPacketOnSecondSampa)
{
uint16_t data[4] = {2,1,3,4};


  DS_CurrentDualSampa = new DualSampa(1,7);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  DualSampaChecker DSchecker(DS_CurrentDualSampa);
  DS_regeneration_count=1;
  DSchecker.checkdata(4,data);
  BOOST_ASSERT(DSchecker.lastRecChip == 7);
  BOOST_ASSERT(DSchecker.lastRecChannel == 15);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaDataPacketOnChannel31)
{
uint16_t data[4] = {2,1,3,4};

  DS_CurrentDualSampa = new DualSampa(1,7);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  DualSampaChecker DSchecker(DS_CurrentDualSampa);
  DS_regeneration_count=1;
  DSchecker.checkdata(4,data);
  BOOST_ASSERT(DSchecker.lastRecChannel == 31);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaManyDataPacket)
{
uint16_t data[4] = {2,1,3,4};
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_CurrentDualSampa->SetInternalRef(0xBAE2);
  //
  // Warning : this very long packet spans over TWO time windows
  //           that's why DS_regeneration_count is set so....
  DS_regeneration_count=2;
  DualSampaChecker(DS_CurrentDualSampa).checkseveraldata(64);
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_SUITE_END()
