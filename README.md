INTEL CONFIDENTIAL

Copyright (2018 - 2020) Intel Corporation.

This software and the related documents are Intel copyrighted materials, and your use of them is
governed by the express license under which they were provided to you ("License"). Unless the License
provides otherwise, you may not use, modify, copy, publish, distribute, disclose or transmit this
software or the related documents without Intel's prior written permission.
This software and the related documents are provided as is, with no express or implied warranties,
other than those that are expressly stated in the License.

# Intel® RealSense™ CustomCalibration

Taken from DynamicCalibrationAPI-Linux-2.11.1.0

Contains sources for a working example for calibrating depth/RGB with user custom algorithms.
* For details, please refer to the white paper:
    Intel® RealSense™ Depth Module D400 Series Custom Calibration
    * which is available on Intel® RealSense™ product website:
      https://www.intel.com/content/www/us/en/support/articles/000026725/emerging-technologies/intel-realsense-technology.html

Dependencies
------------
1. librealsense
   the examples require librealsense library on Intel platforms (Ubuntu 16.04 and Ubuntu 18.04), librealsense2-dev prebuilt package is available, see link below:
   https://github.com/IntelRealSense/librealsense/blob/master/doc/distribution_linux.md
  ```
  sudo apt-get install librealsense2-dev
  ```
2. libusb-1.0
  ```
  sudo apt-get install libusb-dev libusb-1.0-0-dev
  ```
3. libglfw, freeglut, and libpng
  ```
  sudo apt-get install libglfw3 libglfw3-dev
  sudo apt-get install freeglut3 freeglut3-dev
  ```

  Install libpng on Ubuntu 16.04
  ```
  sudo apt-get install libpng12-dev
  ```

  Install libpng on Ubuntu 18.04
  ```
  sudo apt-get install libpng-dev
  ```

Compilation
-----------
```
1. $ cd realsense-calibration
2. $ mkdir build/ && cd build
3. $ cmake ..
4. $ make
```
