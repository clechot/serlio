###
### common helpers for both codec and client targets
###

include(FetchContent)

### IDE related
if (CMAKE_GENERATOR MATCHES "Visual Studio.+")
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif ()


### environment

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(SRL_WINDOWS 1)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SRL_LINUX 1)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(SRL_MACOS 1)
endif ()


### look for the PRT libraries

# if prt_DIR is not provided, download PRT from its github home
if (NOT prt_DIR)
	if (SRL_WINDOWS)
		set(PRT_OS "win10")
		set(PRT_TC "vc141")
	elseif (SRL_LINUX)
		set(PRT_OS "rhel7")
		set(PRT_TC "gcc63")
	elseif (SRL_MACOS)
		set(PRT_OS "osx12")
		set(PRT_TC "ac81")
	endif ()

	set(PRT_VERSION "2.1.5705")
	set(PRT_CLS "${PRT_OS}-${PRT_TC}-x86_64-rel-opt")
	set(PRT_URL "https://github.com/esri/esri-cityengine-sdk/releases/download/${PRT_VERSION}/esri_ce_sdk-${PRT_VERSION}-${PRT_CLS}.zip")

	FetchContent_Declare(prt URL ${PRT_URL})
	FetchContent_GetProperties(prt)
	if (NOT prt_POPULATED)
		message(STATUS "Fetching PRT from ${PRT_URL}...")
		FetchContent_Populate(prt)
	endif ()

	set(prt_DIR "${prt_SOURCE_DIR}/cmake")
endif ()

find_package(prt CONFIG REQUIRED)
set(CESDK_VERSION "cesdk_${PRT_VERSION_MAJOR}_${PRT_VERSION_MINOR}_${PRT_VERSION_MICRO}")
message(STATUS "Using prt_DIR = ${prt_DIR} with version ${PRT_VERSION_MAJOR}.${PRT_VERSION_MINOR}.${PRT_VERSION_MICRO}")

function(srl_add_dependency_prt TGT)
	target_compile_definitions(${TGT} PRIVATE -DPRT_VERSION_MAJOR=${PRT_VERSION_MAJOR} -DPRT_VERSION_MINOR=${PRT_VERSION_MINOR})
	target_include_directories(${TGT} PRIVATE ${PRT_INCLUDE_PATH})
	target_link_libraries(${TGT} PRIVATE ${PRT_LINK_LIBRARIES})
endfunction()


### autodesk maya installation location

if (NOT maya_DIR)
	if (WIN32)
		if (EXISTS "C:/Program Files/Autodesk/Maya2019")
			set(maya_DIR "C:/Program Files/Autodesk/Maya2019")
		else ()
			set(maya_DIR "C:/Program Files/Autodesk/Maya2018")
		endif ()
	else ()
		if (EXISTS "/opt/autodesk/maya2019")
			set(maya_DIR "/opt/autodesk/maya2019")
		else ()
			set(maya_DIR "/opt/autodesk/maya2018")
		endif ()
	endif ()
endif ()
message(STATUS "Using maya_DIR = ${maya_DIR} (use '-Dmaya_DIR=xxx' to override)")

find_path(maya_INCLUDE_PATH NAMES "maya/MApiVersion.h" PATHS "${maya_DIR}/include" NO_DEFAULT_PATH)

set(MAYA_LIB_DIR "${maya_DIR}/lib")
find_library(maya_LINK_LIB_FOUNDATION NAMES "Foundation" PATHS "${MAYA_LIB_DIR}")
if (maya_LINK_LIB_FOUNDATION)
	list(APPEND maya_LINK_LIBRARIES ${maya_LINK_LIB_FOUNDATION})
endif ()
find_library(maya_LINK_LIB_OPENMAYA NAMES "OpenMaya" PATHS "${MAYA_LIB_DIR}")
if (maya_LINK_LIB_OPENMAYA)
	list(APPEND maya_LINK_LIBRARIES ${maya_LINK_LIB_OPENMAYA})
endif ()
find_library(maya_LINK_LIB_OPENMAYAUI NAMES "OpenMayaUI" PATHS "${MAYA_LIB_DIR}")
if (maya_LINK_LIB_OPENMAYAUI)
	list(APPEND maya_LINK_LIBRARIES ${maya_LINK_LIB_OPENMAYAUI})
endif ()
find_library(maya_LINK_LIB_METADATA NAMES "MetaData" PATHS "${MAYA_LIB_DIR}")
if (maya_LINK_LIB_METADATA)
	list(APPEND maya_LINK_LIBRARIES ${maya_LINK_LIB_METADATA})
endif ()

# temporary heuristic to detect maya version number
if (maya_DIR MATCHES "[Mm]aya2018")
	set(maya_VERSION_MAJOR "2018")
elseif (maya_DIR MATCHES "[Mm]aya2019")
	set(maya_VERSION_MAJOR "2019")
endif ()

function(srl_add_dependency_maya TGT)
	if (maya_INCLUDE_PATH)
		target_include_directories(${TGT} PRIVATE ${maya_INCLUDE_PATH})
	endif ()
	target_link_libraries(${TGT} PRIVATE ${maya_LINK_LIBRARIES})
endfunction()


### targets installation location

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../install" CACHE PATH "default install path" FORCE)
endif ()
message(STATUS "Installing into ${CMAKE_INSTALL_PREFIX}")
