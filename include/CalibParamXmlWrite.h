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

#include <fstream>
#include <string>
#include <cstring>

using namespace std;

namespace CalibParamXmlWrite
{

    string ParseParaName(string str)
    {
		size_t i0 = str.find_first_of(34); //find "
		size_t i1 = str.find_first_of(34, i0 + 1); //find "

		return str.substr(i0 + 1, i1 - i0 - 1);
	}

	double ParseParaValue(string str)
	{
		if (str.find("<value>") == -1) return 0;
		size_t i0 = str.find(">");
		size_t i1 = str.find("<", i0 + 1);
		if (i0 == -1 || i1 == -1) return 0;

		return atof(str.substr(i0 + 1, i1 - i0 - 1).c_str());
	}

	void WriteCustomCalibrationParametersToFile(std::string fileName, const int resolutionLeftRight[2], const double focalLengthLeft[2], const double principalPointLeft[2],
		const double distortionLeft[5], const double focalLengthRight[2], const double principalPointRight[2], const double distortionRight[5],
		const double rotationLeftRight[9], const double translationLeftRight[3], const bool hasRGB, const int resolutionRGB[2],
		const double focalLengthRGB[2], const double principalPointRGB[2], const double distortionRGB[5], const double rotationRGB[9], const double translationRGB[3])
	{
		std::ofstream file;
		file.open(fileName, std::ios::out);
		file << "<?xml version=\"1.0\"?>\n";
		file << "<Config>\n";
		file << "    <param name = \"ResolutionLeftRight\">\n";
		file << "        <value>" << resolutionLeftRight[0] << "</value>\n";
		file << "        <value>" << resolutionLeftRight[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"FocalLengthLeft\">\n";
		file << "        <value>" << focalLengthLeft[0] << "</value>\n";
		file << "        <value>" << focalLengthLeft[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"PrincipalPointLeft\">\n";
		file << "        <value>" << principalPointLeft[0] << "</value>\n";
		file << "        <value>" << principalPointLeft[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"DistortionLeft\">\n";
		file << "        <value>" << distortionLeft[0] << "</value>\n";
		file << "        <value>" << distortionLeft[1] << "</value>\n";
		file << "        <value>" << distortionLeft[2] << "</value>\n";
		file << "        <value>" << distortionLeft[3] << "</value>\n";
		file << "        <value>" << distortionLeft[4] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"FocalLengthRight\">\n";
		file << "        <value>" << focalLengthRight[0] << "</value>\n";
		file << "        <value>" << focalLengthRight[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"PrincipalPointRight\">\n";
		file << "        <value>" << principalPointRight[0] << "</value>\n";
		file << "        <value>" << principalPointRight[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"DistortionRight\">\n";
		file << "        <value>" << distortionRight[0] << "</value>\n";
		file << "        <value>" << distortionRight[1] << "</value>\n";
		file << "        <value>" << distortionRight[2] << "</value>\n";
		file << "        <value>" << distortionRight[3] << "</value>\n";
		file << "        <value>" << distortionRight[4] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"RotationLeftRight\">\n";
		file << "        <value>" << rotationLeftRight[0] << "</value>\n";
		file << "        <value>" << rotationLeftRight[1] << "</value>\n";
		file << "        <value>" << rotationLeftRight[2] << "</value>\n";
		file << "        <value>" << rotationLeftRight[3] << "</value>\n";
		file << "        <value>" << rotationLeftRight[4] << "</value>\n";
		file << "        <value>" << rotationLeftRight[5] << "</value>\n";
		file << "        <value>" << rotationLeftRight[6] << "</value>\n";
		file << "        <value>" << rotationLeftRight[7] << "</value>\n";
		file << "        <value>" << rotationLeftRight[8] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"TranslationLeftRight\">\n";
		file << "        <value>" << translationLeftRight[0] << "</value>\n";
		file << "        <value>" << translationLeftRight[1] << "</value>\n";
		file << "        <value>" << translationLeftRight[2] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"HasRGB\">\n";
		file << "        <value>" << (int)hasRGB << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"ResolutionRGB\">\n";
		file << "        <value>" << resolutionRGB[0] << "</value>\n";
		file << "        <value>" << resolutionRGB[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"FocalLengthRGB\">\n";
		file << "        <value>" << focalLengthRGB[0] << "</value>\n";
		file << "        <value>" << focalLengthRGB[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"PrincipalPointRGB\">\n";
		file << "        <value>" << principalPointRGB[0] << "</value>\n";
		file << "        <value>" << principalPointRGB[1] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"DistortionRGB\">\n";
		file << "        <value>" << distortionRGB[0] << "</value>\n";
		file << "        <value>" << distortionRGB[1] << "</value>\n";
		file << "        <value>" << distortionRGB[2] << "</value>\n";
		file << "        <value>" << distortionRGB[3] << "</value>\n";
		file << "        <value>" << distortionRGB[4] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"RotationLeftRGB\">\n";
		file << "        <value>" << rotationRGB[0] << "</value>\n";
		file << "        <value>" << rotationRGB[1] << "</value>\n";
		file << "        <value>" << rotationRGB[2] << "</value>\n";
		file << "        <value>" << rotationRGB[3] << "</value>\n";
		file << "        <value>" << rotationRGB[4] << "</value>\n";
		file << "        <value>" << rotationRGB[5] << "</value>\n";
		file << "        <value>" << rotationRGB[6] << "</value>\n";
		file << "        <value>" << rotationRGB[7] << "</value>\n";
		file << "        <value>" << rotationRGB[8] << "</value>\n";
		file << "    </param>\n";
		file << "    <param name = \"TranslationLeftRGB\">\n";
		file << "        <value>" << translationRGB[0] << "</value>\n";
		file << "        <value>" << translationRGB[1] << "</value>\n";
		file << "        <value>" << translationRGB[2] << "</value>\n";
		file << "    </param>\n";
		file << "</Config>\n";
		file.close();
	}

    void ReadCustomCalibrationParametersFromFile(std::string fileName, int resolutionLeftRight[2], double focalLengthLeft[2], double principalPointLeft[2],
		double distortionLeft[5], double focalLengthRight[2], double principalPointRight[2], double distortionRight[5], double rotationLeftRight[9], double translationLeftRight[3], bool& hasRGB, int resolutionRGB[2], double focalLengthRGB[2], double principalPointRGB[2],
		double distortionRGB[5], double rotationLeftRGB[9], double translationLeftRGB[3])
    {
		string str;
		ifstream inFile;
		inFile.open(fileName);
		if (inFile.fail())
		{
			std::cout << "Open " << fileName << " failed." << std::endl;
//			delete m_dcApi;
//			return EXIT_FAILURE;
		}

		while (getline(inFile, str))
		{
			string paraName = ParseParaName(str);
			if (!paraName.empty())
			{
				if (paraName.compare("ResolutionLeftRight") == 0)
				{
					getline(inFile, str);
					resolutionLeftRight[0] = (int)ParseParaValue(str);
					getline(inFile, str);
					resolutionLeftRight[1] = (int)ParseParaValue(str);
				}
				else if (paraName.compare("FocalLengthLeft") == 0)
				{
					getline(inFile, str);
					focalLengthLeft[0] = ParseParaValue(str);
					getline(inFile, str);
					focalLengthLeft[1] = ParseParaValue(str);
				}
				else if (paraName.compare("PrincipalPointLeft") == 0)
				{
					getline(inFile, str);
					principalPointLeft[0] = ParseParaValue(str);
					getline(inFile, str);
					principalPointLeft[1] = ParseParaValue(str);
				}
				else if (paraName.compare("DistortionLeft") == 0)
				{
					getline(inFile, str);
					distortionLeft[0] = ParseParaValue(str);
					getline(inFile, str);
					distortionLeft[1] = ParseParaValue(str);
					getline(inFile, str);
					distortionLeft[2] = ParseParaValue(str);
					getline(inFile, str);
					distortionLeft[3] = ParseParaValue(str);
					getline(inFile, str);
					distortionLeft[4] = ParseParaValue(str);
				}
				else if (paraName.compare("FocalLengthRight") == 0)
				{
					getline(inFile, str);
					focalLengthRight[0] = ParseParaValue(str);
					getline(inFile, str);
					focalLengthRight[1] = ParseParaValue(str);
				}
				else if (paraName.compare("PrincipalPointRight") == 0)
				{
					getline(inFile, str);
					principalPointRight[0] = ParseParaValue(str);
					getline(inFile, str);
					principalPointRight[1] = ParseParaValue(str);
				}
				else if (paraName.compare("DistortionRight") == 0)
				{
					getline(inFile, str);
					distortionRight[0] = ParseParaValue(str);
					getline(inFile, str);
					distortionRight[1] = ParseParaValue(str);
					getline(inFile, str);
					distortionRight[2] = ParseParaValue(str);
					getline(inFile, str);
					distortionRight[3] = ParseParaValue(str);
					getline(inFile, str);
					distortionRight[4] = ParseParaValue(str);
				}
				else if (paraName.compare("RotationLeftRight") == 0)
				{
					getline(inFile, str);
					rotationLeftRight[0] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[1] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[2] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[3] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[4] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[5] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[6] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[7] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRight[8] = ParseParaValue(str);
				}
				else if (paraName.compare("TranslationLeftRight") == 0)
				{
					getline(inFile, str);
					translationLeftRight[0] = ParseParaValue(str);
					getline(inFile, str);
					translationLeftRight[1] = ParseParaValue(str);
					getline(inFile, str);
					translationLeftRight[2] = ParseParaValue(str);
				}
				else if (paraName.compare("HasRGB") == 0)
				{
					getline(inFile, str);
					hasRGB = (bool)ParseParaValue(str);
				}
				if (paraName.compare("ResolutionRGB") == 0)
				{
					getline(inFile, str);
					resolutionRGB[0] = (int)ParseParaValue(str);
					getline(inFile, str);
					resolutionRGB[1] = (int)ParseParaValue(str);
				}
				else if (paraName.compare("FocalLengthRGB") == 0)
				{
					getline(inFile, str);
					focalLengthRGB[0] = ParseParaValue(str);
					getline(inFile, str);
					focalLengthRGB[1] = ParseParaValue(str);
				}
				else if (paraName.compare("PrincipalPointRGB") == 0)
				{
					getline(inFile, str);
					principalPointRGB[0] = ParseParaValue(str);
					getline(inFile, str);
					principalPointRGB[1] = ParseParaValue(str);
				}
				else if (paraName.compare("DistortionRGB") == 0)
				{
					getline(inFile, str);
					distortionRGB[0] = ParseParaValue(str);
					getline(inFile, str);
					distortionRGB[1] = ParseParaValue(str);
					getline(inFile, str);
					distortionRGB[2] = ParseParaValue(str);
					getline(inFile, str);
					distortionRGB[3] = ParseParaValue(str);
					getline(inFile, str);
					distortionRGB[4] = ParseParaValue(str);
				}
				else if (paraName.compare("RotationLeftRGB") == 0)
				{
					getline(inFile, str);
					rotationLeftRGB[0] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[1] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[2] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[3] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[4] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[5] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[6] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[7] = ParseParaValue(str);
					getline(inFile, str);
					rotationLeftRGB[8] = ParseParaValue(str);
				}
				else if (paraName.compare("TranslationLeftRGB") == 0)
				{
					getline(inFile, str);
					translationLeftRGB[0] = ParseParaValue(str);
					getline(inFile, str);
					translationLeftRGB[1] = ParseParaValue(str);
					getline(inFile, str);
					translationLeftRGB[2] = ParseParaValue(str);
				}
			}
		}
    }

}
