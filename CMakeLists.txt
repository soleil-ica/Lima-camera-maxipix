# CMakeLists.txt for Maxipix

SET(maxipix_srcs src/PriamSerial.cpp  src/PriamAcq.cpp
	 src/PixelArray.cpp src/MaxipixReconstruction.cpp
	 src/MaxipixCamera.cpp src/MaxipixInterface.cpp
	 src/MpxDetConfig.cpp src/MpxCommon.cpp src/MpxChipConfig.cpp src/MpxDacs
	 tools/src/INIReader.cpp tools/ini.c)

FILE(STRINGS "VERSION" maxipix_vers)
ADD_LIBRARY(limamaxipix SHARED ${maxipix_srcs})

TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tools/src")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/cmake ${CMAKE_MODULE_PATH})
include(FindEspiaDrv)

#set paths for espia driver and limaespialib and include files
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${ESPIA_DRV_INCLUDE_DIR}")
TARGET_INCLUDE_DIRECTORIES(limamaxipix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/include")

LINK_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../common/espia")

TARGET_LINK_LIBRARIES(limamaxipix limacore)
TARGET_LINK_LIBRARIES(limamaxipix ${ESPIA_DRV_LIB})
TARGET_LINK_LIBRARIES(limamaxipix limaespia)

SET_TARGET_PROPERTIES(limamaxipix PROPERTIES VERSION ${maxipix_vers} SOVERSION "1")

INSTALL(TARGETS limamaxipix LIBRARY DESTINATION lib)

IF(COMPILE_SIP)
    SET(NAME "maxipix")
    SET(INCLUDES)
    FILE(GLOB sipfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/sip" "${CMAKE_CURRENT_SOURCE_DIR}/sip/*.sip")
    FOREACH(sipfile ${sipfiles})
        SET(INCLUDES 
"${INCLUDES} 
%Include ${sipfile}"
)
    ENDFOREACH()

    SET(IMPORTS
"%Import limacore.sip
%Import limaespia.sip"
)

    IF(SIP_VERSION_STR VERSION_LESS "4.12")
    	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules_before_4_12.sip.in sip/limamaxipix.sip)
    ELSE()
    	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sip/limamodules.sip.in sip/limamaxipix.sip)
    ENDIF()

    SET(SIP_CONCAT_PARTS 1)
    SET(SIP_INCLUDES ${SIP_INCLUDES}
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip"
        "${CMAKE_BINARY_DIR}/sip/core"
        "${CMAKE_SOURCE_DIR}/third-party/Processlib/tasks/sip"
        "${CMAKE_SOURCE_DIR}/common/sip"
        "${CMAKE_SOURCE_DIR}/hardware/sip"
        "${CMAKE_SOURCE_DIR}/control/sip"
        "${CMAKE_SOURCE_DIR}/control/software_operation/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/sip"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common/espia/sip"	
	"${CMAKE_BINARY_DIR}/camera/common/espia/sip")
    set(SIP_DISABLE_FEATURES WITH_CONFIG)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/sip/lima${NAME}.sip PROPERTIES OBJECT_DEPENDS ${sipfiles})
    set(SIP_CHECK_EXC ON)
    ADD_SIP_PYTHON_MODULE(limamaxipix ${CMAKE_CURRENT_BINARY_DIR}/sip/limamaxipix.sip)
    TARGET_INCLUDE_DIRECTORIES(python_module_limamaxipix PRIVATE
                               ${PYTHON_INCLUDE_DIRS}
                               "${CMAKE_SOURCE_DIR}/sip"
                               "${CMAKE_SOURCE_DIR}/sip/core"
			       "${CMAKE_SOURCE_DIR}/third-party/Processlib/sip")
    TARGET_LINK_LIBRARIES(python_module_limamaxipix limamaxipix)

    INSTALL(FILES python/__init__.py DESTINATION "${PYTHON_SITE_PACKAGES_DIR}/Lima/Maxipix")
ENDIF()
