#include <iostream>
#include "DateParser.h"
#include "Receiver.h"
#include "GbtR.h"
#include <map>
#include <cstdint>
#include <tgmath.h>
using namespace std;

class channelrecord
{
public:
  channelrecord():sum(0),sum2(0),nb(0) {}
  float   sum;
  float   sum2;
  int   nb;
};

class sampa
{
public:
   sampa(): exists (false) {}
   channelrecord  rec[32];
   bool exists;
};
sampa    S1[100];

void rec_handler(void *ui,int addr,int ch,int nbsamp,int ts, short *buff)
{
  S1[addr].exists = true;
  S1[addr].rec[ch].nb+= nbsamp;
  for (int i=0;i<nbsamp;i++)
  {
    S1[addr].rec[ch].sum += buff[i];
    S1[addr].rec[ch].sum2 += (buff[i]*buff[i]);
  }
  float sum =0;
  float sum2 =0;
  for (int i=0;i<nbsamp;i++) sum+=buff[i];
  for (int i=0;i<nbsamp;i++) sum2+=(buff[i]*buff[i]);
  
  float ect = (sum2/nbsamp)-((sum/nbsamp)*(sum/nbsamp));
 
  cout << "Sampa " << addr << " channel " << ch << " nb sample "<<  nbsamp << " moy " <<sum/nbsamp << " ecart " << ect<< endl;
}
void display()
{
  for (int a=0;a<10;a++)
  {
    if (S1[a].exists == false) continue;
    for (int ch =0; ch<32; ch++)
    {
      if (S1[a].rec[ch].nb== 0) continue;
      cout << "Sampa " << a << " channel " << ch << " nb sample "<<  S1[a].rec[ch].nb << " moy " << S1[a].rec[ch].sum/S1[a].rec[ch].nb << endl;
    }
  } 
}
void run(char *filename)
{
DateParser dr(filename);
GbtR       gbt_r1(dr);
Receiver   rec(0,gbt_r1,NULL,NULL,rec_handler,NULL);
Receiver   rec1(1,gbt_r1,NULL,NULL,rec_handler,NULL);
//Receiver rec2(2,gbt_r1,NULL,NULL,rec_handler,NULL);

   rec.Start(); 
   rec1.Start(); 
   //rec2.Start(); 
   rec.Join(); 
   rec1.Join();
   //rec2.Join();
   //gbt_r1.Start(); 
   dr.Display();
   display();
}
void usage()
{
}
int main(int argc, char *argv[])
{
char *filename = "../data/input_file";
int i=0;
  while (i<argc)
  {
    if (strcmp(argv[i],"-f")==0) {
      i++;
      if (i<argc) {
        filename = argv[i];
      }
      else {
        usage();
	exit(0);
      } 
      
    }
    i++;
  }
  run(filename);
}
