set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/TMillepede
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    TMillepede.cxx
)

set(HEADERS
    TMillepede.h
)

Set(LINKDEF TMillepedeLinkDef.h)
Set(LIBRARY_NAME TMillepede)
# Set(DEPENDENCIES )

GENERATE_LIBRARY()
