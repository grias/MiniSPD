# Create a library called "libBmnData" which includes the source files given in
# the array.

set(INCLUDE_DIRECTORIES
 ${BASE_INCLUDE_DIRECTORIES}
 ${CMAKE_SOURCE_DIR}/bmndata
 ${CMAKE_SOURCE_DIR}/bmndata/digits
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${Boost_INCLUDE_DIRS}
 ${ROOT_INCLUDE_DIR}
)

include_directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
 ${ROOT_LIBRARY_DIR}
 ${FAIRROOT_LIBRARY_DIR}
)

link_directories(${LINK_DIRECTORIES})

# List of source files
set(SRCS
BmnEventHeader.cxx
BmnRunHeader.cxx
BmnEnums.h
DigiArrays.h
db_structures.h
BmnTrigInfo.cxx
BmnMath.cxx
digits/BmnSiliconDigit.cxx
digits/BmnGemStripDigit.cxx
digits/BmnTrigDigit.cxx
digits/BmnTrigWaveDigit.cxx
digits/BmnDchDigit.cxx
digits/BmnECALDigit.cxx
digits/BmnADCDigit.cxx
digits/BmnTQDCADCDigit.cxx
digits/BmnTDCDigit.cxx
digits/BmnSyncDigit.cxx
)

Set(HEADERS)

Set(LINKDEF BmnDataLinkDef.h)
Set(LIBRARY_NAME BmnData)
Set(DEPENDENCIES Core Base)

GENERATE_LIBRARY()
