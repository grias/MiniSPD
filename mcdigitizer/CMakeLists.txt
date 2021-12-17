set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/mcdigitizer
${CMAKE_SOURCE_DIR}/mcdigitizer/detectorDataDigitizers
${CMAKE_SOURCE_DIR}/bmndata/digits
${CMAKE_SOURCE_DIR}/common
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
) 

link_directories( ${LINK_DIRECTORIES})

set(SRCS
    McDigitizationManager.cxx
    McHit.cxx
    detectorDataDigitizers/AbstractMcDataDigitizer.cxx
    detectorDataDigitizers/SiliconMcDataDigitizer.cxx

)

set(HEADERS
    McDigitizationManager.h
    McHit.h
    detectorDataDigitizers/AbstractMcDataDigitizer.h
    detectorDataDigitizers/SiliconMcDataDigitizer.h
)

Set(LINKDEF McDigitizerLinkDef.h)
Set(LIBRARY_NAME McDigitizer)
Set(DEPENDENCIES BmnData StandCommon)

GENERATE_LIBRARY()
