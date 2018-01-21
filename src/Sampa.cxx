#include <iostream>
#include <string.h>
#include <cstdint>
#include <bitset>
#include "SampaHead.h"
#include "Sampa.h"
using namespace std; 

 /*!
 *  \brief Constructeur
 *
 *  \param addr : hardware address of the sampa chip
 */
Sampa::Sampa(uint16_t addr) : mHaddr(addr),
                              mChannel(0),
			      mWPointer(0),
			      mHasHeader(false),
			      mDataAvailable(false),
			      mCurSendFrame(NULL),
			      mCurWord(0),
			      mCurBit(0),
			      mCurLen(0)
{
  ResetFrame();  // make room for header
//  mChannel = 0;    // channel 0 is default
//  mDataAvailable = false;        // no serial data available;
//  mCurSendFrame = (uint16_t *)0; // no current frame being sent
}

Sampa::~Sampa()
{
  cout <<"Sampa " << (int)mHaddr << " deleted " << endl; 
}

void Sampa::SelectChannel(const uint8_t chid)
{
  mChannel = chid;
}

/*!
 *  \brief starts a new frame
 *
 *  internal buffer is reset and ready to receives new data samples.\n
 *  the frame header will be automatically build when frame will be 'sent out'.\n
 *  If the previous frame has not been sent, it will be lost
 *  
 */
void Sampa::ResetFrame()
{
  mWPointer = 5;  // make room for header
  mHasHeader = false;
}

/*!
 *  \brief Add a data chunk to current frame
 *
 *  \param len  length of the data chunk
 *  \param data adresse of the buffer 
 */
void Sampa::AddData(int const len, uint16_t const *data)
{
  // append data to the crrent frame
  memcpy(mFrame+mWPointer,data,sizeof(data)*len);
  mWPointer += len;
}

void Sampa::MakeSync()
{
SampaHead header;
uint64_t   res;


   res= header.BuildSync();
   mFrame[0]= (res >> 0  ) & 0x3ff;
   mFrame[1]= (res >> 10 ) & 0x3ff;
   mFrame[2]= (res >> 20 ) & 0x3ff;
   mFrame[3]= (res >> 30 ) & 0x3ff;
   mFrame[4]= (res >> 40 ) & 0x3ff;
   mHasHeader =  true;
}

void Sampa::MakeDataHeader()
{
SampaHead header;
uint64_t   res;

   header.mFHammingCode          = 0x3f;
   header.mFHeaderParity         = 0;
   header.mFPkgType              = 4;
   header.mFNbOf10BitWords       = mWPointer-5; 
   header.mFChipAddress          = mHaddr; 
   header.mFChannelAddress       = mChannel ,
   header.mFBunchCrossingCounter = 0xBAE2;
   header.mFPayloadParity        = 0; 
   res= header.Build();
   mFrame[0]= (res >> 0 ) & 0x3ff;
   mFrame[1]= (res >> 10 ) & 0x3ff;
   mFrame[2]= (res >> 20 ) & 0x3ff;
   mFrame[3]= (res >> 30 ) & 0x3ff;
   mFrame[4]= (res >> 40  ) & 0x3ff;
   mHasHeader =  true;
}


/*!
 *  \brief make the current frame available for serial link
 *
 *  The current frame is 'cloned' and linked in the ready list of serial link.\n
 *  If the frame is empty and no header is present, the frame will not been sent.\n
 *  If header has not been built yet, it will be constructed and preponed to the frame.\n
 *
 *  The frame will not been 'reset' and can ben reused.
 *  
 */
void Sampa::SendFrame()
{

uint16_t  *_fp;

  // if no data and no header build, just ignore
  if ((mWPointer == 5) && (mHasHeader == false))
    return;
    
  // if the buffer is not yet built, make it
  if (mHasHeader == false)
    MakeDataHeader();
    
  // 
  // clone the current buffer and link it to the sending list
  //
  _fp= (uint16_t  *) malloc(sizeof(uint16_t)*mWPointer);
  memcpy(_fp,mFrame,sizeof(uint16_t)*mWPointer); 
  mSendList.push_back(_fp);
} 

/*!
 * \brief Display the frame being hold in the internal buffer in binary format
 *
 * Debugging tool for displaying the internal buffer contents. It automatically generates the header if needed.
 */
void Sampa::DispFrame()
{

uint16_t  *_fp = mFrame;

  // if no data and no header build, just ignore
  if ((mWPointer == 5) && (mHasHeader == false))
    return;
  if (mHasHeader == false)
    MakeDataHeader();
  cout << "Header "<< endl;
  for (int i=0;i<5;i++)
    cout << std::bitset<10> (*(_fp++) )<< endl;
  if (mWPointer != 5) {
    cout << "Payload " << endl;
    for (int i=5;i<mWPointer;i++)
      cout << std::bitset<10> (*(_fp++) ) << endl;   
  }
}
/*!
 * \brief indicates wether or not some data is available for serial link
 *
 * \return True is fome data is available, false otherwise
 *
 */
bool Sampa::SerialAvailable()
{
  if (mSendList.size() != 0)
    mDataAvailable = true;
  return mDataAvailable;
}

uint8_t Sampa::GetSerial()
{
uint8_t  res;

  if (mCurSendFrame == (uint16_t *)0) {
    // no frame being sent for now, check for waiting ones
    if (mSendList.size() == 0) 
      return 0xff; 
    // set internal variable
    mCurSendFrame = mSendList.front();
    mSendList.pop_front();
    mCurWord = 0;
    mCurBit  = 0;
    mCurLen  = mCurSendFrame[1]+5; // get size from the header
  }
  // get the data
  res =  ((mCurSendFrame[mCurWord] >> mCurBit) & 1);
  // increment pointer
  mCurBit++;
  if (mCurBit==10) {
    // all bits in current word have been sent out
    mCurWord ++;
    mCurBit = 0;
    if (mCurWord == mCurLen) {
      // all words have been sent out
      mDataAvailable = false;
      // free memory 
      mCurWord = 0;
      mCurLen  = 0;
      mCurBit  = 0; 
      free(mCurSendFrame);
      mCurSendFrame = (uint16_t *)0;
    } 
  }
  return res;
}


