#include <iostream>
#include <cstdint>
#include <chrono>
#include <ctime>
#include "sampa.h"
#include "dualsampa.h"
#include "receiver.h"
#include "GBT_s.h"
#include "GBT_r.h"
#include "manitou.h"
#include "interface.h"
#include "Test.h"
using namespace std;

#define  NBT 100

sampa sampa_0(0);
dualsampa ds(10,12);
GBT_s    gbt_s1;
GBT_r    gbt_r1(&gbt_s1);
receiver rec(0,&gbt_r1);
dualsampa ds1(22,24);
receiver rec1(1,&gbt_r1);
int dsid =0;
int loop = 4;//00000;

void dsp_handler(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 
  // stops when max number of loops reached
  if (loop-- == 0) return;
#if 0
  for (int i = 0;i<32;i++)
  {
    ds.select_channel(0,i);
    ds.reset_frames();
    data[2]=i+loop*32;
    ds.add_data(0,4,data);
    ds.send_frames();
  }
#endif    
  ds.regenerate_data();
  return;  
  //cout << "DSP handler "<< std::dec <<ref  << endl; 
 }
void dsp_handler1(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 
  // stops when max number of loops reached
  if (loop-- == 0) return;
#if 0
  for (int i = 0;i<32;i++)
  {
    ds1.select_channel(0,i);
    ds1.reset_frames();
    data[2]=i;
    ds1.add_data(0,4,data);
    ds1.send_frames();
  }
#endif    
  ds1.regenerate_data();
  return;  
  //cout << "DSP handler "<< std::dec <<ref  << endl; 
 }

void rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff)
{
  cout << "Packet recieved "<< std::dec <<addr << " " << buff[0] << endl; 
  if (len != 2)
  {
    throw "horreur"; 
  }
}
void rec_handler1(int addr,int ch,int nbsamp,int ts,int len, short *buff)
{
  cout << "Packet recieved ++++"<< std::dec <<addr << " " << buff[0] << endl; 
  if (len != 2)
  {
    throw "horreur"; 
  }
}
int main()
{
  try {
  ds.set_user_handler(dsp_handler);
  rec.set_userhandler(rec_handler);
  gbt_s1.plug_elink(0,&ds);
  rec.start();
  ds1.set_user_handler(dsp_handler1);
  rec1.set_userhandler(rec_handler1);
  gbt_s1.plug_elink(1,&ds1);
  rec1.start();
  rec.join();
  rec1.join();
  }
  catch (const std::exception& e)
  {
    cout << "execption catched" << endl;
    throw;
  }
}
