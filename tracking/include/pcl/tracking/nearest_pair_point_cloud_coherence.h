#pragma once

#include <pcl/search/search.h>
#include <pcl/tracking/coherence.h>

namespace pcl {
namespace tracking {
/** \brief @b NearestPairPointCloudCoherence computes coherence between two pointclouds
 * using the nearest point pairs.
 * \author Ryohei Ueda
 * \ingroup tracking
 */
template <typename PointInT>
class PCL_EXPORTS NearestPairPointCloudCoherence
: public PointCloudCoherence<PointInT> {
public:
  using PointCloudCoherence<PointInT>::getClassName;
  using PointCloudCoherence<PointInT>::coherence_name_;
  using PointCloudCoherence<PointInT>::target_input_;

  using PointCoherencePtr = typename PointCloudCoherence<PointInT>::PointCoherencePtr;
  using PointCloudInConstPtr =
      typename PointCloudCoherence<PointInT>::PointCloudInConstPtr;
  using BaseClass = PointCloudCoherence<PointInT>;

  using Ptr = shared_ptr<NearestPairPointCloudCoherence<PointInT>>;
  using ConstPtr = shared_ptr<const NearestPairPointCloudCoherence<PointInT>>;
  using SearchPtr = typename pcl::search::Search<PointInT>::Ptr;
  using SearchConstPtr = typename pcl::search::Search<PointInT>::ConstPtr;

  /** \brief empty constructor */
  NearestPairPointCloudCoherence()
  {
    coherence_name_ = "NearestPairPointCloudCoherence";
  }

  /** \brief Provide a pointer to a dataset to add additional information
   * to estimate the features for every point in the input dataset.  This
   * is optional, if this is not set, it will only use the data in the
   * input cloud to estimate the features.  This is useful when you only
   * need to compute the features for a downsampled cloud.
   * \param search a pointer to a PointCloud message
   */
  inline void
  setSearchMethod(const SearchPtr& search)
  {
    search_ = search;
  }

  /** \brief Get a pointer to the point cloud dataset. */
  inline SearchPtr
  getSearchMethod()
  {
    return (search_);
  }

  /** \brief add a PointCoherence to the PointCloudCoherence.
   * \param[in] cloud coherence a pointer to PointCoherence.
   */
  inline void
  setTargetCloud(const PointCloudInConstPtr& cloud) override
  {
    new_target_ = true;
    PointCloudCoherence<PointInT>::setTargetCloud(cloud);
  }

  /** \brief set maximum distance to be taken into account.
   * \param[in] val maximum distance.
   */
  inline void
  setMaximumDistance(double val)
  {
    maximum_distance_ = val;
  }

protected:
  using PointCloudCoherence<PointInT>::point_coherences_;

  /** \brief This method should get called before starting the actual
   * computation. */
  bool
  initCompute() override;

  /** \brief A flag which is true if target_input_ is updated */
  bool new_target_{false};

  /** \brief A pointer to the spatial search object. */
  SearchPtr search_{nullptr};

  /** \brief max of distance for points to be taken into account*/
  double maximum_distance_{std::numeric_limits<double>::max()};

  /** \brief compute the nearest pairs and compute coherence using
   * point_coherences_ */
  void
  computeCoherence(const PointCloudInConstPtr& cloud,
                   const IndicesConstPtr& indices,
                   float& w_j) override;
};
} // namespace tracking
} // namespace pcl

// #include <pcl/tracking/impl/nearest_pair_point_cloud_coherence.hpp>
#ifdef PCL_NO_PRECOMPILE
#include <pcl/tracking/impl/nearest_pair_point_cloud_coherence.hpp>
#endif
