/*
**********************************************************************************************************
*                                                                                                       **
* INTEL CONFIDENTIAL                                                                                    **
* Copyright (2018 - 2020) Intel Corporation.                                                            **
* This software and the related documents are Intel copyrighted materials, and your use of them is      **
* governed by the express license under which they were provided to you ("License"). Unless the License **
* provides otherwise, you may not use, modify, copy, publish, distribute, disclose or transmit this     **
* software or the related documents without Intel's prior written permission.                           **
* This software and the related documents are provided as is, with no express or implied warranties,    **
* other than those that are expressly stated in the License.                                            **
*                                                                                                       **
**********************************************************************************************************
*/

#ifndef CUSTOMCALIBRATION_H_
#define CUSTOMCALIBRATION_H_

#include <opencv2/opencv.hpp>

// Sample code that detects the chessboard from an image using OpenCV
bool DetectChessboard(const cv::Mat& image, const cv::Size& chessboardSize, std::vector<cv::Point2f>& corners);

// Sample code how to calculate depth camera calibration using OpenCV
double CalibrateDepthCamera(const std::vector<std::vector<cv::Point2f> >& cornersLeft, const std::vector<std::vector<cv::Point2f> >& cornersRight, const cv::Size& chessboardSize, float checkerSize, const cv::Size& imageSizeLR, cv::Mat& Kl, cv::Mat& Dl, cv::Mat& Kr, cv::Mat& Dr, cv::Mat& Rlr, cv::Mat& Tlr);

// Sample code how to calculate RGB camera calibration using OpenCV (this assumes good depth camera calibration)
double CalibrateRGBCamera(const std::vector<std::vector<cv::Point2f> >& cornersLeft, const std::vector<std::vector<cv::Point2f> >& cornersRGB, const cv::Size& chessboardSize, float checkerSize, const cv::Size& imageSizeRGB, const cv::Mat& Kl, const cv::Mat& Dl, cv::Mat& Kc, cv::Mat& Dc, cv::Mat& Rlc, cv::Mat& Tlc);

// Sample code how to recalculate RGB camera calibration extrinsics using OpenCV (this assumes good depth camera calibration and instrinsics of the RGB camera)
double RecalibrateRGBCamera(const std::vector<std::vector<cv::Point2f> >& cornersLeft, const std::vector<std::vector<cv::Point2f> >& cornersRGB, const cv::Size& chessboardSize, float checkerSize, const cv::Mat& Kl, const cv::Mat& Dl, const cv::Mat& Kc, const cv::Mat& Dc, cv::Mat& Rlc, cv::Mat& Tlc);

#endif //CUSTOMCALIBRATION_H_
