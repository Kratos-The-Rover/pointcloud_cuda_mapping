cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(pointcloud_cuda_mapping LANGUAGES CXX CUDA)
set(CUDA_DIR /usr/local/cuda-11)
add_compile_options(-std=c++14 )
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CUDA_STANDARD 14)
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -DMY_DEF=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMY_DEF=1" )
# set(CMAKE_CXX_COMPILER nvcc)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
  tf
  tf2
  pcl_conversions
  pcl_ros
  grid_map_core
  grid_map_msgs
  grid_map_ros
  grid_map_loader
  nodelet
)
find_package(CUDA REQUIRED)
find_package(PCL REQUIRED)

# find_package(Boost REQUIRED COMPONENTS system)

LIST(APPEND CUDA_NVCC_FLAGS "-arch=sm_75") #to be changed for Jetson
link_directories(${CUDA_LIBRARY_DIRS})

 # only necessary if you wish to create a lib
set(CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE OFF)
set(BUILD_SHARED_LIBS ON)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  ${CUDA_INCLUDE_DIRS}
)

# cuda_compile(cloud_cuda src/pointcloud_cuda.cu )
add_library(Cloud_cuda SHARED src/pointcloud_cuda.cu)
target_link_libraries(Cloud_cuda ${catkin_LIBRARIES} ${CUDA_LIBRARIES} ${PCL_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES})
target_compile_features(Cloud_cuda PUBLIC cxx_std_11)
set_target_properties(Cloud_cuda PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON CUDA_SEPARABLE_COMPILATION ON POSITION_INDEPENDENT_CODE ON CUDA_ARCHITECTURES "75")


# cuda_compile(cloud_cuda src/pointcloud_cuda.cu )
add_library(map_cuda SHARED src/grid_map_gpu.cu)
target_link_libraries(map_cuda ${catkin_LIBRARIES} ${CUDA_LIBRARIES} ${PCL_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES})
target_compile_features(map_cuda PUBLIC cxx_std_11)
set_target_properties(map_cuda PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON CUDA_SEPARABLE_COMPILATION ON POSITION_INDEPENDENT_CODE ON CUDA_ARCHITECTURES "75")



# target_link_libraries(cloud_cuda ${CUDA_LIBRARIES})
# add_executable(bruh src/pointcloud_node.cpp)

# target_link_libraries(bruh Cloud_cuda)
# target_link_libraries(bruh PRIVATE Cloud_cuda ${catkin_LIBRARIES} ${PCL_LIBRARIES} ${CUDA_LIBRARIES})
add_library(pointcloud_processor_lib src/pointcloud_nodelet.cpp)
set_target_properties(pointcloud_processor_lib
  PROPERTIES  
    CUDA_SEPARABLE_COMPILATION ON 
    CUDA_RESOLVE_DEVICE_SYMBOLS ON 
    POSITION_INDEPENDENT_CODE ON 
    CUDA_ARCHITECTURES "75"
    )



target_link_libraries(pointcloud_processor_lib LINK_PUBLIC Cloud_cuda map_cuda ${catkin_LIBRARIES} LINK_PUBLIC ${CUDA_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUBLAS_LIBRARIES} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED} ${PCL_LIBRARIES})

add_executable(transform_Publisher src/transform_Publisher.cpp)
target_link_libraries(transform_Publisher ${catkin_LIBRARIES})
################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  sensor_msgs   std_msgs
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
 INCLUDE_DIRS include
 LIBRARIES pointcloud_cuda_mapping
 CATKIN_DEPENDS roscpp rospy sensor_msgs std_msgs tf tf2
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations


## Declare a C++ library
# add_library(${PROJECT_NAME}
#   src/${PROJECT_NAME}/pointcloud_cuda_mapping.cpp
# )

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/pointcloud_cuda_mapping_node.cpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
# )
