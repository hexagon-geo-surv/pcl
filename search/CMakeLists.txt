set(SUBSYS_NAME search)
set(SUBSYS_DESC "Point cloud generic search library")
set(SUBSYS_DEPS common kdtree octree)

PCL_SUBSYS_OPTION(build "${SUBSYS_NAME}" "${SUBSYS_DESC}" ON)
PCL_SUBSYS_DEPEND(build NAME ${SUBSYS_NAME} DEPS ${SUBSYS_DEPS} EXT_DEPS flann)

PCL_ADD_DOC("${SUBSYS_NAME}")

if(NOT build)
  return()
endif()

set(srcs
  src/search.cpp
  src/kdtree.cpp
  src/brute_force.cpp
  src/organized.cpp
  src/octree.cpp
)

set(incs
  "include/pcl/${SUBSYS_NAME}/search.h"
  "include/pcl/${SUBSYS_NAME}/kdtree.h"
  "include/pcl/${SUBSYS_NAME}/kdtree_nanoflann.h"
  "include/pcl/${SUBSYS_NAME}/brute_force.h"
  "include/pcl/${SUBSYS_NAME}/organized.h"
  "include/pcl/${SUBSYS_NAME}/octree.h"
  "include/pcl/${SUBSYS_NAME}/flann_search.h"
  "include/pcl/${SUBSYS_NAME}/pcl_search.h"
)

set(impl_incs
  "include/pcl/${SUBSYS_NAME}/impl/search.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/kdtree.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/flann_search.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/brute_force.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/organized.hpp"
)

set(LIB_NAME "pcl_${SUBSYS_NAME}")
PCL_ADD_LIBRARY(${LIB_NAME} COMPONENT ${SUBSYS_NAME} SOURCES ${srcs} ${incs} ${impl_incs})
target_link_libraries("${LIB_NAME}" pcl_common FLANN::FLANN pcl_octree pcl_kdtree)
list(APPEND EXT_DEPS flann)
PCL_MAKE_PKGCONFIG(${LIB_NAME} COMPONENT ${SUBSYS_NAME} DESC ${SUBSYS_DESC} PCL_DEPS ${SUBSYS_DEPS})

PCL_ADD_INCLUDES("${SUBSYS_NAME}" "${SUBSYS_NAME}" ${incs})
PCL_ADD_INCLUDES("${SUBSYS_NAME}" "${SUBSYS_NAME}/impl" ${impl_incs})
