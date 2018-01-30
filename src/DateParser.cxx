#include "DateParser.h"
using namespace std;

DateParser::DateParser(std::string filename):mFileName(filename)
{
  mNbPhysicsEvent =0;
  mNbEvent         =0;
  mNbParse          =0;
  m_curft           =NULL;
  m_curft1          =NULL;

  if (!(mFpIn = fopen (filename.c_str(),"r")))
  {
    cout<<"ERROR opening file "<<filename<<endl;
    throw -1;
  }
  mutget.lock();
  mutgot.lock();
  try {
  myThread = new std::thread(&DateParser::ThreadRead,this);
  }
  catch (const std::exception& e) {
    cout << "execption catched" << endl;
    throw;
  }

}

void DateParser::ThreadRead()
{
  while (1) {
    mutget.lock();
    m_res = fread(m_buffer,sizeof(uint32_t),m_size,mFpIn);
    mutgot.unlock();
  }
}

int DateParser::MyFread(uint32_t *buffer, int size)
{
   m_buffer = buffer;
   m_size   = size;
   mutget.unlock();
   mutgot.lock();
   return m_res;
} 

bool DateParser::GbtWordAvailable()
{
  try {
    if (m_curft==NULL)
    {
      while (ParseOneEvent() == false) ParseOneEvent();
    }
    //cout << m_ldcwords << "<" << m_ldcsize << endl;
    if (m_ldcwords < (m_ldcsize-4) ) return true;
    while (ParseOneEvent() == false) ParseOneEvent();
    return true;
  } catch (const std::exception& e) {
    return false;
  }
}

Bits128 DateParser::GetWord()
{
Bits128 res(0,0,0,0);;
#if 0
  for (int i=0;i<32;i++) 
  {
    res[i]    = ((*(m_curft+3) >> i)) & 1; 
    res[32+i] = ((*(m_curft+2) >> i)) & 1;
    res[64+i] = ((*(m_curft+1) >> i)) & 1;
    res[96+i] = ((*m_curft >> i)) & 1;
  }
#endif
  //cout << std::hex << *m_curft << " " << *(m_curft+4) << " " << *(m_curft+8)  << " " << *(m_curft+12)  <<endl;
  res = Bits128(*(m_curft+0),*(m_curft+1),*(m_curft+2),*(m_curft+3));
  m_ldcwords +=4; m_curft+=4;
  return res;
}


void DateParser::Swap(uint32_t *buffer, int size)
{
uint32_t temp;
  
  for (int i=0;i<size;i++)
  {
    temp = (*buffer>>24)&0xff+(((*buffer>>16)&0xff)<<8)+
           (((*buffer>>8)&0xff)<<16)+(((*buffer)&0xff)<<24);
  }
}

int DateParser::ReadData (uint32_t *buffer, int size)
{
  int res = fread(buffer,sizeof(uint32_t),size,mFpIn);
//   int res = MyFread(buffer,size);
  if (res == 0) return 0;
  if (mSwapNeeded) Swap(buffer,size);
  return res;
}

bool DateParser::ParseOneEvent()
{
int res;
struct eventHeaderStruct dateheader;

  mNbParse++;
  res = fread(&dateheader,sizeof(uint32_t),hsize,mFpIn);
//  res = MyFread((uint32_t *)&dateheader,hsize);
  if (res==0)
  {
    cout << "Unexpected end of file " << endl;
    throw std::exception();
  }
  //cout << std::hex << dateheader.eventMagic << std::dec << endl;
  if ((dateheader.eventMagic != EVENT_MAGIC_NUMBER) &&
       (dateheader.eventMagic != EVENT_MAGIC_NUMBER_SWAPPED))
  {
    throw "incorrect header ";
  }
  
  if (dateheader.eventMagic == EVENT_MAGIC_NUMBER)
  {
    // no Swap need
    mSwapNeeded = false;
  }
  else
  {
    //  Swap need
    Swap((uint32_t *)&dateheader,hsize);
    mSwapNeeded = true;
  }
  //cout << "dateheader.eventSize " << dateheader.eventSize << endl; 
  //
  uint32_t* databuf = new uint32_t[dateheader.eventSize/sc];
  //
  memcpy(databuf,&dateheader,sizeof(struct eventHeaderStruct));
  res = ReadData(&databuf[hsize],(dateheader.eventSize/sc-hsize));
  if (m_curft1 != NULL) delete[] m_curft1;
  m_curft = databuf;
  m_curft1 = databuf;
  struct eventHeaderStruct* fDateHeaderPtr = (struct eventHeaderStruct*) m_curft;
  
  mNbEvent++;
  if (fDateHeaderPtr->eventType == physicsEvent)
  {
    //cout << "DecodePhysicsEvent " << mNbPhysicsEvent<< endl;
    DecodePhysicsEvent(m_curft);
    mNbPhysicsEvent++;
    return true;
  }
  return false;
}


bool DateParser::DecodePhysicsEvent(uint32_t *databuf)
{
uint64_t curhead = 0;
uint64_t hhead = 0x132003c155554; //132003c155554

  m_curft =databuf;
  struct eventHeaderStruct* fDateHeaderPtr = (struct eventHeaderStruct*) databuf;
  if (fDateHeaderPtr->eventGdcId != VOID_ID)
  {
    // skip GDC header
    cout << "skip GDC header" << endl;
    m_curft+= (fDateHeaderPtr->eventHeadSize)/sc ;
  }

  int eventsize  = (fDateHeaderPtr->eventSize)/4;      
  int ldchsize = (fDateHeaderPtr->eventHeadSize)/4 ;
  m_curft+= ldchsize;
  int ldcwords = ldchsize;   // number of word to be handled
  int eventwords = ldchsize; // skip header;
  while (eventwords<eventsize)
  {
    (struct  eventHeaderStruct*)fDateHeaderPtr;
    fDateHeaderPtr = (struct eventHeaderStruct*) m_curft;
    m_ldcsize  = (fDateHeaderPtr->eventSize)/4;
    int ldchsize = (fDateHeaderPtr->eventHeadSize)/4;

    // skip header 
    m_curft+= ldchsize;
    m_ldcwords= ldchsize;
    uint64_t header = 0;
    struct equipmentHeaderStruct* fDateEquipmentHeaderPtr;
    fDateEquipmentHeaderPtr = (struct equipmentHeaderStruct*) m_curft;
    //cout << fDateEquipmentHeaderPtr->equipmentSize << endl;
    m_curft   += sizeof(struct equipmentHeaderStruct)/4;  // NO SIZE IS INDICATED IN THE HEADER
    m_ldcwords+= sizeof(struct equipmentHeaderStruct)/4;  // IS THAT CORRECT ?
    //cout << "   " << fDateEquipmentHeaderPtr->equipmentSize << endl;
    // skip Start O fpacket (4 words)
    //cout << "Start of packet "<< *curft << endl;
    m_curft += 4; m_ldcwords+= 4;
    eventwords += m_ldcsize; 
    //cout << eventwords << " " << eventsize << endl;
  }
  return true;
}  
void DateParser::Display()
{
   cout << mNbParse << " " << mNbEvent << " " << mNbPhysicsEvent << endl;
}
