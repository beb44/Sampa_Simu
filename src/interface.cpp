/*!
 * \file interface;h
 * \brief provides C like interfaces
 */
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include "sampa.h"
#include "dualsampa.h"
#include "receiver.h"
#include "manitou.h"

/*!
 *  \function create_dualsampa
 *  \brief External method to create dualsampa object
 *
 *  \param ad1  hardware address of the first sampa chip
 *  \param ad2  hardware address of the second sampa chip
 *
 *  \return dualsampa object reference
 */
extern "C" int create_dualsampa(int ad1,int ad2)
{
  return manitou::getInstance()->create_dualsampa(ad1,ad2); 
}
/*!
 *  \function delete_dualsampa
 *  \brief External method to delete dualsampa object
 *
 *  \param id  dualsampa object reference
 *
 *  \return 0 if the deletion has been properly processed, -1 in case of error
 */
extern "C" int delete_dualsampa(int id)
{
  try {
    manitou::getInstance()->delete_dualsampa(id);
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
    manitou::getInstance()->get_dualsampa(id)->reset_frames();
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
    manitou::getInstance()->get_dualsampa(id)->add_data(sampaid,len,buffer);
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
    manitou::getInstance()->get_dualsampa(id)->send_frames();
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

extern "C" int set_user_handler(int id,void (*user_handler)(int))
{
  try {
    manitou::getInstance()->get_dualsampa(id)->set_data_provider(user_handler);
  }
  catch (const std::exception & e)
  {
    cerr << e.what() << endl;
    return -1;  // returns -1 in case of error
  }
  return 0;

}

