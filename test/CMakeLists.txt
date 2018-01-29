set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

add_executable(sampaTest 
               ReceiverTest.cxx 
	       DualSampaTest.cxx 
	       GbtRTest.cxx
	       GbtSTest.cxx)
target_link_libraries(sampaTest sampasender sampareceiver )
