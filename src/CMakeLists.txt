cmake_policy(SET CMP0057 NEW)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_library(sampabase SHARED
        Bits128.h
        Bits128.cxx
        GbtLink.h
        Elink.h
        SampaHead.h
        SampaHead.cxx
        )

add_library(sampasender SHARED
        DualSampa.cxx
        DualSampa.h
        GbtS.cxx
        GbtS.h
        Manitou.cxx
        Manitou.h
        Sampa.cxx
        Sampa.h
	SampaElink.h
        )
target_link_libraries(sampasender PUBLIC sampabase)

add_library(sampareceiver SHARED
        Elink.h
        GbtLink.h
	GbtElink.cxx
	GbtElink.h
        GbtR.cxx
        GbtR.h
        Receiver.cxx
        Receiver.h
	GbtRInterface.h
        )
target_link_libraries(sampareceiver PUBLIC sampabase Threads::Threads)

add_executable(DateReader DateReader.cxx DateParser.cxx)
target_link_libraries(DateReader sampareceiver)

add_executable(sampaMain main.cxx test.cxx)
target_link_libraries(sampaMain sampasender sampareceiver)

# fake target to get IDEs to take those files into account even if they
# are not part of any other target
add_custom_target(sampa_ide SOURCES documentation.h interface.cxx interface.h test.h Doxyfile)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.8)
  find_package(Doxygen REQUIRED dot)
  if (DOXYGEN_FOUND)
    doxygen_add_docs(doc)
  endif ()
endif ()

