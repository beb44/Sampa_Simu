#ifndef MANITOU
#define MANITOU
#include <cstdint>
#include <map>
#include "dualsampa.h"

using namespace std;
/*!
 *  \brief main ojbect manager
 *
 *  This class provides an handling interfaces for all objects externally created. \n
 *  Each accessible object is 'referenced' by an unique ID (provided at object creation and can only be accessed through this reference.\n
 *  This class is implmeneted as a singleton.
 */
class manitou
{
static manitou *Instance;

public:
  static manitou *getInstance();
  int create_dualsampa(uint8_t addr1,uint8_t addr2);
  void delete_dualsampa(int ref);
  dualsampa *get_dualsampa(int ref);
private:
  manitou();
  static int           dsmapid;
  map<int,dualsampa *> dsmap;
  
};
#endif
