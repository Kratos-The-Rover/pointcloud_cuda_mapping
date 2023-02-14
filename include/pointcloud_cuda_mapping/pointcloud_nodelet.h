#ifndef POINTCLOUD_NODELET_H
#define POINTCLOUD_NODELET_H

#include <cuda_runtime.h>
#include <cublas_v2.h>

#include <ros/ros.h>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl_ros/point_cloud.h>

#include <pointcloud_cuda_mapping/pointcloud_cuda.cuh>

#include <pluginlib/class_list_macros.hpp>


#include <nodelet/nodelet.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer_interface.h>
#include <tf2_ros/buffer.h>
#include <grid_map_core/GridMap.hpp>
#include <grid_map_msgs/GridMap.h>
#include <tf2_eigen/tf2_eigen.h>
#include <sensor_msgs/PointCloud2.h>
typedef pcl::PointCloud<pcl::PointXYZRGB> PointCloud;

namespace pointcloud{
    class PointcloudProcessor: public nodelet::Nodelet{
        public:
            PointcloudProcessor();
        private:
            virtual void onInit();

            // initialization Params
            int concurrency_lvl;
            bool concurrent_callback;
            double transform_tolerance;
            std::string target_frame;

            // cublas handle
            cublasHandle_t h;

            // clouds
            PointCloud::Ptr host_cloud_ptr;
            std::shared_ptr <CudaPointCloud> dev_cloud_ptr;

            // ros stuff
            ros::NodeHandle nh;
            ros::NodeHandle private_nh;
            std::shared_ptr<ros::Subscriber> cloud_sub;
            std::shared_ptr<ros::SubscribeOptions> cloud_sub_opts;
            // std::shared_ptr<ros::Publisher<pcl::PointCloud<pcl::PointXYZRGB>::Ptr>> cloud_pub;
            ros::Publisher cloud_pub;
            ros::Publisher map_pub;


            // tf struff
            std::shared_ptr<tf2_ros::TransformListener> frame_listener;
            std::shared_ptr<tf2_ros::BufferInterface> frame_buffer_interface;
            std::shared_ptr<tf2_ros::Buffer> frame_buffer;
            geometry_msgs::TransformStamped curr_transform;

            //
            bool cloud_allocated=false;
            pcl::PointCloud<pcl::PointXYZRGB>::Ptr get_pcl_cloud(std::shared_ptr <CudaPointCloud> dev_cloud_ptr);


            //map
            grid_map_msgs::GridMapPtr msg_ptr;
            boost::shared_ptr <grid_map::GridMap>  map_ptr;

            void cloud_sub_callback(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_msg);
            // void cloud_sub_callback(const sensor_msgs::PointCloud2ConstPtr& cloud_msg_ptr);
            void createMsg();
            void createMap();

    };
}
#endif
