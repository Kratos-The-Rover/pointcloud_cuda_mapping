#ifndef GRIDMAP_CUDA_CUH
#define GRIDMAP_CUDA_CUH
#include <iostream>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <cublas_v2.h>
#include <thrust/device_vector.h>
#define MAP_SIZE_LIMIT 1000

class GPU_GridMap{
    __host__ __device__ float* Map;
    __host__ __device__ int resolution;
    __host__ __device__ int width=0;
    __host__ __device__ int height=0;
    __host__ __device__ int map_size=0;
    __host__ __device__ int origin_x=0;
    __host__ __device__ int origin_y=0;

    int* ptr_resolution;
    int* ptr_width;
    int* ptr_height;
    int* ptr_size;
    int* ptr_origin_x;
    int* ptr_origin_y;


    __host__ __device__ std::pair<int,int> origin;
    public:
    GPU_GridMap(int resolution,int width=0,int height=0);
    GPU_GridMap(int resolution,float width=0,float height=0);
    GPU_GridMap(int resolution,float width=0,float height=0,float origin_x=0,float origin_y=0);
    GPU_GridMap(int resolution,int width=0,int height=0,int origin_x=0,int origin_y=0);
    GPU_GridMap(int resolution,float width=0,float height=0,int origin_x=0,int origin_y=0);
    GPU_GridMap* shift_to_gpu();
    __host__ __device__ int indexOf(float x, float y);
    __device__ float* pointerOf(float x,float y);
    __device__ float valueOf(float x,float y);
    __device__ int indexOf(float* ptr);
    __device__ float* pointerOf(float* ptr);
    __device__ float valueOf(float* ptr);
    __device__ int indexOfN(float x, float y);
    __device__ size_t size();
    // __device__ 
};
#endif