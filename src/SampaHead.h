#ifndef SAMPA_HEAD
#define SAMPA_HEAD
#include <cstdint>
/*!
 *  \brief Sampa header parsing and building tool
 *
 */
#define   HAMMINGCODE_OFFSET  0
#define   HEADERPARITY_OFFSET 6
#define   PKGTYPE_OFFSET      7
#define   NBWORD_OFFSET       10
#define   CHAPPADDR_OFFSET    20
#define   CHANNEL_OFFSET      24
#define   BUNCHCROSS_OFFSET   29
#define   PARITY_OFFSET       49

#define   HAMMINGCODE_MASK    0x000000000003F 
#define   HEADERPARITY_MASK   0x0000000000040
#define   PKGTYPE_MASK        0x0000000000380
#define   NBWORD_MASK         0x00000000FFC00
#define   CHAPPADDR_MASK      0x0000000F00000
#define   CHANNEL_MASK        0x000001F000000
#define   BUNCHCROSS_MASK     0x1FFFFE0000000
#define   PARITY_MASK         0x2000000000000
class SampaHead
{
public:
    SampaHead();
    SampaHead(uint64_t head);
    uint64_t    mFHammingCode;         // 6 bit long 
    uint64_t    mFHeaderParity;        // 1 bit long
    uint64_t    mFPkgType;             // 3 bit long
    uint64_t    mFNbOf10BitWords;      // 10 bit long 
    uint64_t    mFChipAddress;         // 4 bit long 
    uint64_t    mFChannelAddress;      // 5 bit long ,
    uint64_t    mFBunchCrossingCounter;// 20 bit long 
    uint64_t    mFPayloadParity;       // 1 bit long 
    
    uint64_t    Build();
    void        Decode();
    void        Decode(uint64_t head);
    void        Display();
    
    uint64_t    BuildSync();
    
    uint8_t     GetPacketType();
    uint8_t     GetPacketType(uint64_t head);
    uint16_t    GetNbWords();
    uint16_t    GetNbWords(uint64_t head);
private:
    uint64_t    InternRawHead;
};
#endif
