#PROJECT(hls_dl)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)


ADD_EXECUTABLE(hls_dl   ${DIR_SRCS})

FIND_PATH(myHeader zapp.h)
IF(myHeader)
MESSAGE(STATUS “header path:”${myHeader})
INCLUDE_DIRECTORIES(${myHeader})
ENDIF(myHeader)

FIND_LIBRARY(myLib tinyxml)
IF(myLib)
MESSAGE(STATUS “lib path:”${myLib})
ENDIF(myLib)

#TARGET_LINK_LIBRARIES(indexparse  /home/ovtsvn/linux_devlibs/bin/x86_64/debug/libtinyxml.so )
INCLUDE_DIRECTORIES(/home/ovtsvn/linux_devlibs/include/)

#LINK_DIRECTORIES(/home/ovtsvn/linux_devlibs/bin/x86_64/debug/)

#link_directories(/home/ovtsvn/linux_devlibs/bin/x86_64/debug/)

#TARGET_LINK_LIBRARIES(ott_live /home/ovtsvn/linux_devlibs/bin/x86_64/debug/libzapp.so)

TARGET_LINK_LIBRARIES(hls_dl curl)
TARGET_LINK_LIBRARIES(hls_dl pthread)



#add_definitions (-DZMOD_BUILD_DATE="\\"$(shell date)\\"" -DZMOD_SVNVER="\\"$(shell svnversion . | sed "s/.*://")\\"")

#add_definitions (-DZMOD_BUILD_DATE="\"$(shell date)\"" -DZMOD_SVNVER="\"$(shell svnversion . | sed "s/.*://")\"")

#TARGET_LINK_LIBRARIES(rtr  zcomm man disreceive zapp)
