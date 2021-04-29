set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/standdata
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/hitproducer
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    StandHitsProducer.cxx
    StandSiliconCluster.cxx
    StandClustersContainer.cxx
)

set(HEADERS
    StandHitsProducer.h
    StandSiliconCluster.h
    StandClustersContainer.h
)

Set(LINKDEF StandHitsProducerLinkDef.h)
Set(LIBRARY_NAME StandHitsProducer)
Set(DEPENDENCIES BmnData StandData StandCommon)

GENERATE_LIBRARY()
