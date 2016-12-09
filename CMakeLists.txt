#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  |                     https://www.maurobellone.com                          |
#  |                                                                           |
#  | Copyright (c) 2017, - All rights reserved.                                |
#  | Authors: Mauro Bellone                                                    |
#  | Released under ___ license                                                |
#  +---------------------------------------------------------------------------+ 

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project(pcl_SVM_train_classify)

find_package (PCL 1.8 REQUIRED)

include_directories (${PCL_INCLUDE_DIRS})
link_directories    (${PCL_LIBRARY_DIRS})
add_definitions     (${PCL_DEFINITIONS})

ADD_EXECUTABLE  (pcl_SVM_train_classify pcl_SVM_train_classify.cpp)

TARGET_LINK_LIBRARIES (pcl_SVM_train_classify ${PCL_LIBRARIES} )

