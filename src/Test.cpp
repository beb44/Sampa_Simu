#include "Test.h"
#include <iostream>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;
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
    dsarr[i] = new dualsampa(i*2,i*2+1);
    dsarr[i]->set_internal_ref(i);
    dsarr[i]->set_user_handler(this);
    recarr[i] = new receiver(dsarr[i]);
    recarr[i]->set_userhandler(this);
  }
 // recarr[0]->display_stats();

  //std::chrono::time_point<std::chrono::system_clock> start, end;
  cstart = std::chrono::system_clock::now();
  laststarted= std::chrono::system_clock::now();
  for (int i=0; i< _nbthread;i++) recarr[i]->start();
  for (int i=0; i< _nbthread;i++)  if (recarr[i]->joinable()) recarr[i]->join();
  cend = std::chrono::system_clock::now();
}        

void Test::dsp_handler(int ref)
{
  if (looparr[ref] == _nbloop) 
  {
    cout << "---"<< endl;
    laststarted= std::chrono::system_clock::now();
  }
  if (looparr[ref]-- == 0)return;

  dsarr[ref]->regenerate_data();
}
void Test::rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff)
{
}

void Test::display()
{
  int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
                             (cend-cstart).count();  
  cout << "Test condition : " << title<< " "<< _nbthread << " " << _nbloop << endl;			     
  std::cout <<  "elapsed time: " << elapsed_seconds << " ms" << endl;
  elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>
                             (laststarted-cstart).count(); 
  std::cout <<  "last started thread after: " << elapsed_seconds << " ms" << endl;;
			     
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
    dsarr[ref]->select_channel(0,i);
    dsarr[ref]->reset_frames();
    dsarr[ref]->add_data(0,4,data);
    dsarr[ref]->send_frames();
  }

  dsarr[ref]->regenerate_data();
  //std::this_thread::yield();
} 
