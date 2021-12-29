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

#include "GL/freeglut.h"

#include "CalibrationManager.h"

using namespace CustomCalibrator;

int main(int argc, char * argv[])
{
    CustomCalibration *customCalib = new CustomCalibration();
    if (!customCalib->Initialize())
    {
        delete customCalib;
        return EXIT_FAILURE;
    }

    customCalib->Start();
    glutMainLoop();

    delete customCalib;
    return EXIT_SUCCESS;
}



