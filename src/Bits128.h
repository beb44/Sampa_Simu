#ifndef BITS128
#define BITS128
#include <cstdint>
/*!
 * \brief 128 bits word emulation class
 */
 

class Bits128
{
public:
  Bits128();
  Bits128(uint32_t m3,uint32_t m2,uint32_t m1,uint32_t m0)
  {
    w[0]= m0;
    w[1]= m1;
    w[2]= m2;
    w[3]= m3;
  };
  /*!
   * \brief return bit value at pos
   *
   * \param pos bit position 
   *
   * \return bit value
   */  
  uint8_t Get(int pos)
  {
    return  ((w[pos/32] & mask[pos %32])?1:0);	     
  };
  /*!
   * \brief set bit value at pos
   *
   * \param pos bit position 
   * \param val value of the bit
   *
   */
  void Set(int pos,uint8_t val)
  {
    int P1= pos>>5;
    int P2= pos & 0x1f;

    w[P1] &=  maskcomp[P2];
    if (val)
    {
      w[P1] |= mask[P2];
    }
    //w[P1] |=  (val<<P2); // set value

  };
  
private: 
   static const  uint32_t  maskcomp[32];
   static const  uint32_t  mask[32];
  /*! \brief word value split in 4 32 bit maord                      */
  uint32_t  w[4];
};

 #endif
