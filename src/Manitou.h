#ifndef MANITOU
#define MANITOU
#include <cstdint>
#include <map>
#include "DualSampa.h"

using namespace std;
/*!
 *  \brief main ojbect manager
 *
 *  This class provides an handling interfaces for all objects externally created. \n
 *  Each accessible object is 'referenced' by an unique ID (provided at object creation and can only be accessed through this reference.\n
 *  This class is implmeneted as a singleton.
 */
class Manitou
{
static Manitou *Instance;

public:
  static Manitou *GetInstance();
  int CreateDualSampa(uint8_t addr1,uint8_t addr2);
  void DeleteDualSampa(int ref);
  DualSampa *GetDualSampa(int ref);
private:
  Manitou();
  static int           mDsMapId;
  map<int,DualSampa *> mDsMap;
  
};
#endif
