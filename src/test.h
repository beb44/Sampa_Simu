#ifndef TEST
#define TEST
#include "DualSampa.h"
#include "Receiver.h"
#include <cstdint>
#include <chrono>
#include <ctime>
#include <map>

class Test: public DualSampaHandler//,ReceiverHandler
{
public:
  Test(int nbthread,int nbloop);
  void start();
  void DspHandler(int ref);
  void RecHandler(int addr,int ch,int nbsamp,int ts, short *buff);
  void display();
//private:
  std::map<int,DualSampa *>   dsarr;
  std::map<int,Receiver  *> recarr;
  std::map<int,int>         looparr;
  std::chrono::time_point<std::chrono::system_clock> cstart; 
  std::chrono::time_point<std::chrono::system_clock> laststarted;
  std::chrono::time_point<std::chrono::system_clock> cend;
  int  _nbthread ;
  int  _nbloop  ;
  std::string  title;
};
class Test1: public Test
{
public:
  Test1(int nbthread,int nbloop);
  void dsp_handler(int ref);
  
  int  maxdiff;
};
#endif
