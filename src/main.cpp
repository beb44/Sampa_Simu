#include <iostream>
#include <cstdint>
#include "sampa.h"
#include "dualsampa.h"
#include "receiver.h"
#include "manitou.h"
#include "interface.h"

using namespace std;

#define  NBT 200

sampa sampa_0(0);
dualsampa ds(10,12);
receiver rec(&ds);
int dsid =0;
int loop = 50000;//00000;
dualsampa *dsarr[NBT];
receiver  *recarr[NBT];
int looparr[NBT];

void dsp_handlerarr(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 

  if (--looparr[ref] == 0)return;
  for (int i = 0;i<32;i++)
  {
    dsarr[ref]->select_channel(0,i);
    dsarr[ref]->reset_frames();
    dsarr[ref]->add_data(0,4,data);
    dsarr[ref]->send_frames();
  }
  dsarr[ref]->regenerate_data();
}
void dsp_handler(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 
  // stops when max number of loops reached
  if (--loop == 0) return;
#if 0
  for (int i = 0;i<32;i++)
  {
    ds.select_channel(0,i);
    ds.reset_frames();
    ds.add_data(0,4,data);
    ds.send_frames();
  }
#endif    
  ds.regenerate_data();
  return;  
  //cout << "DSP handler "<< std::dec <<ref  << endl; 
  ds.reset_frames();
  ds.add_data(0,4,data);
  ds.add_data(0,4,data);
  ds.send_frames();
  ds.make_sync(0);
  ds.send_frames();
  ds.reset_frames();
  ds.select_channel(0,0);  // channel 0
  ds.add_data(0,4,data);
  ds.add_data(0,4,data);
  ds.send_frames();
  ds.regenerate_data();
 }

void rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff)
{
  //cout << "Packet recieved "<< std::dec <<ch << " " << buff[0] << endl; 
  if (len != 2)
  {
    throw "horreur"; 
  }
}
int main()
{

#if 0
  ds.set_user_handler(dsp_handler);
  rec.set_userhandler(rec_handler);
  rec.start();
  rec.join();
#endif
  for (int i=0; i< NBT;i++)
  {
    looparr[i]= 10000;
    dsarr[i] = new dualsampa(i*2,i*2+1);
    dsarr[i]->set_internal_ref(i);
    dsarr[i]->set_user_handler(dsp_handlerarr);
    recarr[i] = new receiver(dsarr[i]);
    recarr[i]->set_userhandler(rec_handler);
  }
  for (int i=0; i< NBT;i++) recarr[i]->start();
  for (int i=0; i< NBT;i++) recarr[i]->join();
  
  cout <<"hello  "<<endl;
 // rec.process();
 for (int i=0;i<1;i++)
 {
   dsid = create_dualsampa(101,222);
   cout << dsid << endl;
   delete_dualsampa(dsid);
 }
}
