#include <iostream>
#include <stdexcept>
#include "Manitou.h"

using namespace std;

Manitou* Manitou::Instance = 0;
int      Manitou::mDsMapId  = 0;

Manitou* Manitou::GetInstance()
{
  if (Instance == 0) Instance = new Manitou();
  return Instance;
}

Manitou::Manitou()
{
  cout <<"Manitou created" << endl; 
  mDsMap.clear();
}
/*!
 *  \brief Creates dualsampa object and assign a reference
 *
 *  \param addr1  hardware address of the first sampa chip
 *  \param addr2  hardware address of the second sampa chip
 *
 *  \return dualsampa object reference
 */
int Manitou::CreateDualSampa(uint8_t addr1,uint8_t addr2)
{
  mDsMapId++;
  mDsMap[mDsMapId] = new DualSampa(addr1,addr2);
  mDsMap[mDsMapId]->SetInternalRef(mDsMapId);
  return mDsMapId;
}
/*!
 *  \brief Creates dualsampa object and assign a reference
 *
 *  Ckeck the existence of the dualsampa object and delete if it exists.
 *
 *  \param ref  dualsampa object reference
 *
 * \throws std::expection
 *
 */
void Manitou::DeleteDualSampa(int ref)
{
  if (mDsMap.find(ref) ==mDsMap.end()) throw std::runtime_error("unkown reference");
  delete mDsMap[ref];
  mDsMap.erase(ref);
}

DualSampa *Manitou::GetDualSampa(int ref)
{
  if (mDsMap.find(ref) ==mDsMap.end()) throw std::logic_error("unkown reference");
  return mDsMap[ref];
}

