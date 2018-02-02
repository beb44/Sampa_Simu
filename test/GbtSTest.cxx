#include <iostream>
#include <stdlib.h>  
#include <time.h>       
#include <cstdint>
#include <chrono>
#include <ctime>
#include <thread>
#include <array>
#include <list>
#define BOOST_TEST_INCLUDED
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp> 
#include <boost/test/framework.hpp>

#include "../src/GbtS.h"

class GbtS_checker : public Elink
{
public:
  GbtS_checker(int nbbit):mNbBit(nbbit){}
  bool SerialAvailable()
  {
    return (mNbBit != 0);
  }
  uint8_t GetSerial()
  {
    BOOST_ASSERT(mNbBit !=0);
    mNbBit--;
    return 1;
  }
private:
  int    mNbBit;

};

BOOST_AUTO_TEST_SUITE(gbts_test)

BOOST_AUTO_TEST_CASE(GbtSCreation)
{
GbtS gbts;
  BOOST_ASSERT((true));   
}

BOOST_AUTO_TEST_CASE(GbtSNoProvider)
{
GbtS gbts;
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
}

BOOST_AUTO_TEST_CASE(GbtSOneDumbProvider)
{
GbtS gbts;
GbtS_checker gbts1(0);

  gbts.PlugElink(0,&gbts1);
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
}

BOOST_AUTO_TEST_CASE(GbtSOneProvider)
{
GbtS gbts;
GbtS_checker gbts1(2000);

  gbts.PlugElink(0,&gbts1);
  for (int i=0;i<1000;i++) {  
    BOOST_ASSERT(gbts.GbtWordAvailable()==true); 
    gbts.GetWord();
  }  
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
}

BOOST_AUTO_TEST_CASE(GbtSTwoConsecutiveProviders)
{
GbtS gbts;
GbtS_checker gbts1(2000);
GbtS_checker gbts2(2000);

  gbts.PlugElink(0,&gbts1);
  gbts.PlugElink(1,&gbts2);
  for (int i=0;i<1000;i++) {  
    BOOST_ASSERT(gbts.GbtWordAvailable()==true); 
    gbts.GetWord();
  }  
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
}

BOOST_AUTO_TEST_CASE(GbtSTwoNonConsecutiveProviders)
{
GbtS gbts;
GbtS_checker gbts1(2000);
GbtS_checker gbts2(2000);
Bits128 word;
Bits128 testword(0,0,3,3);

  gbts.PlugElink(0,&gbts1);
  gbts.PlugElink(16,&gbts2);
  for (int i=0;i<1000;i++) {  
    BOOST_ASSERT(gbts.GbtWordAvailable()==true); 
    word = gbts.GetWord();
    BOOST_ASSERT ((word.Get(0 ) == 1) && (word.Get(1 ) == 1));
    BOOST_ASSERT ((word.Get(32) == 1) && (word.Get(33) == 1));
  }  
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
}

BOOST_AUTO_TEST_CASE(GbtS40DumbProviders)
{
GbtS gbts;
GbtS_checker *gbtscheck[40];
Bits128 word;
Bits128 testword(0,0,3,3);

  for (int i=0;i<40;i++)
  {
    gbtscheck[i] = new GbtS_checker(0);
    gbts.PlugElink(1,gbtscheck[i]);
  }
  BOOST_ASSERT(gbts.GbtWordAvailable()==false); 
  for (int i=0;i<40;i++) delete  gbtscheck[i]; 
}

BOOST_AUTO_TEST_CASE(GbtS40Providers)
{
GbtS gbts;
GbtS_checker *gbtscheck[40];
Bits128 word;
Bits128 testword(0,0,3,3);

  for (int i=0;i<40;i++)
  {
    gbtscheck[i] = new GbtS_checker(4000);
    gbts.PlugElink(i,gbtscheck[i]);
  }
  for (int i=0;i<2000;i++) {  
    BOOST_ASSERT(gbts.GbtWordAvailable()==true); 
    word = gbts.GetWord();
    if (rand()%50 == 0) {
      for (int j=0;j<80;j++) 
        BOOST_ASSERT (word.Get(j) == 1);
    }
  }  
  BOOST_ASSERT(gbts.GbtWordAvailable()==false);   
  for (int i=0;i<40;i++) delete  gbtscheck[i]; 
}

BOOST_AUTO_TEST_SUITE_END()
