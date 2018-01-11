#include <iostream>
#include <cstdint>
#include "sampa.h"
#include "dualsampa.h"
#include "reciever.h"
#include "manitou.h"
#include "interface.h"

using namespace std;

sampa sampa_0(0);
dualsampa ds(10,12);
reciever rec(&ds);
int dsid =0;
int loop = 5;

void dsp_handler(int ref)
{
uint16_t data[4] = {7,2,0x1,0x20}; 
  // stops when max number of loops reached
  if (--loop == 0) return;
  cout << "DSP handler "<< std::dec <<ref  << endl; 
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
  rec.process();
 }

void rec_handler(int addr,int ch,int nbsamp,int ts,int len, short *buff)
{
  cout << "Packet recieved "<< std::dec <<ts << " " << len << endl; 
}
int main()
{
uint16_t data[4] = {7,2,0x1,0x20}; 


  ds.set_user_handler(dsp_handler);
  rec.set_userhandler(rec_handler);
  cout <<"hello  "<<endl;
#if 0  
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
  ds.reset_frames();
  ds.select_channel(0,5);  // channel 5
  ds.add_data(0,4,data);
  ds.add_data(1,4,data);
  ds.send_frames();
#endif
 // rec.process();
 for (int i=0;i<1;i++)
 {
   dsid = create_dualsampa(101,222);
   cout << dsid << endl;
   delete_dualsampa(dsid);
 }
}
