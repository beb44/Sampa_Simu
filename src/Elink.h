#ifndef ELINK
#define ELINK
#include <cstdint> 

  /*! \brief Elink emulation interface class              */
 

class Elink
{
public:
  virtual ~Elink() {}
  /*!
   *  \brief return the availability of data on the serial link
   *
   *  \return True is the data is available, false otherwise
   */
  virtual bool SerialAvailable()=0; 
  /*!
   *  \brief return next bit on Elink stream
   *
   *  The reciever must first check the data is available otherwise
   *  the returned data may be not be consistant
   *
   *  \return next bit
   */
  virtual uint8_t GetSerial()=0;
};
#endif
