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
  if (DS_regeneration_count == 0 ) return;
  DS_regeneration_count--;
  DS_CurrentDualSampa->RegenerateData();
}

class DualSampaChecker
{
public:

  DualSampaChecker(DualSampa *DS):mDS(DS){}
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

  DualSampa   *mDS;  
};

BOOST_AUTO_TEST_CASE(DualSampaSyncEmission)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=1;
  int res = DualSampaChecker(DS_CurrentDualSampa).CheckSync();
  std::cout << res << std::endl;
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaEmptyTimeWindow)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=0;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  BOOST_ASSERT(res == 0);
  std::cout << res << std::endl;
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_CASE(DualSampaOneTimeWindow)
{
  DS_CurrentDualSampa = new DualSampa(1,2);
  DS_CurrentDualSampa->SetDataProvider(DspHandler);
  DS_regeneration_count=1;
  int res = DualSampaChecker(DS_CurrentDualSampa).CountBits();
  BOOST_ASSERT(res == 8192);
  std::cout << res << std::endl;
  delete DS_CurrentDualSampa;
}

BOOST_AUTO_TEST_SUITE_END()
