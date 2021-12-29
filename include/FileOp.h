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

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif  
#include <stdlib.h>  
#include <stdio.h>

using namespace std;

namespace CustomCalibrator
{
    class FileOp
    {
    public:
        FileOp(string serial) { 
            mIndex = 0;

            struct stat info;
            if (stat("../CustomResult/", &info) != 0)
            {
#ifdef _WIN32
                _mkdir("../CustomResult/");
#else
                mkdir("../CustomResult/", S_IRUSR | S_IWUSR | S_IXUSR);
#endif
            }

            mDir = "../CustomResult/" + serial;
            if (stat(mDir.c_str(), &info) != 0)
            {
#ifdef _WIN32
                _mkdir(mDir.c_str());
#else
                mkdir(mDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
            }

            mDir = mDir + "/DC";
            for (int i = 1; ; i++)
            {
                string DC = mDir + to_string(i);
                struct stat info;

                if (stat(DC.c_str(), &info) != 0)
                {
                    mDir = DC;
                    break;
                }
            }

#ifdef _WIN32
            _mkdir(mDir.c_str());
#else
            mkdir(mDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
        }

        ~FileOp() {};

        string GetOutputForlder() { return mDir; }

        void SaveFrameToFile(uint8_t * leftImage, uint8_t * rightImage, uint8_t * colorImage,
			int lrWidth, int lrHeight, int colorWidth, int colorHeight)
        {
            mIndex += 1;
            stringstream ss;
            ss << std::setw(3) << std::setfill('0') << mIndex;

            string leftFileName = mDir + "/leftImage" + ss.str() + ".bin";
			WriteBinFile(leftFileName.c_str(), leftImage, lrWidth * lrHeight * sizeof(uint16_t));

            string rightFileName = mDir + "/rightImage" + ss.str() + ".bin";
			WriteBinFile(rightFileName.c_str(), rightImage, lrWidth * lrHeight * sizeof(uint16_t));

            if (colorImage != nullptr)
            {
				string colorFileName = mDir + "/colorImage" + ss.str() + ".bin";
				WriteBinFile(colorFileName.c_str(), (uint8_t *)colorImage, colorWidth * colorHeight * sizeof(uint16_t));
            }
        }

    private:
		void WriteBinFile(const char *fileName, uint8_t *image, int filesize)
		{
			ofstream myFile(fileName, ios::out | ios::binary);
			myFile.write((const char*)image, filesize);
			myFile.close();
		}

        int mIndex;
        string mDir;
    };
}
