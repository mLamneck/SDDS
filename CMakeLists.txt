cmake_minimum_required(VERSION 3.16)
project(sdds LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sdds
	src/uPlatform.h
	src/uPlatform.cpp

	src/uStrings.h
	src/uStrings.cpp

	src/uEnumMacros.h
	src/uEnumMacros.cpp

	src/uTime.h
	src/uTime.cpp

	src/uLinkedList.h
	src/uLinkedList.cpp

	src/uTypedef.h
	src/uTypedef.cpp

	src/uMultask.h
	src/uMultask.cpp

    src/uJsonSerializer.h
    src/uJsonSerializer.cpp

    src/uPlainCommHandler.h
    src/uPlainCommHandler.cpp

	src/uParamSave.h
	src/uParamSave.cpp

	src/uCrc8.h
	src/uCrc8.cpp
)

if(DEFINED QT_VERSION_MAJOR)
	message("use sdds with Qt")
	target_link_libraries(sdds PUBLIC Qt6::Core)
    target_compile_definitions(sdds PUBLIC SDDS_ON_QT=1)
endif()

target_include_directories(sdds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


