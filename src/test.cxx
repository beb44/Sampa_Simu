#include "test.h"
#include <iostream>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;
void RecHandler1(void * ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
 ((Test *)ui)->RecHandler(addr,ch,nbsamp,ts, buff);
}


Test::Test(int nbthread,int nbloop)
{
  dsarr.clear();
  recarr.clear(); 
  _nbthread = nbthread; 
  _nbloop  = nbloop;
  title    = "idle frame only";
}

void Test::start()
{ 
  for (int i=0; i< _nbthread;i++)
  {
    looparr[i]= _nbloop;
    dsarr[i]= new DualSampa(i*2,i*2+1);
    dsarr[i]->SetInternalRef(i);
    dsarr[i]->SetDataProvider(static_cast<DualSampaHandler&>( *this));
    recarr[i] = new Receiver(*dsarr[i],(void *)this,NULL,RecHandler1,NULL);
    //recarr[i]->SetUserHandler(this);
  }
  //recarr[0]->display_stats();

  //std::chrono::time_point<std::chrono::system_clock> start, end;
  cstart = std::chrono::system_clock::now();
  laststarted= std::chrono::system_clock::now();
  for (int i=0; i< _nbthread;i++) recarr[i]->Start();
  for (int i=0; i< _nbthread;i++)  if (recarr[i]->Joinable()) recarr[i]->Join();
  cend = std::chrono::system_clock::now();
}        

void Test::DspHandler(int ref)
{
  if (looparr[ref] == _nbloop) 
  {
    laststarted= std::chrono::system_clock::now();
  }
  if (looparr[ref]-- == 0)return;

  dsarr[ref]->RegenerateData();
}
void Test::RecHandler(int addr,int ch,int nbsamp,int ts, short *buff)
{
}

void Test::display()
{
  int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
                             (cend-cstart).count();  
  cout << "Test condition : " << title<< " "<< _nbthread << " " << _nbloop;			     
  std::cout <<  " elapsed time: " << elapsed_seconds << " ms "; 
  std::cout <<  " mean time per thread: "<< elapsed_seconds/_nbthread <<endl;
  elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
                             (laststarted-cstart).count(); 
//  std::cout <<  "last started thread after: " << elapsed_seconds << " ms" << endl;;
			     
}


Test1::Test1(int nbthread,int nbloop) :Test(nbthread,nbloop)
{
  title    = "short frames on all channels ";
  maxdiff =0;
}
void Test1::dsp_handler(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 


  if (looparr[ref] == _nbloop) laststarted= std::chrono::system_clock::now();

#if 0
  int min = 100000, max =0;
  for (std::map<int,int>::iterator it=looparr.begin() ; it!=looparr.end() ; ++it)
  {
    if (it->second <min) min = it->second;
    if (it->second >max) max = it->second;
  }
  if ((max-min)>maxdiff) maxdiff = (max-min);
#endif
  if (looparr[ref]-- == 0)return;
  for (int i = 0;i<32;i++)
  {
    dsarr[ref]->SelectChannel(0,i);
    dsarr[ref]->ResetFrames();
    dsarr[ref]->AddData(0,4,data);
    dsarr[ref]->SendFrames();
  }

  dsarr[ref]->RegenerateData();
  //std::this_thread::yield();
} 
