
#include <pluginlib/class_list_macros.h>
#include <pluginlib/class_list_macros.hpp>
#include <pointcloud_cuda_mapping/pointcloud_nodelet.h>
#include <pointcloud_cuda_mapping/pointcloud_cuda.cuh>
#include <tf2_eigen/tf2_eigen.h>
#include <sensor_msgs/PointCloud2.h>
#include <grid_map_msgs/GridMap.h>
#include <grid_map_core/GridMap.hpp>
#include <grid_map_ros/grid_map_ros.hpp>
namespace pointcloud
{

/******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
    * * Initialization
    * todo: ADD GridMap Initializations
    * todo: Add GridMap Publisher
********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
    
    PointcloudProcessor::PointcloudProcessor(){}
    void PointcloudProcessor::onInit(){


        std::cout<<"gonna create map"<<std::endl;
        createMap();
        createMsg();

        //* get node handles
        private_nh=getPrivateNodeHandle();
        nh=getMTNodeHandle();
        
        //* get params
        private_nh.param<int>("concurrency_lvl",concurrency_lvl,1);
        private_nh.param<bool>("concurrent_callbacks",concurrent_callback,0);
        private_nh.param<double>("transform_tolerence",transform_tolerance,0.05);
        private_nh.param<std::string>("target_frame",target_frame,"odom");
        
        //* init tf stuff
        frame_buffer.reset(new tf2_ros::Buffer());
        frame_listener.reset(new tf2_ros::TransformListener(*frame_buffer));

        //* init cuBLAS
        cublasCreate(&(this->h));
        NODELET_INFO_STREAM("CUBLAS INITIALIZED");


        //* init subsciber
        cloud_sub_opts.reset(new ros::SubscribeOptions());
        cloud_sub_opts->allow_concurrent_callbacks=concurrent_callback;
        cloud_sub_opts->initByFullCallbackType<const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr&>("cloud_in",concurrency_lvl,boost::bind(&PointcloudProcessor::cloud_sub_callback,this,_1));
        cloud_sub.reset(new ros::Subscriber());
        (*cloud_sub)=nh.subscribe((*cloud_sub_opts));


        //* cloud_pub.reset(new ros::Publisher); 
        // cloud_pub=nh.advertise<pcl::PointCloud<pcl::PointXYZRGB>>("/clouder",10);
        map_pub = nh.advertise<grid_map_msgs::GridMap>("clouder", 10);



    }





/******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
    * * Subscriber Callback
    * todo: ADD GridMap Publishing
********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/





    void PointcloudProcessor::cloud_sub_callback(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr& cloud_msg){
	try
        {
            ros::Rate rate(5);
            NODELET_INFO_STREAM("GOT A POINTCLOUD.");

            //* Allocating GPU Pointcloud
            if(!cloud_allocated){
                NODELET_INFO_STREAM("FIRST GPU POINTCLOUD ALLOCATION");
                dev_cloud_ptr.reset(new CudaPointCloud(cloud_msg,&h));
                cloud_allocated=true;
            }else {
                NODELET_INFO_STREAM("GPU POINTCLOUD ALLOCATION");
                dev_cloud_ptr->re_allocate_cloud(cloud_msg);
            }
            NODELET_INFO_STREAM("DEBUG 1");

            //* Benchmarking Time tool
            // ros::Time begin=ros::Time::now();
            
            
            // *Looking up for broadcasted transform
            try{
                curr_transform=frame_buffer->lookupTransform(target_frame,cloud_msg->header.frame_id,ros::Time(0));

            }catch(std::runtime_error& e){
                NODELET_ERROR_STREAM(e.what());
            }

        
            //* Transforming Transforms int SO(3) Matrices
            Eigen::MatrixXf tf_mat=((Eigen::MatrixXd)tf2::transformToEigen(curr_transform).matrix()).cast<float>();
            tf_mat.resize(1,16);
            float* tf_mat_ptr=tf_mat.data();

            //* Allocating the Matrix on GPU
            dev_cloud_ptr->add_mat(tf_mat_ptr);
            float* a;

            //* Call GPU Transform
            dev_cloud_ptr->transform_cloud(&(this->h));

            GPU_GridMap gpu_map(0.5,100,100, 100,100);
            // makeMap(dev_cloud_ptr.get(),  &gpu_map);
            
            //* Publish Transform
            pcl::PointCloud<pcl::PointXYZRGB>::Ptr publish_msg=get_pcl_cloud(dev_cloud_ptr);
            NODELET_INFO_STREAM("DEBUG 2");






            for(int i=0;i<publish_msg->size();i++){
                // std::cout<<"mapped: "<<i<<"/"<<publish_msg->size()<<" points "<<std::endl;
                if(!nh.ok() ){
                    // ros::Duration(5).sleep();
                    break;
                }
                grid_map::Position coordinate(publish_msg->points[i].x,publish_msg->points[i].y);
                grid_map::Index index;
                map_ptr->getIndex(coordinate,index);
   
                if(isnan(msg_ptr->data[0].data[800*index[1]+index[0]])){
                    msg_ptr->data[0].data[800*index[1]+index[0]]=publish_msg->points[i].z;
                    // map_ptr->atPosition("elevation",coordinate)=resVec[i+2];
                    map_ptr->atPosition("count",coordinate)=1;

                }
                else {
                    float prev=map_ptr->atPosition("elevation",coordinate);
                    msg_ptr->data[0].data[800*index[1]+index[0]]*=map_ptr->atPosition("count",coordinate);
                    msg_ptr->data[0].data[800*index[1]+index[0]]+=publish_msg->points[i].z;
                    
                    // std::cout<<msg_ptr->data[0].data[5000*index[0]+index[1]]<<std::endl;
                    msg_ptr->data[0].data[800*index[1]+index[0]]/=(++(map_ptr->atPosition("count",coordinate)));//(map_ptr->atPosition("elevation",coordinate)+resVec[i+2])/counter;
                    // std::cout<<map_ptr->atPosition("elevation",coordinate)<<std::endl;
                    // std::cout<<"count: " <<map_ptr->atPosition("count",coordinate)<<" prev: "<<prev<<" elev: "<< map_ptr->atPosition("elevation",coordinate)<<" resvec: "<<resVec[i+2]<<" diff: "<<map_ptr->atPosition("elevation",coordinate)-resVec[i+2]<<" "<<counter<<std::endl;

                }
            }

            std::cout<<"MAPPING DONE"<<std::endl;


            map_pub.publish(msg_ptr);
            // this->cloud_pub.publish(publish_msg);
            // rate.sleep();

            // NODELET_INFO_STREAM("CALLBACK FREQUENCY: "<<1/(ros::Time::now()-begin).toSec());

            }
            catch(tf2::TransformException& e)
            {
                NODELET_ERROR_STREAM("ERROR: "<<e.what());
            }
    
    
    }

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
    * * Get PointCloud Back From GPU (Made as a Library function)
    * todo: ADD GridMap Publishing
********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
  
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr PointcloudProcessor::get_pcl_cloud(std::shared_ptr <CudaPointCloud> dev_cloud_ptr){
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_msg;
        cloud_msg.reset(new pcl::PointCloud<pcl::PointXYZRGB>());
        cloud_msg->header.frame_id="odom";
        cloud_msg->header.stamp=ros::Time::now().toNSec()/1000;
        cloud_msg->height=1;
        cloud_msg->width=dev_cloud_ptr->get_cloud_size();
        cloud_msg->reserve(dev_cloud_ptr->get_cloud_size());
        dev_cloud_ptr->get_pcl_cloud(cloud_msg);
        return cloud_msg;
    }




    void PointcloudProcessor::createMsg(){
        std::cout<<"creating message"<<std::endl;
        msg_ptr.reset(new grid_map_msgs::GridMap);
        std::cout<<"step1 done"<<std::endl;


        msg_ptr->info.header.stamp.fromNSec(map_ptr->getTimestamp());
        msg_ptr->info.header.frame_id = map_ptr->getFrameId();
        msg_ptr->info.resolution = map_ptr->getResolution();
        msg_ptr->info.length_x = map_ptr->getLength().x();
        msg_ptr->info.length_y = map_ptr->getLength().y();
        msg_ptr->info.pose.position.x =map_ptr->getPosition().x();
        msg_ptr->info.pose.position.y = map_ptr->getPosition().y();
        msg_ptr->info.pose.position.z = 0.0;
        msg_ptr->info.pose.orientation.x = 0.0;
        msg_ptr->info.pose.orientation.y = 0.0;
        msg_ptr->info.pose.orientation.z = 0.0;
        msg_ptr->info.pose.orientation.w = 1.0;
        
        msg_ptr->data.clear();
        msg_ptr->layers={"elevation"};
        msg_ptr->basic_layers=map_ptr->getBasicLayers();

        std_msgs::Float32MultiArray dataArray;

        msg_ptr->outer_start_index=map_ptr->getStartIndex()(0);
        msg_ptr->inner_start_index=map_ptr->getStartIndex()(1);
        std::cout<<"main message created"<<std::endl;

        dataArray.layout.dim.reserve(2);
        dataArray.layout.dim.resize(2);

        dataArray.layout.dim[0].stride = 640000;
        dataArray.layout.dim[0].size = 800;
        dataArray.layout.dim[1].stride = 800;
        dataArray.layout.dim[1].size = 800;
        dataArray.layout.dim[0].label=grid_map::storageIndexNames[grid_map::StorageIndices::Column];
        dataArray.layout.dim[1].label=grid_map::storageIndexNames[grid_map::StorageIndices::Row];
        std::vector<float> vec(640000,NAN);
        dataArray.data=vec;
        msg_ptr->data.push_back(dataArray);
        std::cout<<"message created"<<std::endl;
    }


    void PointcloudProcessor::createMap(){
        std::cout<<"creating map"<<std::endl;
        map_ptr.reset(new grid_map::GridMap);
        map_ptr->add("elevation");
        map_ptr->add("count");
        map_ptr->setFrameId("odom");
        map_ptr->setGeometry(grid_map::Length(80,80),0.1);
        map_ptr->clearAll();

    }




    
} // namespace 
PLUGINLIB_EXPORT_CLASS(pointcloud::PointcloudProcessor, nodelet::Nodelet)




