set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/common
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    StandIOManager.cxx
    StandSiliconGeoMapper.cxx
    StandReverseSiliconGeoMapper.cxx
    StandGemGeoMapper.cxx
    StandStrawGeoMapper.cxx
    Hister.cxx
)

set(HEADERS
    StandIOManager.h
    StandSiliconGeoMapper.h
    StandReverseSiliconGeoMapper.h
    StandGemGeoMapper.h
    StandStrawGeoMapper.h
    Hister.h
)

Set(LINKDEF StandCommonLinkDef.h)
Set(LIBRARY_NAME StandCommon)
Set(DEPENDENCIES Physics Gpad Hist RIO Tree)

GENERATE_LIBRARY()
