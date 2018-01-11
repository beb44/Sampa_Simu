#include <iostream>
#include "sampa_head.h"
using namespace std;

sampa_head::sampa_head()
{  
  intern_rawhead = 0;
}
sampa_head::sampa_head(uint64_t head)
{  
  intern_rawhead = head;
}
void sampa_head::decode()
{
  decode(intern_rawhead);
}
void sampa_head::decode(uint64_t head)
{
  fHammingCode             = (head>>44) & 0x3f; 
  fHeaderParity            = (head>>43) & 0x01; 
  fPkgType                 = (head>>40) & 0x07; 
  fNbOf10BitWords          = (head>>30) & 0x3ff; 
  fChipAddress             = (head>>26) & 0x0f; 
  fChannelAddress          = (head>>21) & 0x1f; 
  fBunchCrossingCounter    = (head>>1) & 0xfffff;
  fPayloadParity           = head & 1;
}
uint64_t sampa_head::build()
{
uint64_t res = 0;
 
   res = fHammingCode;
   res = (res <<1) + fHeaderParity;
   res = (res <<3) + fPkgType;
   res = (res <<10) + fNbOf10BitWords;
   res = (res <<4)+ fChipAddress;
   res = (res <<5) + fChannelAddress;
   res = (res <<20) + fBunchCrossingCounter;
   res = (res <<1) + fPayloadParity;
   return res;
  
}

uint64_t sampa_head::build_sync()
{

   fHammingCode          = 0x13;
   fHeaderParity         = 0;
   fPkgType              = 2;
   fNbOf10BitWords       = 0; 
   fChipAddress          = 0xf; 
   fChannelAddress       = 0 ,
   fBunchCrossingCounter = 0xAAAAA;
   fPayloadParity        = 0; 
   return build();
 }

void sampa_head::display()
{
  cout << "Header content " << endl;
  cout << "fHammingCode          " << std::hex<< fHammingCode <<endl;
  cout << "fHeaderParity         " << std::hex<< fHeaderParity <<endl;
  cout << "fPkgType              " << std::hex<< fPkgType <<endl;
  cout << "fNbOf10BitWords       " << std::hex<< fNbOf10BitWords <<endl;
  cout << "fChipAddress          " << std::hex<< fChipAddress <<endl;
  cout << "fChannelAddress       " << std::hex<< fChannelAddress <<endl;
  cout << "fBunchCrossingCounter " << std::hex<< fBunchCrossingCounter <<endl;
  cout << "fPayloadParity        " << std::hex<< fPayloadParity <<endl;
 
}
