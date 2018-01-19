
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
        )
target_link_libraries(sampasender PUBLIC sampabase)

add_library(sampareceiver SHARED
        Elink.h
        GbtLink.h
        GbtR.cxx
        GbtR.h
        Receiver.cxx
        Receiver.h
        )
target_link_libraries(sampareceiver PUBLIC sampabase)

add_executable(sampaTest main.cxx)
target_link_libraries(sampaTest sampasender sampareceiver)

# fake target to get IDEs to take those files into account even if they
# are not part of any other target
add_custom_target(sampa_ide SOURCES documentation.h interface.cxx interface.h test.h Doxyfile)

