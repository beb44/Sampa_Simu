#include <iostream>
#include <stdexcept>
#include "manitou.h"

using namespace std;

manitou* manitou::Instance = 0;
int      manitou::dsmapid  = 0;

manitou* manitou::getInstance()
{
  if (Instance == 0) Instance = new manitou();
  return Instance;
}

manitou::manitou()
{
  cout <<"Manitou created" << endl; 
  dsmap.clear();
}
/*!
 *  \brief Creates dualsampa object and assign a reference
 *
 *  \param ad1  hardware address of the first sampa chip
 *  \param ad2  hardware address of the second sampa chip
 *
 *  \return dualsampa object reference
 */
int manitou::create_dualsampa(uint8_t addr1,uint8_t addr2)
{
  dsmapid++;
  dsmap[dsmapid] = new dualsampa(addr1,addr2);
  dsmap[dsmapid]->set_internal_ref(dsmapid);
  return dsmapid;
}
/*!
 *  \brief Creates dualsampa object and assign a reference
 *
 *  Ckeck the existence of the dualsampa object and delete if it exists.
 *
 *  \param id  dualsampa object reference
 *
 * \throws std::expection
 *
 */
void manitou::delete_dualsampa(int ref)
{
  if (dsmap.find(ref) ==dsmap.end()) throw std::logic_error("unkown reference");
  delete dsmap[ref];
  dsmap.erase(ref);
}

dualsampa *manitou::get_dualsampa(int ref)
{
  if (dsmap.find(ref) ==dsmap.end()) throw std::logic_error("unkown reference");
  return dsmap[dsmapid];
}

