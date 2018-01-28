#include <iostream>
#include <stdlib.h>  
#include <time.h>       
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>
#include <array>
#define BOOST_TEST_INCLUDED
#define BOOST_TEST_MODULE Sampa Simu Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> 
#include <boost/test/framework.hpp>
#include "../src/Elink.h"
#include "../src/GbtR.h"
#include "../src/GbtS.h"
#include "../src/Receiver.h"
#include "../src/DualSampa.h"

int regeneration_count;
int processed_data_count;
int processed_clustersum_count;
int processed_data_begin_count;
int processed_data_end_count;
DualSampa *CurrentDualSampa;

void ReceiverTestRecHandler(void * ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
std::string TestCase = boost::unit_test::framework::current_test_case().p_name;
  processed_data_count++;
//  std::cout <<  boost::unit_test::framework::current_test_case().p_name  << std::endl;
  if (TestCase == std::string("CheckOneDataElementReception"))
  {
    BOOST_ASSERT(ts==2);
    BOOST_ASSERT(nbsamp==2);
  }

  if (TestCase == std::string("CheckTwoDataElementReception"))
  {
    if (processed_data_count==1)
    {
      BOOST_ASSERT(ts==2);
      BOOST_ASSERT(nbsamp==2);
    }
    if (processed_data_count==2)
    {
      BOOST_ASSERT(ts==20);
      BOOST_ASSERT(nbsamp==7);
      for (int i=0;i<7;i++) BOOST_ASSERT(buff[i]==i);
    }
  }
}
void ReceiverTestRecClusterSumHandler(void *ui,int addr,int ch,int nbsamp,int ts,int val)
{
std::string TestCase = boost::unit_test::framework::current_test_case().p_name;
  processed_clustersum_count++;
//  std::cout <<  boost::unit_test::framework::current_test_case().p_name  << std::endl;
  if (TestCase == std::string("CheckOneDataClusterSumElementReception"))
  {
    // check incoming timestamp
    BOOST_ASSERT(ts==2);
    BOOST_ASSERT(nbsamp==2);
    // check incoming cluster sum
    BOOST_ASSERT(val==(1<<10)+32);
  }
  if (TestCase == std::string("CheckTwoDataClusterSumElementReception"))
  {
    if (processed_clustersum_count == 1) {
      // check incoming timestamp
      BOOST_ASSERT(ts==255);
      BOOST_ASSERT(nbsamp==10);
      // check incoming cluster sum
      BOOST_ASSERT(val==10000);
    }
    if (processed_clustersum_count == 2) {
      // check incoming timestamp
      BOOST_ASSERT(ts==512);
      BOOST_ASSERT(nbsamp==8);
      // check incoming cluster sum
      BOOST_ASSERT(val==26750);
    }
  }
}

void ReceiverTestBeginOfPacketHandler(void *ui,uint64_t header)
{
    processed_data_begin_count++;

}

void ReceiverTestEndOfPacketHandler(void *ui)
{
    processed_data_end_count++;
}


void DspHandler(int ref)
{
std::string TestCase = boost::unit_test::framework::current_test_case().p_name;
  //std::cout <<  boost::unit_test::framework::current_test_case().p_name  << std::endl;
  if ((TestCase == std::string("CheckHeaderReception"))                  ||
      (TestCase == std::string("CheckNoDataElementReception"))           ||
      (TestCase == std::string("CheckNoDataClusterSumElementReception")) ||
      (TestCase == std::string("CheckThreadModeNoPacket")))
  {
    if (regeneration_count==0) return;
    regeneration_count--;
    return;
  }
  if ((TestCase == std::string("CheckOneDataElementReception"))           ||
      (TestCase == std::string("CheckOneDataClusterSumElementReception")) ||
      (TestCase == std::string("CheckBeginOfPacketReception1"))           ||
      (TestCase == std::string("CheckBeginOfPacketReception2"))           ||
      (TestCase == std::string("CheckEndOfPacketReception1"))             ||
      (TestCase == std::string("CheckEndOfPacketReception2")))
  {
    uint16_t data[4] = {2,2,0x1,0x20};
    if (regeneration_count==0) return;
    regeneration_count--;
    CurrentDualSampa->ResetFrames();
    CurrentDualSampa->AddData(0,4,data);
    CurrentDualSampa->SendFrames();
    CurrentDualSampa->RegenerateData(); 
    return;
  }
  if ((TestCase == std::string("CheckTwoDataElementReception")) ||
      (TestCase == std::string("CheckBeginAndEndOfPacketReception1")))
  {
    uint16_t data[13] = {2,2,0x1,0x20,7,20,0,1,2,3,4,5,6};
    if (regeneration_count==0) return;
    regeneration_count--;
    CurrentDualSampa->ResetFrames();
    CurrentDualSampa->AddData(0,13,data);
    CurrentDualSampa->SendFrames();
    CurrentDualSampa->RegenerateData(); 
    return;
  }
  if ((TestCase == std::string("CheckTwoDataClusterSumElementReception")) ||
      (TestCase == std::string("CheckBeginAndEndOfPacketReception2")))
  {
    uint16_t data[8] = {10,255,(10000/1024),(10000%1024),8,512,(26750/1024),(26750%1024)};
    if (regeneration_count==0) return;
    regeneration_count--;
    CurrentDualSampa->ResetFrames();
    CurrentDualSampa->AddData(0,8,data);
    CurrentDualSampa->SendFrames();
    CurrentDualSampa->RegenerateData(); 
    return;
  }
}
BOOST_AUTO_TEST_SUITE(receiver_test)

BOOST_AUTO_TEST_CASE(InstanceCreationWithDualSampa)
{
DualSampa ds(10,12);
Receiver rec(ds);
  BOOST_ASSERT((true));   
}
BOOST_AUTO_TEST_CASE(InstanceCreationWithGbtR)
{
GbtS     gbt_s1;
GbtR     gbt_r1(gbt_s1);
Receiver rec(0,gbt_r1);
  BOOST_ASSERT((true));   
}

BOOST_AUTO_TEST_CASE(CheckHeaderReception)
{

  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa);
  regeneration_count =1;
  rec.Process();
  BOOST_ASSERT(true);
  delete CurrentDualSampa;
}
BOOST_AUTO_TEST_CASE(CheckOneDataElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count == 1);
  BOOST_ASSERT(processed_clustersum_count == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckOneDataClusterSumElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecClusterSumHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count == 0);
  BOOST_ASSERT(processed_clustersum_count == 1);
  BOOST_ASSERT(processed_data_begin_count == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckTwoDataElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 2);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckTwoDataClusterSumElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecClusterSumHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 2);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckNoDataElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 10;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckNoDataClusterSumElementReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecClusterSumHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 5;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckBeginOfPacketReception1)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,ReceiverTestBeginOfPacketHandler,
                                   ReceiverTestRecHandler,
				   NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 1);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 1);
  BOOST_ASSERT(processed_data_end_count   == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckBeginOfPacketReception2)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,ReceiverTestBeginOfPacketHandler,
                                   ReceiverTestRecClusterSumHandler,
				   NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 1);
  BOOST_ASSERT(processed_data_begin_count == 1);
  BOOST_ASSERT(processed_data_end_count   == 0);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckEndOfPacketReception1)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,
                                   ReceiverTestRecHandler,
				   ReceiverTestEndOfPacketHandler);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 1);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 1);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckEndOfPacketReception2)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,
                                   ReceiverTestRecClusterSumHandler,
				   ReceiverTestEndOfPacketHandler);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 1);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 1);
  delete CurrentDualSampa;
}
BOOST_AUTO_TEST_CASE(CheckBeginAndEndOfPacketReception1)
{
int  loop = 1+rand()%10;
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,ReceiverTestBeginOfPacketHandler,
                                   ReceiverTestRecHandler,
				   ReceiverTestEndOfPacketHandler);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = loop;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == loop*2);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == loop);
  BOOST_ASSERT(processed_data_end_count   == loop);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckBeginAndEndOfPacketReception2)
{
int  loop = 1+rand()%10;
 
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,ReceiverTestBeginOfPacketHandler,
                                   ReceiverTestRecClusterSumHandler,
				   ReceiverTestEndOfPacketHandler);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count         = loop;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == loop*2);
  BOOST_ASSERT(processed_data_begin_count == loop);
  BOOST_ASSERT(processed_data_end_count   == loop);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(CheckThreadModeNoTraffic)
{ 
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,
                                   ReceiverTestRecClusterSumHandler,
				   NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count         = 0;
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Start();
  rec.Join();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  BOOST_ASSERT(rec.Joinable()             == false);
 
  delete CurrentDualSampa;
}
BOOST_AUTO_TEST_CASE(CheckThreadModeNoPacket)
{
int  loop = 1+rand()%10;
 
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,
                                   ReceiverTestRecClusterSumHandler,
				   NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  processed_clustersum_count = 0;
  processed_data_count       = 0;
  processed_data_begin_count = 0;
  processed_data_end_count   = 0;
  rec.Start();
  rec.Join();
  BOOST_ASSERT(processed_data_count       == 0);
  BOOST_ASSERT(processed_clustersum_count == 0);
  BOOST_ASSERT(processed_data_begin_count == 0);
  BOOST_ASSERT(processed_data_end_count   == 0);
  BOOST_ASSERT(rec.Joinable()             == false);
 
  delete CurrentDualSampa;
}

class ReceiverTestDirtyTraffic: public Elink
{
public:
  ReceiverTestDirtyTraffic(int nbBits):mNbBit(nbBits){};
  bool SerialAvailable()
  {
    return (mNbBit != 0);
  }
  uint8_t GetSerial()
  {
    BOOST_ASSERT(mNbBit !=0);
    mNbBit--;
    return (rand()%2);
  }
private:
  int    mNbBit;
};
BOOST_AUTO_TEST_CASE(CheckDirtyTraffic)
{
ReceiverTestDirtyTraffic tester(rand()%200000+1);

  Receiver rec(tester,0,NULL,
                        ReceiverTestRecClusterSumHandler,
		        NULL);
  rec.Start();
  rec.Join();
  BOOST_ASSERT(rec.Joinable()             == false);

}
int ReceiverEnduranceRecDataElt;
int ReceiverEnduranceSentDataElt;

void TrafficGenerator(int ref)
{

  if (regeneration_count == 0) return;
  regeneration_count--;
  for (int chip=0;chip<2;chip++)
  {
    for (int channel=0;channel<32;channel++)
    {
      int credit = (rand()%100);
      bool somethingtosend = false;
      
      CurrentDualSampa->SelectChannel(chip,channel);
      // assume average 30% occupation
      while (credit<30)
      {
        if (somethingtosend==false)
          CurrentDualSampa->ResetFrames();
	somethingtosend= true;
	int  nb = rand()%10+1;
	uint16_t  buffer[20];
	for (int i=0;i<nb;i++) buffer[i+2] = rand()%800+200;
	buffer[0] = nb;
	buffer[1] = rand()%200;
        CurrentDualSampa->AddData(chip,nb+2,buffer);
	credit += (rand()%100);
	ReceiverEnduranceSentDataElt++;
      }
      if (somethingtosend) {
        CurrentDualSampa->SendFrames();
      }
    }	
  }
  CurrentDualSampa->RegenerateData(); 
  return;
}
void ReceiverEnduranceTestHandler(void * ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
  BOOST_ASSERT((addr == 11) || (addr == 7));
  BOOST_ASSERT((0<=ch) && (ch<32));
  ReceiverEnduranceRecDataElt++;
}
BOOST_AUTO_TEST_CASE(ReceiverEnduranceTest)
{
  ReceiverEnduranceRecDataElt  = 0;
  ReceiverEnduranceSentDataElt =0;
  CurrentDualSampa = new DualSampa(7,11);
  Receiver rec(*CurrentDualSampa,0,NULL,
                                   ReceiverEnduranceTestHandler,
				   NULL);
  CurrentDualSampa->SetDataProvider(TrafficGenerator);
  regeneration_count = 10000;
  rec.Process();
  BOOST_ASSERT(ReceiverEnduranceSentDataElt != 0);
  BOOST_ASSERT(ReceiverEnduranceSentDataElt == ReceiverEnduranceRecDataElt);
  delete CurrentDualSampa;
}

BOOST_AUTO_TEST_SUITE_END()
