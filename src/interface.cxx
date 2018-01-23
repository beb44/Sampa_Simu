/*!
 * \file interface.h
 * \brief provides C like interfaces
 */
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include "Sampa.h"
#include "DualSampa.h"
#include "Receiver.h"
#include "Manitou.h"

/*!
 *  \brief External method to create dualsampa object
 *
 *  \param ad1  hardware address of the first sampa chip
 *  \param ad2  hardware address of the second sampa chip
 *
 *  \return dualsampa object reference
 */
extern "C" int create_dualsampa(int ad1,int ad2)
{
  return Manitou::GetInstance()->CreateDualSampa(ad1,ad2); 
}
/*!
 *  \brief External method to delete dualsampa object
 *
 *  \param id  dualsampa object reference
 *
 *  \return 0 if the deletion has been properly processed, -1 in case of error
 */
extern "C" int delete_dualsampa(int id)
{
  try {
    Manitou::GetInstance()->DeleteDualSampa(id);
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;
}

extern "C" int reset_frames(int id)
{
  try {
    Manitou::GetInstance()->GetDualSampa(id)->ResetFrames();
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

extern "C" int add_data(int id,int sampaid,int len,unsigned short *buffer)
{
  try {
    Manitou::GetInstance()->GetDualSampa(id)->AddData(sampaid,len,buffer);
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

extern "C" int send_frames(int id)
{
  try {
    Manitou::GetInstance()->GetDualSampa(id)->SendFrames();
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

extern "C" int set_data_provider(int id,void (*user_handler)(int))
{
  try {
    Manitou::GetInstance()->GetDualSampa(id)->SetDataProvider(user_handler);
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

