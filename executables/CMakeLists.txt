set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/hitproducer
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR})

link_directories( ${LINK_DIRECTORIES})

add_executable(digitsToHits digitsToHits.cxx)

target_link_libraries(digitsToHits LINK_PUBLIC Core StandHitsProducer)
