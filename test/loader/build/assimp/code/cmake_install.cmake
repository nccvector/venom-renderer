# Install script for directory: /home/vector/venom-renderer/ext/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.3.0x" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5.3.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/vector/venom-renderer/test/loader/build/assimp/bin/libassimp.so.5.3.0"
    "/home/vector/venom-renderer/test/loader/build/assimp/bin/libassimp.so.5"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5.3.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so.5"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibassimp5.3.0x" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/vector/venom-renderer/test/loader/build/assimp/bin/libassimp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/anim.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/aabb.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ai_assert.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/camera.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/color4.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/color4.inl"
    "/home/vector/venom-renderer/test/loader/build/assimp/code/../include/assimp/config.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ColladaMetaData.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/commonMetaData.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/defs.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/cfileio.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/light.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/material.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/material.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/matrix3x3.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/matrix3x3.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/matrix4x4.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/matrix4x4.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/mesh.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ObjMaterial.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/pbrmaterial.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/GltfMaterial.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/postprocess.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/quaternion.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/quaternion.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/scene.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/metadata.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/texture.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/types.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/vector2.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/vector2.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/vector3.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/vector3.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/version.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/cimport.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/AssertHandler.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/importerdesc.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Importer.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/IOStream.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/IOSystem.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Logger.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/LogStream.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/NullLogger.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/cexport.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Exporter.hpp"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/DefaultIOStream.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SceneCombiner.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/fast_atof.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/qnan.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/BaseImporter.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Hash.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ParsingUtils.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/StreamReader.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/StreamWriter.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/StringComparison.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/StringUtils.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SGSpatialSort.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/GenericProperty.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SpatialSort.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SmallVector.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SmoothingGroups.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/StandardShapes.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/RemoveComments.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Subdivision.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Vertex.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/LineSplitter.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/TinyFormatter.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Profiler.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/LogAux.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Bitmap.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/XMLTools.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/XmlParser.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/BlobIOSystem.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/MathFunctions.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Exceptional.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/ByteSwapper.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/home/vector/venom-renderer/ext/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

