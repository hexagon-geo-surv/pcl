set(SUBSYS_NAME octree)
set(SUBSYS_DESC "Point cloud octree library")
set(SUBSYS_DEPS common)

PCL_SUBSYS_OPTION(build "${SUBSYS_NAME}" "${SUBSYS_DESC}" ON)
PCL_SUBSYS_DEPEND(build NAME ${SUBSYS_NAME} DEPS ${SUBSYS_DEPS})

PCL_ADD_DOC("${SUBSYS_NAME}")

if(NOT build)
  return()
endif()

set(srcs
  src/octree_inst.cpp
)

set(incs
  "include/pcl/${SUBSYS_NAME}/octree_base.h"
  "include/pcl/${SUBSYS_NAME}/octree_container.h"
  "include/pcl/${SUBSYS_NAME}/octree_impl.h"
  "include/pcl/${SUBSYS_NAME}/octree_nodes.h"
  "include/pcl/${SUBSYS_NAME}/octree_key.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_density.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_occupancy.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_singlepoint.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_pointvector.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_changedetector.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_voxelcentroid.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud.h"
  "include/pcl/${SUBSYS_NAME}/octree_iterator.h"
  "include/pcl/${SUBSYS_NAME}/octree_search.h"
  "include/pcl/${SUBSYS_NAME}/octree.h"
  "include/pcl/${SUBSYS_NAME}/octree2buf_base.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_adjacency.h"
  "include/pcl/${SUBSYS_NAME}/octree_pointcloud_adjacency_container.h"
)

set(impl_incs
  "include/pcl/${SUBSYS_NAME}/impl/octree_base.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree_pointcloud.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree2buf_base.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree_iterator.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree_search.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree_pointcloud_voxelcentroid.hpp"
  "include/pcl/${SUBSYS_NAME}/impl/octree_pointcloud_adjacency.hpp"
)

set(LIB_NAME "pcl_${SUBSYS_NAME}")
PCL_ADD_LIBRARY(${LIB_NAME} COMPONENT ${SUBSYS_NAME} SOURCES ${srcs} ${incs} ${impl_incs})
target_link_libraries("${LIB_NAME}" pcl_common)
PCL_MAKE_PKGCONFIG(${LIB_NAME} COMPONENT ${SUBSYS_NAME} DESC ${SUBSYS_DESC} PCL_DEPS ${SUBSYS_DEPS})

# Install include files
PCL_ADD_INCLUDES("${SUBSYS_NAME}" "${SUBSYS_NAME}" ${incs})
PCL_ADD_INCLUDES("${SUBSYS_NAME}" "${SUBSYS_NAME}/impl" ${impl_incs})
