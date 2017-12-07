cmake_minimum_required(VERSION 2.6)
project(ogreExporter)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE) 
set(CMAKE_DEBUG_POSTFIX _d)

# Find Ogre
find_package(OGRE REQUIRED)

# See if we have Maya..
# Set from env first
if (NOT MAYA_HOME)
	SET(MAYA_HOME $ENV{MAYA_HOME})
endif()

if (NOT EXISTS ${MAYA_HOME})
	MESSAGE(FATAL_ERROR "MAYA_HOME does not resolve to a directory. You can either specify it as environment variable (MAYA_HOME) or pass it to CMake as argument (-DMAYA_HOME=XYZ). Set this to the root folder of Maya, such as -DMAYA_HOME=\"C:/Program Files/Autodesk/Maya2018\".")
endif()

# Double check for required folders..
if (NOT EXISTS "${MAYA_HOME}/include")
	MESSAGE(FATAL_ERROR "MAYA_HOME does not contain a /include folder which is required by this project. Please make sure your MAYA_HOME points to the root folder of Maya. If this persists and you are sure Maya exists, a reinstall of Maya may be worth trying.")
endif()
if (NOT EXISTS "${MAYA_HOME}/lib")
	MESSAGE(FATAL_ERROR "MAYA_HOME does not contain a /lib folder which is required to link with Maya. Please make sure your MAYA_HOME points to the root folder of Maya. If this persists and you are sure Maya exists, a reinstall of Maya may be worth trying.")
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}
			${CMAKE_CURRENT_SOURCE_DIR}/include
			${OGRE_INCLUDE_DIRS} 
			"${MAYA_HOME}/include")

link_directories(${OGRE_LIBRARY_DIRS} "${MAYA_HOME}/lib")


set( ogreExporter_h ${ogreExporter_h}		
			./include/_singleton.h
			./include/animation.h
			./include/blendshape.h
			./include/material.h
			./include/materialSet.h
			./include/mayaExportLayer.h
			./include/mesh.h
			./include/ogreExporter.h
			./include/paramlist.h
			./include/particles.h
			./include/skeleton.h
			./include/submesh.h
			./include/vertex.h
			)

set( ogreExporter_src ${ogreExporter_src}	
			./src/blendshape.cpp
			./src/material.cpp
			./src/materialSet.cpp
			./src/mesh.cpp
			./src/ogreExporter.cpp
			./src/paramlist.cpp
			./src/particles.cpp
			./src/skeleton.cpp
			./src/submesh.cpp
			)

# Create source groups
source_group("Header Files" FILES ${ogreExporter_h})

# Add header files to sources to make headers visible in Visual Studio
set(ogreExporter_src ${ogreExporter_src} ${ogreExporter_h})

# Create shared library
add_library(ogreExporter SHARED ${ogreExporter_src})
set_property(TARGET ogreExporter PROPERTY COMPILE_DEFINITIONS NDEBUG COMPILE_DEFINITIONS _WINDOWS COMPILE_DEFINITIONS NT_PLUGIN COMPILE_DEFINITIONS REQUIRE_IOSTREAM)
#set_target_properties( ogreExporter PROPERTIES IMPORT_SUFFIX ".mll" )
set_target_properties( ogreExporter PROPERTIES OUTPUT_NAME "ogreExporter" )
set_target_properties( ogreExporter PROPERTIES SUFFIX ".mll" )
set_target_properties( ogreExporter PROPERTIES LINK_FLAGS "/export:initializePlugin /export:uninitializePlugin" )

# Add library dependencies
target_link_libraries(ogreExporter
			optimized OgreMain.lib debug OgreMain.lib
			optimized Foundation.lib debug Foundation.lib
			optimized OpenMaya.lib debug OpenMaya.lib
			optimized OpenMayaAnim.lib debug OpenMayaAnim.lib
			optimized OpenMayaRender.lib debug OpenMayaRender.lib
			)