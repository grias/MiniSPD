set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/hitproducer
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}

) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    StandHitsProducer.cxx
    StandIOManager.cxx
    StandSiliconGeoMapper.cxx
    StandSiliconCluster.cxx
    StandClustersContainer.cxx
    StandSiliconHit.cxx
)

set(HEADERS
    StandHitsProducer.h
    StandIOManager.h
    StandSiliconGeoMapper.h
    StandSiliconCluster.h
    StandClustersContainer.h
    StandSiliconHit.h
)


Set(LINKDEF StandHitsProducerLinkDef.h)
Set(LIBRARY_NAME StandHitsProducer)
Set(DEPENDENCIES Core Base BmnData)

GENERATE_LIBRARY()
