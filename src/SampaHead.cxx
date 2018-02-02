#include <iostream>
#include "SampaHead.h"
using namespace std;

SampaHead::SampaHead() : InternRawHead(0)
{  
//  intern_rawhead = 0;
}
SampaHead::SampaHead(uint64_t head): InternRawHead(head)
{  
//  intern_rawhead = head;
}
void SampaHead::Decode() 
{
  Decode(InternRawHead);
}
void SampaHead::Decode(uint64_t head)
{
  mFHammingCode             = ((head & HAMMINGCODE_MASK ) >>HAMMINGCODE_OFFSET); 
  mFHeaderParity            = ((head & HEADERPARITY_MASK)>>HEADERPARITY_OFFSET); 
  mFPkgType                 = ((head & PKGTYPE_MASK     )>>PKGTYPE_OFFSET     ); 
  mFNbOf10BitWords          = ((head & NBWORD_MASK      )>>NBWORD_OFFSET      ); 
  mFChipAddress             = ((head & CHAPPADDR_MASK   )>>CHAPPADDR_OFFSET   ); 
  mFChannelAddress          = ((head & CHANNEL_MASK     )>>CHANNEL_OFFSET     ); 
  mFBunchCrossingCounter    = ((head & BUNCHCROSS_OFFSET)>>BUNCHCROSS_OFFSET  );
  mFPayloadParity           = ((head & PARITY_MASK      )>> PARITY_OFFSET     );
}
uint64_t SampaHead::Build()
{
uint64_t res = 0;
 
   res = (mFHammingCode          << HAMMINGCODE_OFFSET )+
         (mFHeaderParity         << HEADERPARITY_OFFSET)+
         (mFPkgType              << PKGTYPE_OFFSET     )+
         (mFNbOf10BitWords       << NBWORD_OFFSET      )+
         (mFChipAddress          << CHAPPADDR_OFFSET   )+
         (mFChannelAddress       << CHANNEL_OFFSET     )+
         (mFBunchCrossingCounter << BUNCHCROSS_OFFSET  )+
         (mFPayloadParity        << PARITY_OFFSET      );
   return res;
  
}

uint64_t SampaHead::BuildSync()
{

   mFHammingCode          = 0x13;
   mFHeaderParity         = 0;
   mFPkgType              = 2;
   mFNbOf10BitWords       = 0; 
   mFChipAddress          = 0xf; 
   mFChannelAddress       = 0 ,
   mFBunchCrossingCounter = 0xAAAAA;
   mFPayloadParity        = 0; 
   return Build();
 }
uint8_t SampaHead::GetPacketType()
{
  return ((InternRawHead & PKGTYPE_MASK) >> PKGTYPE_OFFSET);
}
uint8_t SampaHead::GetPacketType(uint64_t head)
{
  return ((head & PKGTYPE_MASK) >> PKGTYPE_OFFSET);
}
uint16_t SampaHead::GetNbWords()
{
  return ((InternRawHead & NBWORD_MASK) >> NBWORD_OFFSET);
}
uint16_t SampaHead::GetNbWords(uint64_t head)
{
  return ((head & NBWORD_MASK) >> NBWORD_OFFSET);
}

void SampaHead::Display()
{
  cout << "Header content " << endl;
  cout << "fHammingCode          " << std::hex<< mFHammingCode <<endl;
  cout << "fHeaderParity         " << std::hex<< mFHeaderParity <<endl;
  cout << "fPkgType              " << std::hex<< mFPkgType <<endl;
  cout << "fNbOf10BitWords       " << std::hex<< mFNbOf10BitWords <<endl;
  cout << "fChipAddress          " << std::hex<< mFChipAddress <<endl;
  cout << "fChannelAddress       " << std::hex<< mFChannelAddress <<endl;
  cout << "fBunchCrossingCounter " << std::hex<< mFBunchCrossingCounter <<endl;
  cout << "fPayloadParity        " << std::hex<< mFPayloadParity <<endl;
 
}
