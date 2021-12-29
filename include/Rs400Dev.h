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

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <functional>
#include <memory>       // For shared_ptr
#include <thread>

#include "librealsense2/rs.hpp"

#ifdef _WIN32
#define MUTEX_LOCK      EnterCriticalSection
#define MUTEX_UNLOCK    LeaveCriticalSection
#else
#define MUTEX_LOCK(m)    if (0 != pthread_mutex_lock(m)) throw std::runtime_error ("pthread_mutex_lock failed")
#define MUTEX_UNLOCK(m)  if (0 != pthread_mutex_unlock(m)) throw std::runtime_error ("pthread_mutex_unlock failed")
#endif

namespace RsCamera
{
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

    enum RS_400_STREAM_TYPE
    {
        RS400_STREAM_INFRARED,
        RS400_STREAM_INFRARED2,
        RS400_STREAM_COLOR,
        RS400_STREAM_COUNT
    };

    enum GVD_FIELDS
    {
        SKU_COMPONENT = 166,
        RGB_MODE = 178,
    };

	struct camera_info
	{
		std::string name;
		std::string serial;
		std::string fw_ver;
		std::string pid;
		bool isRGB;
		bool isWide;
	};

    class Rs400Device
    {
    public:
        Rs400Device();
        virtual ~Rs400Device();

		camera_info InitializeCamera();
        bool SetMediaMode(int width, int height, int frameRate, int colorWidth, int colorHeight, bool enableColor);

        //If depth stream is enabled, otherImage will be depth image,
        //otherwise it will be right image
        void StartCapture(std::function<void(const void *leftImage, const void *colorImage,
            const void *rgbImage, const uint64_t timeStamp)> callback);
        void StopCapture();

        void EnableAutoExposure(float value);
		void EnableEmitter(float value);
		void SetAeControl(unsigned int point);

    private:
        bool GetProfile(rs2::stream_profile& profile, rs2_stream stream, int width, int height,
            int fps, int index);

    private:
        rs2::context *m_context;
        rs2::device m_device;
        rs2::sensor m_depthSensor;
		rs2::sensor m_colorSensor;

        std::function<void(const void *leftImage, const void *rightImage, const void *colorImage,
            const uint64_t timeStamp)> m_callback;

        std::vector<rs2::stream_profile> m_depthProfiles;
		std::vector<rs2::stream_profile> m_colorProfile;
        std::unique_ptr<uint16_t[]> m_lrImage[2];

        void *m_pData[RS400_STREAM_COUNT + 1];
        uint64_t m_timestamp[RS400_STREAM_COUNT + 1];
        uint64_t m_ts;

#ifdef _WIN32
        CRITICAL_SECTION m_mutex;
#else
        pthread_mutex_t  m_mutex;
#endif
        bool m_stopProcessFrame;
        bool m_captureStarted;
        bool m_bColorEnabled;
    };
}

