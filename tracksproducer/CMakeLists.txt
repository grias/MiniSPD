set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/standdata
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/tracksproducer
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    StandTracksProducer.cxx
)

set(HEADERS
    StandTracksProducer.h
)

Set(LINKDEF StandTracksProducerLinkDef.h)
Set(LIBRARY_NAME StandTracksProducer)
Set(DEPENDENCIES StandData StandCommon Minuit)

GENERATE_LIBRARY()
