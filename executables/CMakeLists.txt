set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/bmndata
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/common
${CMAKE_SOURCE_DIR}/decoder
${CMAKE_SOURCE_DIR}/decoder/mappers
${CMAKE_SOURCE_DIR}/hitproducer
${CMAKE_SOURCE_DIR}/hitproducer/hitmakers
${CMAKE_SOURCE_DIR}/standdata
${CMAKE_SOURCE_DIR}/tracksproducer
${CMAKE_SOURCE_DIR}/mcdigitizer
${CMAKE_SOURCE_DIR}/mcdigitizer/detectorDataDigitizers
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR})

link_directories( ${LINK_DIRECTORIES})

add_executable(rawToDigits rawToDigits.cxx)
target_link_libraries(rawToDigits LINK_PUBLIC Core Decoder)

add_executable(digitsToHits digitsToHits.cxx)
target_link_libraries(digitsToHits LINK_PUBLIC Core StandHitsProducer)

add_executable(hitsToTracks hitsToTracks.cxx)
target_link_libraries(hitsToTracks LINK_PUBLIC Core StandTracksProducer)

add_executable(testMcDigitizer testMcDigitizer.cxx)
target_link_libraries(testMcDigitizer LINK_PUBLIC Core McDigitizer)
