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

#pragma once

#include <cstdint>
#include <vector>

namespace CustomCalibWrapper
{
	class CustomCalibrationWrapper
	{
	private:
		void* mData;
		std::string mOutputFolder;

	public:
		CustomCalibrationWrapper(std::string outputFolder, int chessboardWidth, int chessboardHeight, float chessboardSquareSize, int numCameras, int numImages);
		~CustomCalibrationWrapper();

		bool AddImage(uint8_t* image, int width, int height, int stride, int cameraIndex, int imageIndex);
		int CalculateCalibration(int lrWidth, int lrHeight, int rgbWidth, int rgbHeight);
	};
}
