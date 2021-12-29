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

#include <chrono>
#include "Rs400Dev.h"
#include "CustomCalibrationWrapper.h"
#include "FileOp.h"

namespace CustomCalibrator
{
	const int NUM_SHOTS = 6;
	const int CHESSBOARD_HEIGHT = 6;
	const int CHESSBOARD_WIDTH = 9;
	const float CHESSBOARD_SQUARE_SIZE = 40.0f;

	typedef struct tagCOLOR
	{
		int R;
		int G;
		int B;
		int A;
	} COLOR;

	typedef struct tagPOINT_D
	{
		double x;
		double y;
	} POINT_D;

	class Stopwatch
	{
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::milliseconds milliseconds;

		clock::time_point start_;

	private:
		milliseconds intervalMs(const clock::time_point& t1, const clock::time_point& t0)
		{
			return std::chrono::duration_cast<milliseconds>(t1 - t0);
		}

		clock::time_point now()
		{
			return clock::now();
		}

	public:
		Stopwatch() : start_(clock::now()) {}

		void Start()
		{
			start_ = clock::now();
		}

		clock::time_point Restart()
		{
			start_ = clock::now();
			return start_;
		}

		double ElapsedMilliseconds()
		{
			return 1.0 * intervalMs(now(), start_).count();
		}
	};

	class CustomCalibration
	{
	public:
		CustomCalibration();
		virtual ~CustomCalibration();

		/* Initialization */
		bool Initialize(void);

		void Start(void);

	private:
		void InitializeGL(void *parentClass);

		void OnDisplay(void);
		void OnIdle(void);
		void OnKeyBoard(unsigned char key, int x, int y);
		void OnClose(void);

		void RenderLines(std::vector<POINT_D>, COLOR color, int lineWidth);
		void RenderText(float x, float y, void *font, const char* string, COLOR color);

		void Demosaic16ToLuminance8(const uint16_t* image, int width, int height, uint8_t* output);
		void ConvertLuminance16ToLuminance8(const uint16_t* image, int width, int height, uint8_t* output);
		void ConvertYUY2ToLuminance8(const uint8_t* image, int width, int height, uint8_t* output);
		void ConvertYUY2ToRGBA(const uint8_t* image, int width, int height, uint8_t* output);

	private:
		RsCamera::Rs400Device *m_rsDevice;
		RsCamera::camera_info m_cameraInfo;
		CustomCalibWrapper::CustomCalibrationWrapper* m_calibWrapper;
		FileOp* m_fileOp;

		std::unique_ptr<uint8_t[]> m_leftImage;
		std::unique_ptr<uint8_t[]> m_rightImage;
		std::unique_ptr<uint32_t[]> m_colorImage;
		std::unique_ptr<uint8_t[]> m_grayImage;

		int m_width;
		int m_height;
		int m_fps;
		int m_rgbWidth;
		int m_rgbHeight;

		bool m_imagesCapture;
		bool m_captureStarted;
		int  m_numShot;
		bool m_inProcessing;
		bool m_inUpdating;
		bool m_shotFailed;

		Stopwatch *m_watch;
		GLuint m_textures[3];
	};
}
