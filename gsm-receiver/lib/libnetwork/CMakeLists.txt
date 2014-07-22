########### next target ###############

SET(libnetwork_STAT_SRCS
   tcpclientdata.cpp
)

add_library(libnetwork STATIC ${libnetwork_STAT_SRCS})
