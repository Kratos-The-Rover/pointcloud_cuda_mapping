#ifndef POINTCLOUD_CUDA_CUH
#define POINTCLOUD_CUDA_CUH
#include <iostream>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <cublas_v2.h>
#include <thrust/device_vector.h>
#include <pointcloud_cuda_mapping/grid_map_gpu.cuh>
typedef pcl::PointCloud<pcl::PointXYZRGB> PointCloud;

class CudaPointCloud{
    private:
        __host__ __device__ float* cloud;
        float* rgb;
        float* tf_mat;
        __device__ __host__ size_t cloud_size;
        float* transformed_cloud;
        int num_cloud_dims;
        pcl::PCLHeader cloud_header;
        cublasHandle_t* cub_handle_ptr;

    public:

        __device__ __host__ CudaPointCloud(const PointCloud::ConstPtr& cloud_ptr,cublasHandle_t* cub_handle_pt);
        float* mount_filtered_cloud(const PointCloud::ConstPtr& cloud_ptr,float* cuda_cloud_mat);
        float* transform_cloud(float* arg_cloud, float* transform_mat);
        float* transform_cloud(cublasHandle_t* cub_handle_pt);
        float* add_mat(float* mat);
        void re_allocate_cloud(const PointCloud::ConstPtr& cloud_ptr);
        void get_pcl_cloud( pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_msg);
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr get_pcl_cloud();
        void read_cuda_cloud();
        int get_cloud_size();
        __host__ __device__ size_t size(int tid);
        __host__ __device__ float* data();

        ~CudaPointCloud();
    };

    void makeMap(CudaPointCloud* cloud,  GPU_GridMap* grid_map);
__global__ void makeMapFromCloud(CudaPointCloud* cloud,  GPU_GridMap* grid_map);
#endif