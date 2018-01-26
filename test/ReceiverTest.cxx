#include <iostream>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>
#define BOOST_TEST_INCLUDED
#define BOOST_TEST_MODULE test module name
#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> 
#include <boost/test/framework.hpp>
#include "../src/GbtR.h"
#include "../src/GbtS.h"
#include "../src/Receiver.h"
#include "../src/DualSampa.h"

int regeneration_count;
int processed_data_count;
DualSampa *CurrentDualSampa;

void ReceiverTestRecHandler(void * ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
  std::cout <<  boost::unit_test::framework::current_test_case().p_name  << std::endl;
  if (boost::unit_test::framework::current_test_case().p_name == std::string("CheckOneDataElememtReception"))
  {
    BOOST_ASSERT(ts==2);
    processed_data_count++;
  }
}
void DspHandler(int ref)
{
  std::cout <<  boost::unit_test::framework::current_test_case().p_name  << std::endl;
  if (boost::unit_test::framework::current_test_case().p_name == std::string("CheckHeaderReception"))
  {
    return;
  }
  if (boost::unit_test::framework::current_test_case().p_name == std::string("CheckOneDataElememtReception"))
  {
    uint16_t data[4] = {7,2,0x1,0x20};
    if (regeneration_count==0) return;
    regeneration_count--;
    CurrentDualSampa->ResetFrames();
    CurrentDualSampa->AddData(0,4,data);
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
}
BOOST_AUTO_TEST_CASE(CheckOneDataElememtReception)
{
  
  CurrentDualSampa = new DualSampa(1,2);
  Receiver rec(*CurrentDualSampa,0,NULL,ReceiverTestRecHandler,NULL);
  CurrentDualSampa->SetDataProvider(DspHandler);
  regeneration_count = 1;
  processed_data_count = 0;
  rec.Process();
  BOOST_ASSERT(processed_data_count == 1);
}


BOOST_AUTO_TEST_SUITE_END()
