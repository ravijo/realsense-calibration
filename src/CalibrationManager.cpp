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
#include <string>
#include "GL/freeglut.h"
#include "CalibrationManager.h"
#include "CustomCalibration.h"

using namespace std;
using namespace CustomCalibrator;
using namespace RsCamera;
using namespace CustomCalibWrapper;

CustomCalibration::CustomCalibration()
{
	m_rsDevice = new Rs400Device();
	m_captureStarted = false;
	m_numShot = 0;
	m_imagesCapture = false;
	m_inProcessing = false;
	m_inUpdating = false;
	m_shotFailed = false;
	m_fileOp = NULL;
	m_width = 1920;
	m_height = 1080;
	m_fps = 15;
	m_rgbWidth = 1920;
	m_rgbHeight = 1080;

	m_calibWrapper = NULL;
	m_cameraInfo.isRGB = false;
	m_cameraInfo.isWide = false;

	m_watch = new Stopwatch();
}

CustomCalibration::~CustomCalibration()
{
	if (m_captureStarted)
		m_rsDevice->StopCapture();

	delete m_rsDevice;
	delete m_watch;

}

bool CustomCalibration::Initialize()
{
	int numCameras = 2;

	InitializeGL((void *)this);

	m_cameraInfo = m_rsDevice->InitializeCamera();

	if (m_cameraInfo.name.empty()) return false;

	if (m_cameraInfo.isWide)
	{
		m_width = 1280;
		m_height = 800;
	}
	else
	{
		m_width = 1920;
		m_height = 1080;
	}

	m_fps = 15;

	m_rgbWidth = 1920;
	m_rgbHeight = 1080;

	m_leftImage = std::unique_ptr<uint8_t[]>(new uint8_t[m_width*(m_height + 1)]);
	m_rightImage = std::unique_ptr<uint8_t[]>(new uint8_t[m_width*(m_height + 1)]);
	if (m_cameraInfo.isRGB)
	{
		m_colorImage = std::unique_ptr<uint32_t[]>(new uint32_t[m_rgbWidth*(m_rgbHeight + 1)]);
		m_grayImage  = std::unique_ptr<uint8_t[]>(new uint8_t[m_rgbWidth*(m_rgbHeight + 1)]);
		numCameras += 1;
	}

	m_fileOp = new FileOp(m_cameraInfo.serial);

	m_calibWrapper = new CustomCalibrationWrapper(m_fileOp->GetOutputForlder(), 
		CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT, CHESSBOARD_SQUARE_SIZE, numCameras, NUM_SHOTS);

	return true;
}

void CustomCalibration::InitializeGL(void *parentClass)
{
	int c = 0;

	// Open a GLUT window to display point cloud
	glutInit(&c, nullptr);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Intel RealSense Custom Calibrator");

	glutSetWindowData(parentClass);
	glutDisplayFunc([]() {
		CustomCalibration *pCalib = reinterpret_cast<CustomCalibration *>(glutGetWindowData());
		pCalib->OnDisplay();
	});
	glutIdleFunc([]() {
		CustomCalibration *pCalib = reinterpret_cast<CustomCalibration *>(glutGetWindowData());
		pCalib->OnIdle();
	});
	glutReshapeFunc([](int width, int height) {
		glViewport(0, 0, width, height);
	});
	glutCloseFunc([]() {
		CustomCalibration *pCalib = reinterpret_cast<CustomCalibration *>(glutGetWindowData());
		pCalib->OnClose();
	});
	glutKeyboardFunc([](unsigned char key, int x, int y) {
		CustomCalibration *pCalib = reinterpret_cast<CustomCalibration *>(glutGetWindowData());
		pCalib->OnKeyBoard(key, x, y);
	});

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);


	int winWidth = glutGet(GLUT_WINDOW_WIDTH);
	int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, winWidth, winHeight);

	glGenTextures(3, m_textures);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(3.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CustomCalibration::Start(void)
{
	if (m_captureStarted) return;

	m_captureStarted = true;
	m_rsDevice->EnableEmitter(0.0f);
	m_rsDevice->EnableAutoExposure(1.0f);
	if (m_cameraInfo.isWide)
		m_rsDevice->SetAeControl(800);
	else
		m_rsDevice->SetAeControl(500);

	m_rsDevice->SetMediaMode(m_width, m_height, m_fps, m_rgbWidth, m_rgbHeight, m_cameraInfo.isRGB);

	m_rsDevice->StartCapture([&](const void *leftImage, const void *rightImage,
		const void *colorImage, const uint64_t timeStamp) 
	{
		uint8_t *left = m_leftImage.get();
		uint8_t *right = m_rightImage.get();
		if (m_cameraInfo.isWide)
		{
			ConvertLuminance16ToLuminance8((uint16_t *)leftImage, m_width, m_height, left);
			ConvertLuminance16ToLuminance8((uint16_t *)rightImage, m_width, m_height, right);
		}
		else
		{
			Demosaic16ToLuminance8((uint16_t *)leftImage, m_width, m_height, left);
			Demosaic16ToLuminance8((uint16_t *)rightImage, m_width, m_height, right);
		}

		if (colorImage != nullptr)
		{
			uint32_t *color = m_colorImage.get();
			ConvertYUY2ToRGBA((uint8_t *)colorImage, m_rgbWidth, m_rgbHeight, (uint8_t*)color);
		}

		if (m_imagesCapture && m_numShot < NUM_SHOTS)
		{
			m_imagesCapture = false;

			m_inProcessing = true;
			glutPostRedisplay();

			m_fileOp->SaveFrameToFile((uint8_t *)leftImage, (uint8_t *)rightImage, (uint8_t *)colorImage,
				m_width, m_height, m_rgbWidth, m_rgbHeight);

			bool rt = m_calibWrapper->AddImage(left, m_width, m_height, m_width, 0, m_numShot);
			if (rt)
			{
				rt = m_calibWrapper->AddImage(right, m_width, m_height, m_width, 1, m_numShot);
				if (rt && m_cameraInfo.isRGB && colorImage != NULL)
				{
					uint8_t *gray = m_grayImage.get();
					ConvertYUY2ToLuminance8((uint8_t *)colorImage, m_rgbWidth, m_rgbHeight, gray);
					rt = m_calibWrapper->AddImage(gray, m_rgbWidth, m_rgbHeight, m_rgbWidth, 2, m_numShot);
				}
			}

			m_inProcessing = false;
			if (rt)
			{
				m_numShot += 1;
			}
			else
			{
				m_watch->Restart();
				m_shotFailed = true;
			}

			if (m_numShot == NUM_SHOTS)
			{
				m_inUpdating = true;
				glutPostRedisplay();

				rt = m_calibWrapper->CalculateCalibration(m_width, m_height, m_rgbWidth, m_rgbHeight);
				if (rt == 0)
                    printf("Calibration successed.\n");
				else
                    printf("Calibration failed.\n");

				m_inUpdating = false;
				glutLeaveMainLoop();
			}
		}

		glutPostRedisplay();
	});
}

void CustomCalibration::Demosaic16ToLuminance8(const uint16_t* image, int width, int height, uint8_t* output)
{
	// init
	int w = width;
	int w1 = w - 1;

	int h = height;
	int h1 = h - 1;

	const uint16_t * p = nullptr;
	const uint16_t * pu = nullptr;
	const uint16_t * pd = nullptr;
	uint8_t * q = nullptr;

	int i = 0;
	int j = 0;

	// First Row from 1 to w1
	p = image + 1;
	pd = p + w;
	q = output + 1;
	for (i = 1; i < w1; ++i)
	{
		*(q++) = (uint8_t)(((((*(pd - 1) + *(pd + 1) + *(p - 1) + *(p + 1)) << 1) + ((*pd + *p) << 2)) >> 4) >> 8);
		++p;
		++pd;
	}

	// Row 1 to h1 from 1 to w1
	pu = image + 1;
	p = pu + w;
	pd = p + w1;
	q = output + w + 1;

	for (j = 1; j < h1; ++j)
	{
		for (i = 1; i < w1; ++i)
		{
			*(q++) = (uint8_t)(((*(pu - 1) + *(pu + 1) + *(pd - 1) + *(pd + 1) + (((*pu + *(p - 1) + *(p + 1) + *pd)) << 1) + (*p << 2)) >> 4) >> 8);
			++pu;
			++p;
			++pd;
		}

		pu += 2;
		p += 2;
		pd += 2;
		q += 2;
	}

	// Last Row from 1 to w1
	pu = image + (h1 - 1) * w + 1;
	p = pu + w;
	q = output + h1 * w + 1;
	for (i = 1; i < w1; ++i)
	{
		*(q++) = (uint8_t)(((((*(pu - 1) + *(pu + 1) + *(p - 1) + *(p + 1)) << 1) + ((*pu + *p) << 2)) >> 4) >> 8);
		++pu;
		++p;
	}

	// (0, 0)
	p = image;
	pd = image + w;
	q = output;
	*q = (uint8_t)(((*(pd + 1) + *(p + 1) + *pd + *p) >> 2) >> 8);

	// (h1, 0)
	pu = image + (h1 - 1) * w;
	p = pu + w;
	q = output + h1 * w;
	*q = (uint8_t)(((*(pu + 1) + *(p + 1) + *pu + *p) >> 2) >> 8);

	// Column 0 with row from 1 to h1
	pu = image;
	p = image + w;
	pd = p + w;
	q = output + w;
	for (j = 1; j < h1; ++j)
	{
		*q = (uint8_t)(((((*(pu + 1) + *(pd + 1) + *pu + *pd) << 1) + ((*(p + 1) + *p) << 2)) >> 4) >> 8);
		pu += w;
		p += w;
		pd += w;
		q += w;
	}

	// (0, w1)
	p = image + w1;
	pd = p + w;
	q = output + w1;
	*q = (uint8_t)(((*(pd - 1) + *(p - 1) + *pd + *p) >> 2) >> 8);

	// (h1, w1)
	pu = image + h1 * w - 1;
	p = pu + w;
	q = output + h * w - 1;
	*q = (uint8_t)(((*(pu - 1) + *(p - 1) + *pu + *p) >> 2) >> 8);

	// Column w1 with row from 1 to h1
	pu = image + w1;
	p = pu + w;
	pd = p + w;
	q = output + w1 + w;
	for (j = 1; j < h1; ++j)
	{
		*q = (uint8_t)(((((*(pu - 1) + *(pd - 1) + *pu + *pd) << 1) + ((*(p - 1) + *p) << 2)) >> 4) >> 8);
		pu += w;
		p += w;
		pd += w;
		q += w;
	}
}


void CustomCalibration::ConvertLuminance16ToLuminance8(const uint16_t* image, int width, int height, uint8_t* output)
{
	auto ptr = output;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			uint8_t val = (uint8_t)(image[i * width + j] >> 8);
			*ptr++ = val;
		}
	}
}

void CustomCalibration::ConvertYUY2ToLuminance8(const uint8_t* image, int width, int height, uint8_t* output)
{
	auto out = output;
	auto in = image;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j += 2)
		{
			*out++ = in[0];
			*out++ = in[2];
			in += 4;
		}
	}
}

void CustomCalibration::OnDisplay(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render image
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);

	if (m_cameraInfo.isRGB)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		uint8_t *color = (uint8_t*)m_colorImage.get();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_rgbWidth, m_rgbHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 1); glVertex2d(-1.0, -1.0);
		glTexCoord2d(0, 0); glVertex2d(-1.0, 1.0);
		glTexCoord2d(1, 0); glVertex2d(1.0, 1.0);
		glTexCoord2d(1, 1); glVertex2d(1.0, -1.0);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	uint8_t *left = m_leftImage.get();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, left);

	glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex2d(-1.0, -1.0);
	glTexCoord2d(0, 0); glVertex2d(-1.0, -0.5);
	glTexCoord2d(1, 0); glVertex2d(-0.5, -0.5);
	glTexCoord2d(1, 1); glVertex2d(-0.5, -1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, m_textures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	uint8_t *right = m_rightImage.get();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, right);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1); glVertex2d(-0.5, -1.0);
	glTexCoord2d(0, 0); glVertex2d(-0.5, -0.5);
	glTexCoord2d(1, 0); glVertex2d(0, -0.5);
	glTexCoord2d(1, 1); glVertex2d(0, -1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	COLOR color = { 0xFF, 0x45, 0, 0xFF };
	if (m_inProcessing || m_inUpdating || m_shotFailed)
	{
		if (m_inProcessing || m_inUpdating)
		{
			color.G = 0xff;
			color.R = 0;
			if (m_inProcessing)
				RenderText(-5, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Processing image ...", color);
			else
				RenderText(-5, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Processing calibration ...", color);
		}
		else
		{
			RenderText(-5, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Corner detection failed. Please retake the shot", color);

            if (m_watch->ElapsedMilliseconds() > 2000)
			{
				m_shotFailed = false;
			}
		}
	}
	else
	{
		std::vector<POINT_D> points;
		points.push_back({ -1.0, 0.0 });
		points.push_back({ 1.0, 0.0 });

		points.push_back({ 0.0, 1.0 });
		points.push_back({ 0.0, -1.0 });

		points.push_back({ -1.0, -0.75 });
		points.push_back({ -0.502, -0.75 });

		points.push_back({ -0.498, -0.75 });
		points.push_back({ 0.0, -0.75 });

		points.push_back({ -0.75, -0.5 });
		points.push_back({ -0.75, -1.0 });

		points.push_back({ -0.25, -0.5 });
		points.push_back({ -0.25, -1.0 });

		RenderLines(points, color, 2);
		color.B = 0;
		color.G = 0xFF;
		color.R = 0;
		string action = "Position camera and hit enter key to take image";
		RenderText(-5, 0, GLUT_BITMAP_TIMES_ROMAN_24, action.c_str(), color);
	}

	color.R = 0xFF;
	color.G = 0xFF;
	color.B = 0xFF;
	string numCaptured = to_string(m_numShot) + " / " + to_string(NUM_SHOTS);
	RenderText(-3, -0.92, GLUT_BITMAP_TIMES_ROMAN_24, numCaptured.c_str(), color);

	RenderText(-2, 0.85, GLUT_BITMAP_TIMES_ROMAN_24, m_cameraInfo.name.c_str(), color);
	string fwVersion = "FW Version: " + m_cameraInfo.fw_ver;
	RenderText(-3, 0.85, GLUT_BITMAP_TIMES_ROMAN_24, fwVersion.c_str(), color);
	string serial = "Serial Number: " + m_cameraInfo.serial;
	RenderText(-3, 0.75, GLUT_BITMAP_TIMES_ROMAN_24, serial.c_str(), color);

	glutSwapBuffers();
}

void CustomCalibration::OnIdle(void)
{
}

void CustomCalibration::OnKeyBoard(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'q':
	case 27:   //ESC
		if (m_captureStarted)
		{
			m_rsDevice->StopCapture();
		}
		glutLeaveMainLoop();
		break;
	case 13:  //Enter
		m_imagesCapture = true;
		break;
	}
}

void CustomCalibration::OnClose(void)
{
	if (m_captureStarted)
		m_rsDevice->StopCapture();

	m_captureStarted = false;
}

void CustomCalibration::ConvertYUY2ToRGBA(const uint8_t* image, int width, int height, uint8_t* output)
{
	int n = width*height;
	auto src = image;
	auto dst = output;
	for (; n; n -= 16, src += 32)
	{
		int16_t y[16] = {
			src[0], src[2], src[4], src[6],
			src[8], src[10], src[12], src[14],
			src[16], src[18], src[20], src[22],
			src[24], src[26], src[28], src[30],
		}, u[16] = {
			src[1], src[1], src[5], src[5],
			src[9], src[9], src[13], src[13],
			src[17], src[17], src[21], src[21],
			src[25], src[25], src[29], src[29],
		}, v[16] = {
			src[3], src[3], src[7], src[7],
			src[11], src[11], src[15], src[15],
			src[19], src[19], src[23], src[23],
			src[27], src[27], src[31], src[31],
		};

		uint8_t r[16], g[16], b[16];
		for (int i = 0; i < 16; i++)
		{
			int32_t c = y[i] - 16;
			int32_t d = u[i] - 128;
			int32_t e = v[i] - 128;

			int32_t t;
#define clamp(x)  ((t=(x)) > 255 ? 255 : t < 0 ? 0 : t)
			r[i] = clamp((298 * c + 409 * e + 128) >> 8);
			g[i] = clamp((298 * c - 100 * d - 208 * e + 128) >> 8);
			b[i] = clamp((298 * c + 516 * d + 128) >> 8);
#undef clamp
		}

		uint8_t out[16 * 4] = {
				r[0], g[0], b[0], 255, r[1], g[1], b[1], 255,
				r[2], g[2], b[2], 255, r[3], g[3], b[3], 255,
				r[4], g[4], b[4], 255, r[5], g[5], b[5], 255,
				r[6], g[6], b[6], 255, r[7], g[7], b[7], 255,
				r[8], g[8], b[8], 255, r[9], g[9], b[9], 255,
				r[10], g[10], b[10], 255, r[11], g[11], b[11], 255,
				r[12], g[12], b[12], 255, r[13], g[13], b[13], 255,
				r[14], g[14], b[14], 255, r[15], g[15], b[15], 255,
		};
#ifdef _WIN32
		memcpy_s((void *)dst, sizeof out, out, sizeof out);
#else
                memcpy((void *)dst, out, sizeof out);
#endif
		dst += sizeof out;
	}
}

void CustomCalibration::RenderLines(std::vector<POINT_D> points, COLOR color, int lineWidth)
{
	glEnable(GL_BLEND);
	glLineWidth((GLfloat)lineWidth);
	glColor4ub(color.R, color.G, color.B, color.A);

	for (int i = 0; i < points.size(); i += 2)
	{
		glBegin(GL_LINES);
		glVertex2d(points[i].x, points[i].y);
		glVertex2d(points[i+1].x, points[i+1].y);
		glEnd();
	}
	glDisable(GL_BLEND);
}

void CustomCalibration::RenderText(float x, float y, void *font, const char* text, COLOR color)
{
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	float x0 = (x + 1) * w / 2;
	float y0 = h - (y + 1) * h / 2;

	if (x == -2) x0 = 20;

	if (x == -3 || x == -5)
	{
		int nStrLengthPixels = 0;
		for (char *p = (char*)text; *p; p++)
		{
			if (*p == '\n') break;
			nStrLengthPixels += glutBitmapWidth(font, *p);
		}

		if (x == -3)
			x0 = w - nStrLengthPixels - 20;
		else
			x0 = (w - nStrLengthPixels) / 2;
	}
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, -1, +1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4ub(color.R, color.G, color.B, color.A);
	glRasterPos2f(x0, y0);

	glutBitmapString(font, (const unsigned char*)text);

	// Restore GL state to what it was prior to this call
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}
