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
#include "TestChrono.h"
BOOST_AUTO_TEST_SUITE(receiver__performance_test)

int regeneration_count[800];  // up to 800 processes


DualSampa *CurrentDualSampa[800]; // up to 800 DualSampa

void DspHandlerIdle(int ref)
{
  if (regeneration_count[ref]==0) return;
  regeneration_count[ref]--;
  CurrentDualSampa[ref]->RegenerateData(); 
  return;
}
void ReceiverPerformanceTestHandler(void * ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
}

BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToDualSampa)
{
  TestChrono chrono;
  chrono.Start();
  CurrentDualSampa[0] = new DualSampa(7,11);
  CurrentDualSampa[0]->SetDataProvider(DspHandlerIdle);
  CurrentDualSampa[0]->SetInternalRef(0);
  regeneration_count[0] = 10000;
  Receiver rec(*CurrentDualSampa[0],0,NULL,
                                   ReceiverPerformanceTestHandler,
				   NULL);
  rec.Process();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  delete CurrentDualSampa[0];
}

BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToDualSampaThreadMode)
{
TestChrono chrono;

  CurrentDualSampa[0] = new DualSampa(7,11);
  CurrentDualSampa[0]->SetDataProvider(DspHandlerIdle);
  CurrentDualSampa[0]->SetInternalRef(0);
  regeneration_count[0] = 10000;
  Receiver rec(*CurrentDualSampa[0],0,NULL,
                                   ReceiverPerformanceTestHandler,
				   NULL);
  chrono.Start();
  rec.Start();
  rec.Join();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  delete CurrentDualSampa[0];
}
BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToDualSampaThreadMode100Instance)
{
TestChrono chrono;
  for (int i=0;i<100;i++) {
    CurrentDualSampa[i] = new DualSampa(7,11);
    CurrentDualSampa[i]->SetDataProvider(DspHandlerIdle);
    CurrentDualSampa[i]->SetInternalRef(i);
    regeneration_count[i] = 10000;
  }
  Receiver *rec[100];
  for (int i=0;i<100;i++) {
    rec[i] = new Receiver(*CurrentDualSampa[i],(void *)i,NULL,
                                   ReceiverPerformanceTestHandler,
				   NULL);
  }	
  chrono.Start();
  for (int i=0;i<100;i++) rec[i]->Start();
  for (int i=0;i<100;i++) rec[i]->Join();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  for (int i=0;i<100;i++) delete rec[i];
  for (int i=0;i<100;i++) delete CurrentDualSampa[i];

}


BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToFirstGbtPort)
{
GbtS gbts;
GbtR gbtr(gbts);
  TestChrono chrono;
  chrono.Start();
  CurrentDualSampa[0] = new DualSampa(7,11);
  CurrentDualSampa[0]->SetDataProvider(DspHandlerIdle);
  CurrentDualSampa[0]->SetInternalRef(0);
  gbts.PlugElink(0,CurrentDualSampa[0]);
  regeneration_count[0] = 10000;
  Receiver rec(0,gbtr,0,NULL,
                      ReceiverPerformanceTestHandler,
		      NULL);
  rec.Process();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  delete CurrentDualSampa[0];
}
BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToLastGbtPort)
{
GbtS gbts;
GbtR gbtr(gbts);
  TestChrono chrono;
  chrono.Start();
  CurrentDualSampa[0] = new DualSampa(7,11);
  CurrentDualSampa[0]->SetDataProvider(DspHandlerIdle);
  CurrentDualSampa[0]->SetInternalRef(0);
  gbts.PlugElink(39,CurrentDualSampa[0]);
  regeneration_count[0] = 10000;
  Receiver rec(39,gbtr,0,NULL,
                      ReceiverPerformanceTestHandler,
		      NULL);
  rec.Process();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  delete CurrentDualSampa[0];
}

BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToFirstGbtPort10Instances)
{
GbtS gbts;
GbtR gbtr(gbts);
  TestChrono chrono;
  chrono.Start();
  for (int i=0;i<10;i++) {
    CurrentDualSampa[i] = new DualSampa(7,11);
    CurrentDualSampa[i]->SetDataProvider(DspHandlerIdle);
    CurrentDualSampa[i]->SetInternalRef(i);
    gbts.PlugElink(i,CurrentDualSampa[i]);
    regeneration_count[i] = 10000;
  }
  Receiver *rec[10];
  for (int i=0;i<10;i++) {
    rec[i] = new Receiver(i,gbtr,(void *)i,NULL,
                                   ReceiverPerformanceTestHandler,
				   NULL);
  }	
  chrono.Start();
  for (int i=0;i<10;i++) rec[i]->Start();
  for (int i=0;i<10;i++) rec[i]->Join();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  for (int i=0;i<10;i++) delete rec[i];
  for (int i=0;i<10;i++) delete CurrentDualSampa[i];
}

BOOST_AUTO_TEST_CASE(ReceiverNoPacketPerfomanceTestConnectedToFirstGbtPort40Instances)
{
GbtS gbts;
GbtR gbtr(gbts);
  TestChrono chrono;
  chrono.Start();
  for (int i=0;i<40;i++) {
    CurrentDualSampa[i] = new DualSampa(7,11);
    CurrentDualSampa[i]->SetDataProvider(DspHandlerIdle);
    CurrentDualSampa[i]->SetInternalRef(i);
    gbts.PlugElink(i,CurrentDualSampa[i]);
    regeneration_count[i] = 10000;
  }
  Receiver *rec[40];
  for (int i=0;i<40;i++) {
    rec[i] = new Receiver(i,gbtr,(void *)i,NULL,
                                   ReceiverPerformanceTestHandler,
				   NULL);
  }	
  chrono.Start();
  for (int i=0;i<40;i++) rec[i]->Start();
  for (int i=0;i<40;i++) rec[i]->Join();
  chrono.Stop();
  int elapsed_seconds = chrono.ElapsedMs(); 
  std::cout << boost::unit_test::framework::current_test_case().p_name << " " << elapsed_seconds << std::endl;
  for (int i=0;i<40;i++) delete rec[i];
  for (int i=0;i<40;i++) delete CurrentDualSampa[i];
}
BOOST_AUTO_TEST_SUITE_END()

