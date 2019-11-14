#-*-cmake-*-
#
# Test for Exiv2 sources
#
# Once loaded this will define
#  EXIV2_FOUND        - system has Exiv2
#  EXIV2_INCLUDE_DIR  - include directory for Exiv2
#

SET(EXIV2_FOUND "NO")

FIND_PATH(EXIV2_INCLUDE_DIR exiv2/image.hpp
    "${EXIV2_LOCATION}"
    "${EXIV2_LOCATION}/include"
    /usr/include
    /usr/include/Exiv2
    /opt/local/include/Exiv2
)

IF(EXIV2_INCLUDE_DIR)
    SET(EXIV2_FOUND "YES")
    MESSAGE(STATUS "Exiv2 found at: ${EXIV2_INCLUDE_DIR}/exiv2")
ENDIF(EXIV2_INCLUDE_DIR)

#####

