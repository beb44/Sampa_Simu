#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <bitset>
#include "eventD7-106.h"
#include "GbtLink.h"
#include "Bits128.h"
#include <string>
#include <mutex>
#include <thread>
class DateParser : public gbtlink
{
public:
  DateParser(std::string filename);
  
  bool          GbtWordAvailable(); 
  Bits128   	GetWord();
  void          Start();
  void          Display();

private:
  int           mNbParse;
  bool          ParseOneEvent();
  bool          DecodePhysicsEvent(uint32_t *databuf);
  void          ThreadRead();
  int           MyFread(uint32_t *buffer, int size);
  void          Swap(uint32_t *buffer, int size);
  int           ReadData (uint32_t *buffer, int size);
  std::string   mFileName;
  FILE          *mFpIn;

  
  static const  int hsize  = sizeof(struct eventHeaderStruct)/sizeof(uint32_t); 
  static const  int sc     = sizeof(uint32_t);
  bool              mSwapNeeded;

  int               mNbEvent;
  int               mNbPhysicsEvent;
  
  uint32_t          *m_curft1;
  uint32_t          *m_curft;
  int	            m_ldcwords;
  int               m_ldcsize;
  
  uint32_t          *m_buffer;
  int               m_size;
  int               m_res;
  std::mutex        mutget;
  std::mutex        mutgot;
  std::thread       *myThread;

};
