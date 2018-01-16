#ifndef TEST
#define TEST
#include "dualsampa.h"
#include "receiver.h"
#include <cstdint>
#include <chrono>
#include <ctime>
#include <map>

class Test: public dualsampa_handler,receiver_handler
{
public:
  Test(int nbthread,int nbloop);
  void start();
  void dsp_handler(int ref);
  void rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff);
  void display();
//private:
  std::map<int,dualsampa *> dsarr;
  std::map<int,receiver  *> recarr;
  std::map<int,int>         looparr;
  std::chrono::time_point<std::chrono::system_clock> cstart; 
  std::chrono::time_point<std::chrono::system_clock> laststarted;
  std::chrono::time_point<std::chrono::system_clock> cend;
  int  _nbthread ;
  int  _nbloop  ;
  char *title;
};
class Test1: public Test
{
public:
  Test1(int nbthread,int nbloop);
  void dsp_handler(int ref);
  
  int  maxdiff;
};
#endif
