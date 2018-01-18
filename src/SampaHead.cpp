#include <iostream>
#include "SampaHead.h"
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
  fHammingCode             = ((head & HAMMINGCODE_MASK ) >>HAMMINGCODE_OFFSET); 
  fHeaderParity            = ((head & HEADERPARITY_MASK)>>HEADERPARITY_OFFSET); 
  fPkgType                 = ((head & PKGTYPE_MASK     )>>PKGTYPE_OFFSET     ); 
  fNbOf10BitWords          = ((head & NBWORD_MASK      )>>NBWORD_OFFSET      ); 
  fChipAddress             = ((head & CHAPPADDR_MASK   )>>CHAPPADDR_OFFSET   ); 
  fChannelAddress          = ((head & CHANNEL_MASK     )>>CHANNEL_OFFSET     ); 
  fBunchCrossingCounter    = ((head & BUNCHCROSS_OFFSET)>>BUNCHCROSS_OFFSET  );
  fPayloadParity           = ((head & PARITY_MASK      )>> PARITY_OFFSET     );
}
uint64_t sampa_head::build()
{
uint64_t res = 0;
 
   res = (fHammingCode          << HAMMINGCODE_OFFSET )+
         (fHeaderParity         << HEADERPARITY_OFFSET)+
         (fPkgType              << PKGTYPE_OFFSET     )+
         (fNbOf10BitWords       << NBWORD_OFFSET      )+
         (fChipAddress          << CHAPPADDR_OFFSET   )+
         (fChannelAddress       << CHANNEL_OFFSET     )+
         (fBunchCrossingCounter << BUNCHCROSS_OFFSET  )+
         (fPayloadParity        << PARITY_OFFSET      );
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

uint8_t sampa_head::get_packet_type(uint64_t head)
{
  return ((head & PKGTYPE_MASK) >> PKGTYPE_OFFSET);
}

uint16_t sampa_head::get_nbwords(uint64_t head)
{
  return ((head & NBWORD_MASK) >> NBWORD_OFFSET);
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
