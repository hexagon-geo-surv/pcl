/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */

#pragma once

#include <pcl/common/common.h>
#include <pcl/common/point_tests.h> // for pcl::isFinite
#include <pcl/octree/impl/octree_base.hpp>
#include <pcl/types.h>

#include <cassert>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    OctreePointCloud(const double resolution)
: OctreeT()
, input_(PointCloudConstPtr())
, indices_(IndicesConstPtr())
, resolution_(resolution)
, max_x_(resolution)
, max_y_(resolution)
, max_z_(resolution)
{
  if (resolution <= 0.0) {
    PCL_THROW_EXCEPTION(InitFailedException,
                        "[pcl::octree::OctreePointCloud::OctreePointCloud] Resolution "
                            << resolution << " must be > 0!");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    addPointsFromInputCloud()
{
  if (indices_) {
    for (const auto& index : *indices_) {
      assert((index >= 0) && (static_cast<std::size_t>(index) < input_->size()));

      if (isFinite((*input_)[index])) {
        // add points to octree
        this->addPointIdx(index);
      }
    }
  }
  else {
    for (index_t i = 0; i < static_cast<index_t>(input_->size()); i++) {
      if (isFinite((*input_)[i])) {
        // add points to octree
        this->addPointIdx(i);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    addPointFromCloud(const uindex_t point_idx_arg, IndicesPtr indices_arg)
{
  this->addPointIdx(point_idx_arg);
  if (indices_arg)
    indices_arg->push_back(point_idx_arg);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    addPointToCloud(const PointT& point_arg, PointCloudPtr cloud_arg)
{
  assert(cloud_arg == input_);

  cloud_arg->push_back(point_arg);

  this->addPointIdx(cloud_arg->size() - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    addPointToCloud(const PointT& point_arg,
                    PointCloudPtr cloud_arg,
                    IndicesPtr indices_arg)
{
  assert(cloud_arg == input_);
  assert(indices_arg == indices_);

  cloud_arg->push_back(point_arg);

  this->addPointFromCloud(cloud_arg->size() - 1, indices_arg);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
bool
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    isVoxelOccupiedAtPoint(const PointT& point_arg) const
{
  if (!isPointWithinBoundingBox(point_arg)) {
    return false;
  }

  OctreeKey key;

  // generate key for point
  this->genOctreeKeyforPoint(point_arg, key);

  // search for key in octree
  return (this->existLeaf(key));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
bool
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    isVoxelOccupiedAtPoint(const index_t& point_idx_arg) const
{
  // retrieve point from input cloud
  const PointT& point = (*this->input_)[point_idx_arg];

  // search for voxel at point in octree
  return (this->isVoxelOccupiedAtPoint(point));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
bool
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    isVoxelOccupiedAtPoint(const double point_x_arg,
                           const double point_y_arg,
                           const double point_z_arg) const
{
  // create a new point with the argument coordinates
  PointT point;
  point.x = point_x_arg;
  point.y = point_y_arg;
  point.z = point_z_arg;

  // search for voxel at point in octree
  return (this->isVoxelOccupiedAtPoint(point));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    deleteVoxelAtPoint(const PointT& point_arg)
{
  if (!isPointWithinBoundingBox(point_arg)) {
    return;
  }

  OctreeKey key;

  // generate key for point
  this->genOctreeKeyforPoint(point_arg, key);

  this->removeLeaf(key);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    deleteVoxelAtPoint(const index_t& point_idx_arg)
{
  // retrieve point from input cloud
  const PointT& point = (*this->input_)[point_idx_arg];

  // delete leaf at point
  this->deleteVoxelAtPoint(point);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
pcl::uindex_t
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getOccupiedVoxelCenters(AlignedPointTVector& voxel_center_list_arg) const
{
  OctreeKey key;
  key.x = key.y = key.z = 0;

  voxel_center_list_arg.clear();

  return getOccupiedVoxelCentersRecursive(this->root_node_, key, voxel_center_list_arg);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
pcl::uindex_t
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getApproxIntersectedVoxelCentersBySegment(const Eigen::Vector3f& origin,
                                              const Eigen::Vector3f& end,
                                              AlignedPointTVector& voxel_center_list,
                                              float precision)
{
  Eigen::Vector3f direction = end - origin;
  float norm = direction.norm();
  direction.normalize();

  const float step_size = static_cast<float>(resolution_) * precision;
  // Ensure we get at least one step for the first voxel.
  const auto nsteps = std::max<std::size_t>(1, norm / step_size);

  OctreeKey prev_key;

  bool bkeyDefined = false;

  // Walk along the line segment with small steps.
  for (std::size_t i = 0; i < nsteps; ++i) {
    Eigen::Vector3f p = origin + (direction * step_size * static_cast<float>(i));

    PointT octree_p;
    octree_p.x = p.x();
    octree_p.y = p.y();
    octree_p.z = p.z();

    OctreeKey key;
    this->genOctreeKeyforPoint(octree_p, key);

    // Not a new key, still the same voxel.
    if ((key == prev_key) && (bkeyDefined))
      continue;

    prev_key = key;
    bkeyDefined = true;

    PointT center;
    genLeafNodeCenterFromOctreeKey(key, center);
    voxel_center_list.push_back(center);
  }

  OctreeKey end_key;
  PointT end_p;
  end_p.x = end.x();
  end_p.y = end.y();
  end_p.z = end.z();
  this->genOctreeKeyforPoint(end_p, end_key);
  if (!(end_key == prev_key)) {
    PointT center;
    genLeafNodeCenterFromOctreeKey(end_key, center);
    voxel_center_list.push_back(center);
  }

  return (static_cast<uindex_t>(voxel_center_list.size()));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    defineBoundingBox()
{

  double minX, minY, minZ, maxX, maxY, maxZ;

  PointT min_pt;
  PointT max_pt;

  // bounding box cannot be changed once the octree contains elements
  if (this->leaf_count_ != 0) {
    PCL_ERROR("[pcl::octree::OctreePointCloud::defineBoundingBox] Leaf count (%lu) "
              "must be 0\n",
              this->leaf_count_);
    return;
  }

  pcl::getMinMax3D(*input_, min_pt, max_pt);

  float minValue = std::numeric_limits<float>::epsilon() * 512.0f;

  minX = min_pt.x;
  minY = min_pt.y;
  minZ = min_pt.z;

  maxX = max_pt.x + minValue;
  maxY = max_pt.y + minValue;
  maxZ = max_pt.z + minValue;

  // generate bit masks for octree
  defineBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    defineBoundingBox(const double min_x_arg,
                      const double min_y_arg,
                      const double min_z_arg,
                      const double max_x_arg,
                      const double max_y_arg,
                      const double max_z_arg)
{
  // bounding box cannot be changed once the octree contains elements
  if (this->leaf_count_ != 0) {
    PCL_ERROR("[pcl::octree::OctreePointCloud::defineBoundingBox] Leaf count (%lu) "
              "must be 0\n",
              this->leaf_count_);
    return;
  }

  min_x_ = std::min(min_x_arg, max_x_arg);
  min_y_ = std::min(min_y_arg, max_y_arg);
  min_z_ = std::min(min_z_arg, max_z_arg);

  max_x_ = std::max(min_x_arg, max_x_arg);
  max_y_ = std::max(min_y_arg, max_y_arg);
  max_z_ = std::max(min_z_arg, max_z_arg);

  // generate bit masks for octree
  getKeyBitSize();

  bounding_box_defined_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    defineBoundingBox(const double max_x_arg,
                      const double max_y_arg,
                      const double max_z_arg)
{
  // bounding box cannot be changed once the octree contains elements
  if (this->leaf_count_ != 0) {
    PCL_ERROR("[pcl::octree::OctreePointCloud::defineBoundingBox] Leaf count (%lu) "
              "must be 0\n",
              this->leaf_count_);
    return;
  }

  min_x_ = std::min(0.0, max_x_arg);
  min_y_ = std::min(0.0, max_y_arg);
  min_z_ = std::min(0.0, max_z_arg);

  max_x_ = std::max(0.0, max_x_arg);
  max_y_ = std::max(0.0, max_y_arg);
  max_z_ = std::max(0.0, max_z_arg);

  // generate bit masks for octree
  getKeyBitSize();

  bounding_box_defined_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    defineBoundingBox(const double cubeLen_arg)
{
  // bounding box cannot be changed once the octree contains elements
  if (this->leaf_count_ != 0) {
    PCL_ERROR("[pcl::octree::OctreePointCloud::defineBoundingBox] Leaf count (%lu) "
              "must be 0\n",
              this->leaf_count_);
    return;
  }

  min_x_ = std::min(0.0, cubeLen_arg);
  min_y_ = std::min(0.0, cubeLen_arg);
  min_z_ = std::min(0.0, cubeLen_arg);

  max_x_ = std::max(0.0, cubeLen_arg);
  max_y_ = std::max(0.0, cubeLen_arg);
  max_z_ = std::max(0.0, cubeLen_arg);

  // generate bit masks for octree
  getKeyBitSize();

  bounding_box_defined_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getBoundingBox(double& min_x_arg,
                   double& min_y_arg,
                   double& min_z_arg,
                   double& max_x_arg,
                   double& max_y_arg,
                   double& max_z_arg) const
{
  min_x_arg = min_x_;
  min_y_arg = min_y_;
  min_z_arg = min_z_;

  max_x_arg = max_x_;
  max_y_arg = max_y_;
  max_z_arg = max_z_;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    adoptBoundingBoxToPoint(const PointT& point_arg)
{

  constexpr float minValue = std::numeric_limits<float>::epsilon();

  // increase octree size until point fits into bounding box
  while (true) {
    bool bLowerBoundViolationX = (point_arg.x < min_x_);
    bool bLowerBoundViolationY = (point_arg.y < min_y_);
    bool bLowerBoundViolationZ = (point_arg.z < min_z_);

    bool bUpperBoundViolationX = (point_arg.x >= max_x_);
    bool bUpperBoundViolationY = (point_arg.y >= max_y_);
    bool bUpperBoundViolationZ = (point_arg.z >= max_z_);

    // do we violate any bounds?
    if (bLowerBoundViolationX || bLowerBoundViolationY || bLowerBoundViolationZ ||
        bUpperBoundViolationX || bUpperBoundViolationY || bUpperBoundViolationZ ||
        (!bounding_box_defined_)) {

      if (bounding_box_defined_) {

        double octreeSideLen;
        unsigned char child_idx;

        // octree not empty - we add another tree level and thus increase its size by a
        // factor of 2*2*2
        child_idx = static_cast<unsigned char>((bLowerBoundViolationX ? 4 : 0) |
                                               (bLowerBoundViolationY ? 2 : 0) |
                                               (bLowerBoundViolationZ ? 1 : 0));

        BranchNode* newRootBranch;

        newRootBranch = new BranchNode();
        this->branch_count_++;

        this->setBranchChildPtr(*newRootBranch, child_idx, this->root_node_);

        this->root_node_ = newRootBranch;

        octreeSideLen = static_cast<double>(1 << this->octree_depth_) * resolution_;

        if (bLowerBoundViolationX)
          min_x_ -= octreeSideLen;

        if (bLowerBoundViolationY)
          min_y_ -= octreeSideLen;

        if (bLowerBoundViolationZ)
          min_z_ -= octreeSideLen;

        // configure tree depth of octree
        this->octree_depth_++;
        this->setTreeDepth(this->octree_depth_);

        // recalculate bounding box width
        octreeSideLen =
            static_cast<double>(1 << this->octree_depth_) * resolution_ - minValue;

        // increase octree bounding box
        max_x_ = min_x_ + octreeSideLen;
        max_y_ = min_y_ + octreeSideLen;
        max_z_ = min_z_ + octreeSideLen;
      }
      // bounding box is not defined - set it to point position
      else {
        // octree is empty - we set the center of the bounding box to our first pixel
        this->min_x_ = point_arg.x - this->resolution_ / 2;
        this->min_y_ = point_arg.y - this->resolution_ / 2;
        this->min_z_ = point_arg.z - this->resolution_ / 2;

        this->max_x_ = point_arg.x + this->resolution_ / 2;
        this->max_y_ = point_arg.y + this->resolution_ / 2;
        this->max_z_ = point_arg.z + this->resolution_ / 2;

        getKeyBitSize();

        bounding_box_defined_ = true;
      }
    }
    else
      // no bound violations anymore - leave while loop
      break;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    expandLeafNode(LeafNode* leaf_node,
                   BranchNode* parent_branch,
                   unsigned char child_idx,
                   uindex_t depth_mask)
{

  if (depth_mask) {
    // get amount of objects in leaf container
    std::size_t leaf_obj_count = (*leaf_node)->getSize();

    // copy leaf data
    Indices leafIndices;
    leafIndices.reserve(leaf_obj_count);

    (*leaf_node)->getPointIndices(leafIndices);

    // delete current leaf node
    this->deleteBranchChild(*parent_branch, child_idx);
    this->leaf_count_--;

    // create new branch node
    BranchNode* childBranch = this->createBranchChild(*parent_branch, child_idx);
    this->branch_count_++;

    // add data to new branch
    OctreeKey new_index_key;

    for (const auto& leafIndex : leafIndices) {

      const PointT& point_from_index = (*input_)[leafIndex];
      // generate key
      genOctreeKeyforPoint(point_from_index, new_index_key);

      LeafNode* newLeaf;
      BranchNode* newBranchParent;
      this->createLeafRecursive(
          new_index_key, depth_mask, childBranch, newLeaf, newBranchParent);

      (*newLeaf)->addPointIndex(leafIndex);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    addPointIdx(const uindex_t point_idx_arg)
{
  OctreeKey key;

  assert(point_idx_arg < input_->size());

  const PointT& point = (*input_)[point_idx_arg];

  // make sure bounding box is big enough
  adoptBoundingBoxToPoint(point);

  // generate key
  genOctreeKeyforPoint(point, key);

  LeafNode* leaf_node;
  BranchNode* parent_branch_of_leaf_node;
  auto depth_mask = this->createLeafRecursive(
      key, this->depth_mask_, this->root_node_, leaf_node, parent_branch_of_leaf_node);

  if (this->dynamic_depth_enabled_ && depth_mask) {
    // get amount of objects in leaf container
    std::size_t leaf_obj_count = (*leaf_node)->getSize();

    while (leaf_obj_count >= max_objs_per_leaf_ && depth_mask) {
      // index to branch child
      unsigned char child_idx = key.getChildIdxWithDepthMask(depth_mask * 2);

      expandLeafNode(leaf_node, parent_branch_of_leaf_node, child_idx, depth_mask);

      depth_mask = this->createLeafRecursive(key,
                                             this->depth_mask_,
                                             this->root_node_,
                                             leaf_node,
                                             parent_branch_of_leaf_node);
      leaf_obj_count = (*leaf_node)->getSize();
    }
  }

  (*leaf_node)->addPointIndex(point_idx_arg);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
const PointT&
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getPointByIndex(const uindex_t index_arg) const
{
  // retrieve point from input cloud
  assert(index_arg < input_->size());
  return ((*this->input_)[index_arg]);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getKeyBitSize()
{
  constexpr float minValue = std::numeric_limits<float>::epsilon();

  // find maximum key values for x, y, z
  const auto max_key_x =
      static_cast<uindex_t>(std::ceil((max_x_ - min_x_ - minValue) / resolution_));
  const auto max_key_y =
      static_cast<uindex_t>(std::ceil((max_y_ - min_y_ - minValue) / resolution_));
  const auto max_key_z =
      static_cast<uindex_t>(std::ceil((max_z_ - min_z_ - minValue) / resolution_));

  // find maximum amount of keys
  const auto max_voxels =
      std::max<uindex_t>(std::max(std::max(max_key_x, max_key_y), max_key_z), 2);

  // tree depth == amount of bits of max_voxels
  this->octree_depth_ = std::max<uindex_t>(
      std::min<uindex_t>(OctreeKey::maxDepth,
                         std::ceil(std::log2(max_voxels) - minValue)),
      0);

  const auto octree_side_len =
      static_cast<double>(1 << this->octree_depth_) * resolution_;

  if (this->leaf_count_ == 0) {
    double octree_oversize_x;
    double octree_oversize_y;
    double octree_oversize_z;

    octree_oversize_x = (octree_side_len - (max_x_ - min_x_)) / 2.0;
    octree_oversize_y = (octree_side_len - (max_y_ - min_y_)) / 2.0;
    octree_oversize_z = (octree_side_len - (max_z_ - min_z_)) / 2.0;

    assert(octree_oversize_x > -minValue);
    assert(octree_oversize_y > -minValue);
    assert(octree_oversize_z > -minValue);

    if (octree_oversize_x > minValue) {
      min_x_ -= octree_oversize_x;
      max_x_ += octree_oversize_x;
    }
    if (octree_oversize_y > minValue) {
      min_y_ -= octree_oversize_y;
      max_y_ += octree_oversize_y;
    }
    if (octree_oversize_z > minValue) {
      min_z_ -= octree_oversize_z;
      max_z_ += octree_oversize_z;
    }
  }
  else {
    max_x_ = min_x_ + octree_side_len;
    max_y_ = min_y_ + octree_side_len;
    max_z_ = min_z_ + octree_side_len;
  }

  // configure tree depth of octree
  this->setTreeDepth(this->octree_depth_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genOctreeKeyforPoint(const PointT& point_arg, OctreeKey& key_arg) const
{
  // calculate integer key for point coordinates
  key_arg.x = static_cast<uindex_t>((point_arg.x - this->min_x_) / this->resolution_);
  key_arg.y = static_cast<uindex_t>((point_arg.y - this->min_y_) / this->resolution_);
  key_arg.z = static_cast<uindex_t>((point_arg.z - this->min_z_) / this->resolution_);

  assert(key_arg.x <= this->max_key_.x);
  assert(key_arg.y <= this->max_key_.y);
  assert(key_arg.z <= this->max_key_.z);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genOctreeKeyforPoint(const double point_x_arg,
                         const double point_y_arg,
                         const double point_z_arg,
                         OctreeKey& key_arg) const
{
  PointT temp_point;

  temp_point.x = static_cast<float>(point_x_arg);
  temp_point.y = static_cast<float>(point_y_arg);
  temp_point.z = static_cast<float>(point_z_arg);

  // generate key for point
  genOctreeKeyforPoint(temp_point, key_arg);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
bool
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genOctreeKeyForDataT(const index_t& data_arg, OctreeKey& key_arg) const
{
  const PointT temp_point = getPointByIndex(data_arg);

  // generate key for point
  genOctreeKeyforPoint(temp_point, key_arg);

  return (true);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genLeafNodeCenterFromOctreeKey(const OctreeKey& key, PointT& point) const
{
  // define point to leaf node voxel center
  point.x = static_cast<float>((static_cast<double>(key.x) + 0.5f) * this->resolution_ +
                               this->min_x_);
  point.y = static_cast<float>((static_cast<double>(key.y) + 0.5f) * this->resolution_ +
                               this->min_y_);
  point.z = static_cast<float>((static_cast<double>(key.z) + 0.5f) * this->resolution_ +
                               this->min_z_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genVoxelCenterFromOctreeKey(const OctreeKey& key_arg,
                                uindex_t tree_depth_arg,
                                PointT& point_arg) const
{
  // generate point for voxel center defined by treedepth (bitLen) and key
  point_arg.x = static_cast<float>(
      (static_cast<double>(key_arg.x) + 0.5f) *
          (this->resolution_ *
           static_cast<double>(1 << (this->octree_depth_ - tree_depth_arg))) +
      this->min_x_);
  point_arg.y = static_cast<float>(
      (static_cast<double>(key_arg.y) + 0.5f) *
          (this->resolution_ *
           static_cast<double>(1 << (this->octree_depth_ - tree_depth_arg))) +
      this->min_y_);
  point_arg.z = static_cast<float>(
      (static_cast<double>(key_arg.z) + 0.5f) *
          (this->resolution_ *
           static_cast<double>(1 << (this->octree_depth_ - tree_depth_arg))) +
      this->min_z_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
void
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    genVoxelBoundsFromOctreeKey(const OctreeKey& key_arg,
                                uindex_t tree_depth_arg,
                                Eigen::Vector3f& min_pt,
                                Eigen::Vector3f& max_pt) const
{
  // calculate voxel size of current tree depth
  double voxel_side_len =
      this->resolution_ *
      static_cast<double>(1 << (this->octree_depth_ - tree_depth_arg));

  // calculate voxel bounds
  min_pt(0) = static_cast<float>(static_cast<double>(key_arg.x) * voxel_side_len +
                                 this->min_x_);
  min_pt(1) = static_cast<float>(static_cast<double>(key_arg.y) * voxel_side_len +
                                 this->min_y_);
  min_pt(2) = static_cast<float>(static_cast<double>(key_arg.z) * voxel_side_len +
                                 this->min_z_);

  max_pt(0) = min_pt(0) + voxel_side_len;
  max_pt(1) = min_pt(1) + voxel_side_len;
  max_pt(2) = min_pt(2) + voxel_side_len;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
double
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getVoxelSquaredSideLen(uindex_t tree_depth_arg) const
{
  double side_len;

  // side length of the voxel cube increases exponentially with the octree depth
  side_len = this->resolution_ *
             static_cast<double>(1 << (this->octree_depth_ - tree_depth_arg));

  // squared voxel side length
  side_len *= side_len;

  return (side_len);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
double
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getVoxelSquaredDiameter(uindex_t tree_depth_arg) const
{
  // return the squared side length of the voxel cube as a function of the octree depth
  return (getVoxelSquaredSideLen(tree_depth_arg) * 3);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT,
          typename BranchContainerT,
          typename OctreeT>
pcl::uindex_t
pcl::octree::OctreePointCloud<PointT, LeafContainerT, BranchContainerT, OctreeT>::
    getOccupiedVoxelCentersRecursive(const BranchNode* node_arg,
                                     const OctreeKey& key_arg,
                                     AlignedPointTVector& voxel_center_list_arg) const
{
  uindex_t voxel_count = 0;

  // iterate over all children
  for (unsigned char child_idx = 0; child_idx < 8; child_idx++) {
    if (!this->branchHasChild(*node_arg, child_idx))
      continue;

    const OctreeNode* child_node;
    child_node = this->getBranchChildPtr(*node_arg, child_idx);

    // generate new key for current branch voxel
    OctreeKey new_key;
    new_key.x = (key_arg.x << 1) | (!!(child_idx & (1 << 2)));
    new_key.y = (key_arg.y << 1) | (!!(child_idx & (1 << 1)));
    new_key.z = (key_arg.z << 1) | (!!(child_idx & (1 << 0)));

    switch (child_node->getNodeType()) {
    case BRANCH_NODE: {
      // recursively proceed with indexed child branch
      voxel_count += getOccupiedVoxelCentersRecursive(
          static_cast<const BranchNode*>(child_node), new_key, voxel_center_list_arg);
      break;
    }
    case LEAF_NODE: {
      PointT new_point;

      genLeafNodeCenterFromOctreeKey(new_key, new_point);
      voxel_center_list_arg.push_back(new_point);

      voxel_count++;
      break;
    }
    default:
      break;
    }
  }
  return (voxel_count);
}

#define PCL_INSTANTIATE_OctreePointCloudSingleBufferWithLeafDataTVector(T)             \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerPointIndices,                                        \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::OctreeBase<pcl::octree::OctreeContainerPointIndices,                \
                              pcl::octree::OctreeContainerEmpty>>;
#define PCL_INSTANTIATE_OctreePointCloudDoubleBufferWithLeafDataTVector(T)             \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerPointIndices,                                        \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::Octree2BufBase<pcl::octree::OctreeContainerPointIndices,            \
                                  pcl::octree::OctreeContainerEmpty>>;

#define PCL_INSTANTIATE_OctreePointCloudSingleBufferWithLeafDataT(T)                   \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerPointIndex,                                          \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::OctreeBase<pcl::octree::OctreeContainerPointIndex,                  \
                              pcl::octree::OctreeContainerEmpty>>;
#define PCL_INSTANTIATE_OctreePointCloudDoubleBufferWithLeafDataT(T)                   \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerPointIndex,                                          \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::Octree2BufBase<pcl::octree::OctreeContainerPointIndex,              \
                                  pcl::octree::OctreeContainerEmpty>>;

#define PCL_INSTANTIATE_OctreePointCloudSingleBufferWithEmptyLeaf(T)                   \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::OctreeBase<pcl::octree::OctreeContainerEmpty,                       \
                              pcl::octree::OctreeContainerEmpty>>;
#define PCL_INSTANTIATE_OctreePointCloudDoubleBufferWithEmptyLeaf(T)                   \
  template class PCL_EXPORTS pcl::octree::OctreePointCloud<                            \
      T,                                                                               \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::OctreeContainerEmpty,                                               \
      pcl::octree::Octree2BufBase<pcl::octree::OctreeContainerEmpty,                   \
                                  pcl::octree::OctreeContainerEmpty>>;
