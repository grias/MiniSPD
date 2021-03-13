# Create a library called "libDecoder" which includes the source files given in
# the array.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/digits
 ${CMAKE_SOURCE_DIR}/decoder
 ${CMAKE_SOURCE_DIR}/decoder/mappers
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR} 
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
)
 
link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
 BmnRawDataDecoder.cxx

 mappers/BmnAdcProcessor.cxx
 mappers/BmnDchRaw2Digit.cxx
 mappers/BmnECALRaw2Digit.cxx
 mappers/BmnGemRaw2Digit.cxx
 mappers/BmnSiliconRaw2Digit.cxx
 mappers/BmnTrigRaw2Digit.cxx
)

Set(HEADERS)

Set(LINKDEF DecoderLinkDef.h)
Set(LIBRARY_NAME Decoder)
Set(DEPENDENCIES Hist Tree BmnData Gpad)

GENERATE_LIBRARY()
