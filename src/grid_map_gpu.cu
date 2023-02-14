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
    this->origin_x=origin_x;
    this->origin_y=origin_y;
    this->ptr_width= &this->width;
    this->ptr_height= &this->height;
    this->ptr_size= &this->map_size;
    this->ptr_resolution=&this->resolution;
    this->ptr_origin_x=&this->origin_x;
    this->ptr_origin_y=&this->origin_y;
    cudaMallocManaged(&(this->Map),this->width*this->height*sizeof(float));
    cudaMallocManaged(&(this->ptr_width),sizeof(float));
    cudaMallocManaged(&(this->ptr_height),sizeof(float));
    cudaMallocManaged(&(this->ptr_size),sizeof(float));

};

__device__ GPU_GridMap* GPU_GridMap::shift_to_gpu(){
    
}
int GPU_GridMap::indexOf(float x, float y){
    int x_map=(*(this->ptr_origin_x))+x/(*(this->ptr_resolution));
    int y_map=(*(this->ptr_origin_y))+y/(*(this->ptr_resolution));
    return x_map+this->width*y_map;
}


__device__ int GPU_GridMap::indexOfN(float x, float y){
    int x_map=this->origin_x+x/((this->resolution));
    int y_map=(this->origin_y)+y/(this->resolution);
    return x_map+this->width*y_map;
}
