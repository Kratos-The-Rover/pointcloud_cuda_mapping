#include<pointcloud_cuda_mapping/grid_map_gpu.cuh>

GPU_GridMap::GPU_GridMap(int resolution,int width,int height){
    this->resolution=resolution;
    this->width=width;
    this->height=height;
    this->origin={0,0};
    cudaMallocManaged(&(this->Map),width*height*sizeof(float));
};

GPU_GridMap::GPU_GridMap(int resolution,float width,float height){
    this->resolution=resolution;
    this->width=(int)(width/resolution);
    this->height=(int)(height/resolution);
    this->origin={0,0};
    cudaMallocManaged(&(this->Map),this->width*this->height*sizeof(float));
};


GPU_GridMap::GPU_GridMap(int resolution,int width,int height,int origin_x, int origin_y){
    this->resolution=resolution;
    this->width=width;
    this->height=height;
    this->origin={origin_x,origin_y};
    cudaMallocManaged(&(this->Map),width*height*sizeof(float));
};


GPU_GridMap::GPU_GridMap(int resolution,float width,float height, float origin_x, float origin_y){
    this->resolution=resolution;
    this->width=(int)(width/resolution);
    this->height=(int)(height/resolution);
    this->origin={origin_x/resolution,origin_y/resolution};
    cudaMallocManaged(&(this->Map),this->width*this->height*sizeof(float));
};

GPU_GridMap::GPU_GridMap(int resolution,float width,float height, int origin_x, int origin_y){
    this->resolution=resolution;
    this->width=(int)(width/resolution);
    this->height=(int)(height/resolution);
    this->origin={origin_x/resolution,origin_y/resolution};
    this->origin={origin_x,origin_y};
    cudaMallocManaged(&(this->Map),this->width*this->height*sizeof(float));
};

__device__ GPU_GridMap* GPU_GridMap::shift_to_gpu(){
    
}
__device__ int GPU_GridMap::indexOf(float x, float y){
    
}
