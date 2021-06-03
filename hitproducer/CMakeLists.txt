set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/standdata
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/hitproducer
${CMAKE_SOURCE_DIR}/hitproducer/hitmakers
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

    hitmakers/AbstractHitMaker.cxx
    hitmakers/SiliconHitMaker.cxx
    hitmakers/GemHitMaker.cxx
)

set(HEADERS
    StandHitsProducer.h
    StandSiliconCluster.h
    StandClustersContainer.h

    hitmakers/AbstractHitMaker.h
    hitmakers/SiliconHitMaker.h
    hitmakers/GemHitMaker.h
)

Set(LINKDEF StandHitsProducerLinkDef.h)
Set(LIBRARY_NAME StandHitsProducer)
Set(DEPENDENCIES BmnData StandData StandCommon)

GENERATE_LIBRARY()
