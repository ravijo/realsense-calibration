cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(CustomCalibrationTest)

find_package(OpenCV REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(realsense2 REQUIRED)

if(NOT realsense2_FOUND)
    message(FATAL_ERROR "\n\n Intel RealSense SDK 2.0 is missing, please install it from https://github.com/IntelRealSense/librealsense/releases\n\n")
endif()

include_directories(
    include
)

add_executable(${PROJECT_NAME}
    "src/Main.cpp"
    "src/Rs400Dev.cpp"
    "src/CalibrationManager.cpp"
    "src/CustomCalibration.cpp"
    "src/CustomCalibrationWrapper.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_RELEASE "${PROJECT_NAME}"
)

include_directories(${OpenCV_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${realsense2_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} 
    ${OPENGL_LIBRARIES} 
    ${GLUT_LIBRARY} 
    ${realsense2_LIBRARY}
)

