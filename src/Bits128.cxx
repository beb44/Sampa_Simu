#include "Bits128.h"
/*!
 * \brief O 128 bit constructor
 */
 const uint32_t Bits128::maskcomp[32] = 
           { 0xFFFFFFFE,0xFFFFFFFD,0xFFFFFFFB,0xFFFFFFF7,
             0xFFFFFFEF,0xFFFFFFDF,0xFFFFFFBF,0xFFFFFF7F, 
             0xFFFFFEFF,0xFFFFFDFF,0xFFFFFBFF,0xFFFFF7FF, 
             0xFFFFEFFF,0xFFFFDFFF,0xFFFFBFFF,0xFFFF7FFF, 
             0xFFFEFFFF,0xFFFDFFFF,0xFFFBFFFF,0xFFF7FFFF, 
             0xFFEFFFFF,0xFFDFFFFF,0xFFBFFFFF,0xFF7FFFFF, 
             0xFEFFFFFF,0xFDFFFFFF,0xFBFFFFFF,0xF7FFFFFF, 
             0xEFFFFFFF,0xDFFFFFFF,0xBFFFFFFF,0x7FFFFFFF};
 const uint32_t Bits128::mask[32] = 
           { 0x00000001,0x00000002,0x00000004,0x00000008,
             0x00000010,0x00000020,0x00000040,0x00000080, 
             0x00000100,0x00000200,0x00000400,0x00000800, 
             0x00001000,0x00002000,0x00004000,0x00008000, 
             0x00010000,0x00020000,0x00040000,0x00080000, 
             0x00100000,0x00200000,0x00400000,0x00800000, 
             0x01000000,0x02000000,0x04000000,0x08000000, 
             0x10000000,0x20000000,0x40000000,0x80000000}; 
	     

Bits128::Bits128()
{
  Bits128(0,0,0,0);
}
/*!
 * \brief 128 bit constructor with initial value
 *
 * \param m3 Most significant 32 bits
 * \param m2 next significant 32 bits
 * \param m1 next significant 32 bits
 * \param m0 less significant 32 bits
 *
 */
 #if 0
Bits128::Bits128(uint32_t m3,uint32_t m2,uint32_t m1,uint32_t m0)
{
    w[0]= m0;
    w[1]= m1;
    w[2]= m2;
    w[3]= m3;
}
#endif
/*!
 * \brief return bit value at pos
 *
 * \param pos bit position 
 *
 * \return bit value    
 */  
