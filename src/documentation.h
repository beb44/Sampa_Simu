/*! \mainpage Sampa packet encoder/decoder
 *
 * This package provides methods to generate DualSampa Elinks packet. \n
 * It is composed of :
 * - internal processing classes files (*.cpp *.h)
 * - interface extern"C" routines  (interface.cpp interface.h)
 * - makefile file (with several options)
 * - Doxygen configuration file
 * - A main.cpp file containing several usage exemples end test cases.
 * 
 * The regular usage consists in generating a library link which provides 
 * methods (see interface.h). 
 *
 * \section Make Cmake
 *
 * Cmake configurations are delivered and provide automatic makefile
 * generation facilities.\\
 * Three base libraries are generated 
 * - libsampabase.so
 * - libsampasender.so
 * - libsampareceiver.so
 * Morever, it also generate a regression test suite program
 * - sampaTest
 * and a basic date event file reader
 * - DateReader
 *
 * \section Inter Interface method usage
 *
 * Interface routine methods are "pure" C linkable (declared as extern "C")
 * which insure they can be used in C++ or C project and also
 * be safely called from non c/c++ languages (Python, Java,...).\n
 *
 * A method always returns an integer which can be interpreted as
 * - an object reference in case of creation (when strictly positive)
 * - an error code in case of incorrect usage or system problem
 *
 * interface methods never thows exception as 'normal' error reporting 
 * mechanism.
 *
 * \subsection sb1 object creation method
 *
 * Objects (DualSampa, Reciever, ...) are created through creation methods.
 * Creation methods will return either -1 indicating an error as occured
 * (most certainly due to incorrect parameters) or the object reference.
 *
 * \code
 * int dsid = create_dualsampa(101,222);
 * \endcode
 * A reference uniquely identifies the object and is used in object access
 * methods (generally as the first parameter) and deletion method. Using an incorrect reference
 * in an access method ands up in an error.
 *
 * \subsection sb2 object usage methods
 *
 * Once an object exists (has been created and has a 'reference'), a set 
 * of operations are offered to the user.
 * These operation are performed through method (C routine) which 
 * generic signature are
 *
 * \code
 * int  methode_name ( reference, parameters...);
 * \endcode
 * 
 * Here is an usage exemple
 * \code
 * reset_frames(dsid);
 * delete_dualsampa(dsid); 
 * \endcode
 *
 * \subsection sb3 CallBack mechanism
 *
 * User can also provide CallBack method in order to recieve notification
 * on special event.
 * For instance, the reciever object is able to notify incoming packet
 * event and present this event in a undecoded format
 *
 * \code
 * void rec_handler(int ref,int addr,int ch,int nbsamp,int ts,int len, short *buff)
 * {
 *   cout << "Packet recieved "<< std::dec <<ts << " " << len << endl;
 * }
 *
 * int main()
 * {
 * int recid = reciever();
 *
 *    set_reciever_packethandler(rec_handler);
 * }
 * \endcode
 *   
 * Each time the receiver object completes the reception and decoding
 * of an incoming, it will 'callback' the function 'rec_handler' declared
 * and implemented by user. \n
 * Providing callback function is most of the time not mandatory
 * The fisrt parameter of the callback function which allows to mutualize
 * the same routine and share it with several objects.
 *
 * \code
 * int recid1 = reciever();  // receiver on elink 0
 * int recid2 = reciever();  // receiver on elink 1
 *
 * void rec_handler(int ref,int addr,int ch,int nbsamp,int ts,int len, short *buff)
 * {
 *   if (id == recid1)
 *   {
 *     // specific handling for elink0
 *     ...
 *   }    
  *   if (id == recid2)
 *   {
 *     // specific handling for elink1
 *     ...
 *   }    
 * }
 *
 * int main()
 * {
 * int recid1 = reciever();
 *
 *    set_reciever_packethandler(rec_handler);
 * }
 * \endcode 
 * 
 */
