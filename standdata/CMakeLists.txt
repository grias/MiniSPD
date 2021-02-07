set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/standdata
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    StandSiliconHit.cxx
)

set(HEADERS
    StandSiliconHit.h
)


Set(LINKDEF StandDataLinkDef.h)
Set(LIBRARY_NAME StandData)

GENERATE_LIBRARY()
