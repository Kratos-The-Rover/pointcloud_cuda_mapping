# pointcloud_cuda_mapping
CUDA Optimized PointCloud based Mapping Repository
## Introduction:
An Elevation mapping package using capabilities of GPU, Pointclouds and No-Copy Message Publishing from ROS.
## Purpose:
<ul>
<li>Use Stereocamera-generated RGB Pointclouds and the PointCloud Library as mediums of perception for mapping the environment
<li> Understand and use ROS Nodelets. 
<li> Identify parallelizable parts of code.
<li > Use appropriate CUDA Libraries achieve desired parallelization.
</ul>

## Proposed Solution:
<ol>
<li> Use ROS Tf2 package for synchronization of odom/map-baselink transfroms.
<li> Use ROS Nodelets for zero copy message passing between nodes.
<li> Use Linear transformation to bring Pointcloud from Rover to Map frame.
<li> Use cubalsSgemm() to accelerate the matrix multiplication.
<li> Use grid_map_msgs for making the elevation map.
</ol>

## Testing:
Testing is has been done on Gazebo Simulations using the <a href="https://github.com/EuropeanRoverChallenge/ERC-Remote-Navigation-Sim">package</a> provided by the European Rover Challenge 2022.  The Module was also tested using ZED2i Stereocamera at 1920*1080 resolution.
The results can be seen <a href="https://youtu.be/3rKJc1q7uPE?t=206">here</a>
