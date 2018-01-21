#ifndef SAMPAELINK
#define SAMPAELINK
#include "Elink.h"
#include <cstdint>
/*!
 * \brief 'Real" Elink interface for Sampa and DualSampa chips
 */

class SampaElink : public Elink
{
public:
  virtual ~SampaElink() {}
};
#endif
