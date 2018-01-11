#ifndef SAMPA_HEAD
#define SAMPA_HEAD
#include <cstdint>
/*!
 *  \brief Sampa header parsing and building tool
 *
 */
class sampa_head
{
public:
    sampa_head();
    sampa_head(uint64_t head);
    uint32_t    fHammingCode;         // 6 bit long 
    uint32_t    fHeaderParity;        // 1 bit long
    uint32_t    fPkgType;             // 3 bit long
    uint32_t    fNbOf10BitWords;      // 10 bit long 
    uint32_t    fChipAddress;         // 4 bit long 
    uint32_t    fChannelAddress;      // 5 bit long ,
    uint32_t    fBunchCrossingCounter;// 20 bit long 
    uint32_t    fPayloadParity;       // 1 bit long 
    
    uint64_t    build();
    void        decode();
    void        decode(uint64_t head);
    void        display();
    
    uint64_t    build_sync();
private:
    uint64_t    intern_rawhead;
};
#endif
