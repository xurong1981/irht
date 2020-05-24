#include "StdAfx.h"
#include "ImageClass.h"

ImageClass::ImageClass(void)
{
	m_in_img = m_out_img = m_filter_img = m_edge_img = NULL;
	m_width = m_height = m_step = m_totalPixel = 0;
	m_in_img_data = NULL;

	//K-mean
	m_skeletonFlag = FALSE;
	m_headDetectionFlag = FALSE;
	m_iterativeTimes = 0;
	m_classSum = 0;
	m_classCounter = NULL;
	m_mean = m_variance = NULL;
	m_classes = m_binaryMark = NULL;
	m_inputBackgroundType = 0;

	//fcm
	m_u = NULL;

	// IRHT
	m_out_img_bright_part = m_skeleton = m_out_img_ellipse = NULL;
	m_brightPointSum = m_brightPointNumROI = 0;
	m_brightPoints = m_brightPointsROI = NULL;
	m_polarOrigin.x = m_polarOrigin.y = 0;
	m_K = m_ellipseParaNum = m_iterativeTimesIRHT = m_exeTimes = m_samplesNum = 0;
	m_xcNum = m_ycNum = m_aNum = m_bNum = m_phiNum = 0;
	m_pointNumOnFinalEllipse = 0;
	m_xc = m_yc = m_a = m_b = NULL;
	m_phi = NULL;
	m_xcCounter = m_ycCounter = m_aCounter = m_bCounter = m_phiCounter = m_ellipseParaCounter = NULL;
	m_ellipsePara = NULL;

	m_cEst.xc = m_cEst.yc = m_cEst.a = m_cEst.b = 0;
	m_cEst.phi = 0.0;
	m_cEst.u = m_cEst.v = m_cEst.r = m_cEst.s = m_cEst.t = 0.0;
	m_cEstPre.xc = m_cEstPre.yc = m_cEstPre.a = m_cEstPre.b = 0;
	m_cEstPre.phi = 0.0;
	m_cEstPre.u = m_cEstPre.v = m_cEstPre.r = m_cEstPre.s = m_cEstPre.t = 0.0;

	m_roiRegion.width = m_roiRegion.height = 0;

	// Snake
	startPoints = NULL;
}

ImageClass::~ImageClass(void)
{
	delete[]m_folderPath;
	delete[]m_filePath;
	cvReleaseImage(&m_in_img);
	cvReleaseImage(&m_out_img);
	cvReleaseImage(&m_out_img_bright_part);
	cvReleaseImage(&m_skeleton);
	cvReleaseImage(&m_out_img_ellipse);
}

void ImageClass::SetSkeletonFlag(bool flag)
{
	m_skeletonFlag = flag;
}

void ImageClass::SetHeadDectionFlag(bool flag)
{
	m_headDetectionFlag = flag;
}

IplImage * ImageClass::LoadImg(int biFilterFlag)
{
	int i, j;

	if (m_in_img = cvLoadImage(m_filePath, CV_LOAD_IMAGE_GRAYSCALE))           // set new values(CV_LOAD_IMAGE_GRAYSCALE | CV_LOAD_IMAGE_COLOR)
	{
		m_filter_img = cvCloneImage(m_in_img);

		if (biFilterFlag)
		{
			// Gaussian filter
			/*IplImage *filter1 = cvCloneImage(m_in_img);
			cvSmooth(m_in_img, filter1, CV_GAUSSIAN, 5, 5);

			IplImage *filter2 = cvCloneImage(m_in_img);
			cvSmooth(filter1, filter2, CV_GAUSSIAN, 5, 5);*/

			// bilateral filter
			IplImage *filter3 = cvCloneImage(m_in_img);
			cvSmooth(m_in_img, m_filter_img, CV_BILATERAL, 5, 0, 50, 50);

			/*IplImage *filter4 = cvCloneImage(m_in_img);
			cvSmooth(filter3, filter4, CV_BILATERAL, 5, 0, 50, 50);*/
			
			//cvSmooth(filter3, m_filter_img, CV_BILATERAL, 5, 0, 50, 50);

			cvNamedWindow("BilateralFilter", 1);
			cvShowImage("BilateralFilter", m_filter_img);
			cvSaveImage("Input\\BilateralFilter.png", m_filter_img);
		}

		if (m_skeletonFlag) // Tophat
		{
			// white top-hat
			IplImage* temp = 0;
			IplConvKernel* element = 0;

			temp = cvCloneImage(m_filter_img);
			//element = cvCreateStructuringElementEx(7, 7, 0, 0, CV_SHAPE_RECT, 0); // 11
			element = cvCreateStructuringElementEx(11, 11, 0, 0, CV_SHAPE_RECT, 0);

			cvMorphologyEx(m_filter_img, m_filter_img, temp, element, CV_MOP_TOPHAT, 1);
			cvNamedWindow("Tophat", 1);
			cvShowImage("Tophat", m_filter_img);
			cvSaveImage("Input\\Tophat.png", m_filter_img);
		}

		m_width = m_in_img->width;
		m_height = m_in_img->height;
		m_step = m_in_img->widthStep / sizeof(uchar);
		m_totalPixel = m_width * m_height;

		m_polarOrigin.x = m_width/2;
		m_polarOrigin.y = m_height/2;
		m_roiRegion.width = m_width;
		m_roiRegion.height = m_height;

		//allocate memory for in_image_data
		if (m_in_img_data == NULL)
		{
			m_in_img_data = new int* [m_height];
			for (i = 0; i < m_height; ++i)
				m_in_img_data[i] = new int[m_width];
		}
		else
		{
			for (i = 0; i < m_height; ++i)
				delete [] m_in_img_data[i];
			delete m_in_img_data;
			m_in_img_data = NULL;

			m_in_img_data = new int* [m_height];
			for (i = 0; i < m_height; ++i)
				m_in_img_data[i] = new int[m_width];
		}

		unsigned char *in_data = (uchar *)m_filter_img->imageData;
		for (i = 0; i < m_height; ++i)
			for (j = 0; j < m_width; ++j)
				m_in_img_data[i][j] = in_data[i*m_step + j];
	}
	else
		AfxMessageBox(_T("LoadImg Failed!"));

	// Draw Ellipse : 135	138	87	71	1.16	6	109
	/*EllipsePara ep;
	ep.xc = 135;
	ep.yc = 138;
	ep.a = 87;
	ep.b = 71;
	ep.phi = 1.16;
	DrawDetectedEllipse(m_in_img, ep, 255);*/

	return m_in_img;
}

void ImageClass::Init(int n, int inputBackgroundType)
{
	int i, j;

	m_classSum = n;
	m_iterativeTimes = 0;
	m_inputBackgroundType = inputBackgroundType;

	if (m_classCounter != NULL)
	{
		delete [] m_classCounter;
		delete [] m_mean;
		delete [] m_variance;

		for (i = 0; i < m_height; ++i)
		{
			delete [] m_classes[i];
			delete [] m_binaryMark[i];
		}

		for (i = 0; i < m_totalPixel; ++i)
			delete [] m_u[i];
		delete [] m_u;

		m_classCounter = NULL;
		m_mean = m_variance = NULL;
		m_classes = m_binaryMark = NULL;
		m_u = NULL;
	}

	Allocation(n);

	for (i = 0; i < n; ++i) 
	{
		m_classCounter[i] = 0;
		m_mean[i] = m_variance[i] = 0.0;
	}

	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			m_classes[i][j] = 0;
			m_binaryMark[i][j] = 0;
		}
	}

	for (i = 0; i < m_totalPixel; ++i)
	{
		for (j = 0; j < m_classSum; ++j)
			m_u[i][j] = 0.0;
	}
}

void ImageClass::Allocation(int n)
{
	int i;

	m_classCounter = new int[n];
	m_mean = new double[n];
	m_variance = new double[n];

	m_classes = new int* [m_height];                    // allocate memory for classes
	m_binaryMark = new int* [m_height];
	for (i = 0; i < m_height; ++i)
	{
		m_classes[i] = new int[m_width];
		m_binaryMark[i] = new int[m_width];
	}

	m_u = new double * [m_totalPixel];
	for (i = 0; i < m_totalPixel; ++i)
	{
		m_u[i] = new double[m_classSum];
	}
}

void ImageClass::InitOutImg(void)
{
	m_out_img = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 1);
	m_out_img_bright_part = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 1);
	m_skeleton = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 1);
	m_out_img_ellipse = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
}

void ImageClass::CreateOutputImg(int flag)
{
	int i, j;

	unsigned char *out_data;
	out_data = (unsigned char *)malloc(m_step*m_height*sizeof(unsigned char));

	// cretae output image
	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			if (flag == 0)
			{
				if (m_inputBackgroundType == 0)  // always make output's background black
					out_data[i*m_step + j] = (unsigned char)m_classes[i][j]*255/(m_classSum-1);
				else
					out_data[i*m_step + j] = (unsigned char)(m_classSum-1-m_classes[i][j])*255/(m_classSum-1);
			}
			else
			{
				if (m_classes[i][j] == flag)  // always make output's background black
					out_data[i*m_step + j] = 255;
				else
					out_data[i*m_step + j] = 0;
			}
		}
	}

	m_out_img->imageData = (char *)out_data;
}

void ImageClass::CreateOutputEllipse(void)
{
	CvPoint center;
	CvSize axes;
	IplImage *img;

	img = cvCreateImage(cvGetSize(m_out_img_bright_part), 8, 3);

	if (m_in_img != NULL)
		cvMerge(m_in_img, m_in_img, m_in_img, NULL, m_out_img_ellipse);

	if (m_out_img_bright_part != NULL)
		cvMerge(m_skeleton, m_skeleton, m_skeleton, NULL, img);

	// draw ellipse
	center.x = m_cEst.xc;
	center.y = m_cEst.yc;
	axes.width = m_cEst.a;
	axes.height = m_cEst.b;

	//cvCircle(m_out_img_ellipse, center, 2, whiteColor, -1);
	double phi = m_cEst.phi;//180.0 - m_cEst.phi;
	cvEllipse(m_out_img_ellipse, center, axes, phi, 0, 360, CV_RGB(255, 0, 0));

	cvSaveImage("Input\\originalWithEllipse.png", m_out_img_ellipse);

	// draw rectangle
	CvPoint pt1, pt2, pt3, pt4, ptNew1, ptNew2, ptNew3, ptNew4;
	CvScalar grayColor = CV_RGB(200, 200, 200);

	pt1.x = m_polarOrigin.x-m_roiRegion.width/2;
	pt1.y = m_polarOrigin.y-m_roiRegion.height/2;
	pt2.x = m_polarOrigin.x-m_roiRegion.width/2;
	pt2.y = m_polarOrigin.y+m_roiRegion.height/2;
	pt3.x = m_polarOrigin.x+m_roiRegion.width/2;
	pt3.y = m_polarOrigin.y+m_roiRegion.height/2;
	pt4.x = m_polarOrigin.x+m_roiRegion.width/2;
	pt4.y = m_polarOrigin.y-m_roiRegion.height/2;

	RotationPoint(pt1, phi, m_polarOrigin, ptNew1);
	RotationPoint(pt2, phi, m_polarOrigin, ptNew2);
	RotationPoint(pt3, phi, m_polarOrigin, ptNew3);
	RotationPoint(pt4, phi, m_polarOrigin, ptNew4);

	cvLine(m_out_img_ellipse, ptNew1, ptNew2, grayColor);
	cvLine(m_out_img_ellipse, ptNew2, ptNew3, grayColor);
	cvLine(m_out_img_ellipse, ptNew3, ptNew4, grayColor);
	cvLine(m_out_img_ellipse, ptNew4, ptNew1, grayColor);

	cvNamedWindow("Input Image with Ellipse", 1);
	cvShowImage("Input Image with Ellipse", m_out_img_ellipse);

	// skeleton image with ellipse
	//cvCircle(img, center, 2, cvScalar(0, 0, 0), -1);
	cvEllipse(img, center, axes, phi, 0, 360, CV_RGB(255, 0, 0));
	cvSaveImage("Input\\skeletonWithEllipse.png", img);

	ptNew1.x = m_cEst.xc - m_cEst.a*1.1;
	ptNew1.y = m_cEst.yc - m_cEst.b*1.1;
	ptNew2.x = m_cEst.xc + m_cEst.a*1.1;
	ptNew2.y = m_cEst.yc - m_cEst.b*1.1;
	ptNew3.x = m_cEst.xc + m_cEst.a*1.1;
	ptNew3.y = m_cEst.yc + m_cEst.b*1.1;
	ptNew4.x = m_cEst.xc - m_cEst.a*1.1;
	ptNew4.y = m_cEst.yc + m_cEst.b*1.1;

	cvLine(img, ptNew1, ptNew2, grayColor);
	cvLine(img, ptNew2, ptNew3, grayColor);
	cvLine(img, ptNew3, ptNew4, grayColor);
	cvLine(img, ptNew4, ptNew1, grayColor);

	// two focus point
	double c, radian;
	CvPoint p1, p2, ep1, ep2, polarOrigin;

	c = sqrt((double)m_cEst.a*m_cEst.a-(double)m_cEst.b*m_cEst.b);

	radian = m_cEst.phi * CV_PI / 180.0;

	p1.x = m_cEst.xc - c;
	p1.y = m_cEst.yc;
	p2.x = m_cEst.xc + c;
	p2.y = m_cEst.yc;

	polarOrigin.x = m_cEst.xc;
	polarOrigin.y = m_cEst.yc;

	RotationPoint(p1, phi, polarOrigin, ep1);
	RotationPoint(p2, phi, polarOrigin, ep2);

	/*center.x = m_cEst.xc;
	center.y = m_cEst.yc;
	cvCircle(img, center, 2, cvScalar(255, 0, 0), 2);*/

	center.x = ep1.x;
	center.y = ep1.y;
	cvCircle(img, center, 2, cvScalar(0, 0, 255), 2);

	center.x = ep2.x;
	center.y = ep2.y;
	cvCircle(img, center, 2, cvScalar(0, 0, 255), 2);

	cvNamedWindow("EllipseWithRectangle", 1);
	cvShowImage("EllipseWithRectangle", img);
}

bool ImageClass::SaveImg(const char* filePath)
{
	return (bool)cvSaveImage(filePath, m_out_img);
}

void ImageClass::MyCannyDetection(void)
{
	double edgeThresh = 25.0;

	m_edge_img = cvCreateImage(cvGetSize(m_filter_img), IPL_DEPTH_8U, 1);
	//temp = cvCloneImage(m_filter_img);

	cvSmooth(m_filter_img, m_edge_img, CV_BLUR, 3, 3, 0);
	cvNot(m_filter_img, m_edge_img);

	cvCanny(m_filter_img, m_edge_img, edgeThresh, edgeThresh*2.0, 3);
	//cvZero();

	cvNamedWindow("Canny", 1);
	cvShowImage("Canny", m_edge_img);

	cvNot(m_edge_img, m_out_img);
	cvSaveImage("Input\\CannyEdge.png", m_out_img);
}


void ImageClass::HistogramStat(int dims, int num, int *counter, int *data, char *ch)
{
	int i;
	int size, cMax, cSum, dMax, dPeak, dTemp;
	double dPercent;
	float *hranges, zoomY;
	CvPoint original;
	CvHistogram *hist = 0;
	IplImage *histImg = 0;

	size = 0;
	cMax = 0;
	cSum = 0;
	dMax = 0;
	dPeak = 0;
	dPercent = 0.0;
	for (i = 0; i < num; ++i)
	{
		cSum += counter[i];

		if (cMax < counter[i])
		{
			cMax = counter[i];
			dPeak = data[i];
		}

		if (dMax < data[i])
			dMax = data[i];
	}

	size = dMax;
	dPercent = (double)cMax / (double)cSum;
	dTemp = dPercent * 1000.0 + 0.5;
	dPercent = (double)dTemp / 10.0;

	zoomY = (float)size/(float)cMax;
	original.x = 30;
	original.y = size + 50;

	hranges = new float[2];
	hranges[0] = 0;
	hranges[1] = dMax;

	hist = cvCreateHist(dims, &size, CV_HIST_ARRAY, &hranges, 1);
	histImg = cvCreateImage(cvSize(size+60, size+80), 8, 3);
	cvSet(histImg, CV_RGB(255, 255, 255));

	// draw percent
	CvPoint p;
	CvScalar color = cvScalar(0, 0, 0, 0);
	CvFont font;
	char chInf[50];
	chInf[0] = '\0';

	sprintf_s(chInf, "(%d, %.1f%%)", dPeak, dPercent);

	p.x = size-30;
	p.y = 20;

	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX ,0.3, 0.3);
	cvPutText(histImg, chInf, p, &font, color);

	// draw histogram
	DrawCoordinate(histImg, original, size, dPercent);
	DrawHistogram(histImg, original, num, counter, data, size, zoomY);

	char chName[20] = "Histogram";
	strcat_s(chName,ch);
	cvNamedWindow(chName, 1);
	cvShowImage(chName, histImg);
}

void ImageClass::DrawCoordinate(IplImage *img, CvPoint original, int size, double dPercent)
{
	int i, sectLength;
	double sectNum, sectSize;
	CvPoint sP, eP;
	CvScalar color = cvScalar(0, 0, 0, 0);
	CvFont font;

	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX ,0.3, 0.3);

	// x axis
	eP.x = original.x + size + 25;
	eP.y = original.y;
	cvLine(img, original, eP, color);

	sP.x = eP.x;
	sP.y = eP.y;
	eP.x = sP.x - 6;
	eP.y = sP.y - 4;
	cvLine(img, sP, eP, color);

	eP.x = sP.x - 6;
	eP.y = sP.y + 4;
	cvLine(img, sP, eP, color);

	// tag in x axis
	for (i = 0; i < size; i += 10)
	{
		if (i%50 == 0)
		{
			sP.x = original.x + i;
			sP.y = original.y;
			eP.x = original.x + i;
			eP.y = original.y + 10;
			cvLine(img, sP, eP, color);

			char ch[10];
			ch[0] = '\0';
			_itoa_s(i, ch, 10);
			eP.x = original.x + i - 5;
			eP.y = original.y + 20;
			cvPutText(img, ch, eP, &font, color);
		}
		else
		{
			sP.x = original.x+i;
			sP.y = original.y;
			eP.x = original.x+i;
			eP.y = original.y + 5;
			cvLine(img, sP, eP, color);
		}
	}

	// y axis
	eP.x = original.x;
	eP.y = original.y - size - 45;
	cvLine(img, original, eP, color);

	sP.x = eP.x;
	sP.y = eP.y;
	eP.x = sP.x - 4;
	eP.y = sP.y + 6;
	cvLine(img, sP, eP, color);

	eP.x = sP.x + 4;
	eP.y = sP.y + 6;
	cvLine(img, sP, eP, color);

	// symbols in x axis
	if (dPercent <= 5)
		sectSize = 0.5;
	else if (dPercent <= 10)
		sectSize = 1.0;
	else if (dPercent <= 15)
		sectSize = 2.0;
	else
		sectSize = 5.0;

	sectNum = dPercent/sectSize;

	sectLength = cvCeil((double)size/sectNum);

	for (i = 0; i <= cvCeil(sectNum); ++i)
	{
		sP.x = original.x;
		sP.y = original.y - (i*sectLength);
		eP.x = original.x - 5;
		eP.y = original.y - (i*sectLength);

		if (sP.y < 10)
			break;

		cvLine(img, sP, eP, color);

		char ch[10];
		ch[0] = '\0';
		if (sectSize == 0.5)
			sprintf_s(ch, "%.1f", (double)i*sectSize);
		else
			sprintf_s(ch, "%d", (int)(i*sectSize));

		eP.x = original.x - 25;
		cvPutText(img, ch, eP, &font, color);
	}
}

void ImageClass::DrawHistogram(IplImage *img, CvPoint original, int num, int *counter, int *data, int size, float zoomY)
{
	int i, w, h;
	CvPoint sP, eP;
	CvScalar color = cvScalar(0, 0, 255, 0);

	w = 1;
	h = 0;

	for (i = 0; i < num; i++)
	{
		sP.x = original.x + data[i];
		sP.y = original.y;

		eP.x = sP.x;
		eP.y = sP.y - (float)counter[i]*zoomY;

		cvLine(img, sP, eP, color, 2);
	}
}

bool ImageClass::InitClusterCenter(void)
{
	int i, j, k, counter;
	int regionSize, dRegion;
	double u, v;

	counter = 0;
	regionSize = 5;
	dRegion = 50;

	for (i = regionSize/2+1; i < (m_height-1-regionSize/2); ++i)    //––
	{
		for (j = regionSize/2+1; j < (m_width-1-regionSize/2); ++j)    //¡–
		{
			CalculateMeanAndVariance(j, i, regionSize, regionSize, u, v);

			if (counter == 0)
			{
				m_mean[counter] = u;
				m_variance[counter] = v;
				counter++;
			}
			else
			{
				for (k = 0; k < counter; ++k)
				{
					if (fabs(m_mean[k]-u) < (double)dRegion)
					{
						if (m_variance[k] > v)
						{
							m_mean[k] = u;
							m_variance[k] = v;
							break;
						}
						else
							break;
					}
				}

				if (k == counter)
				{
					if ((fabs(m_mean[k]-u) > (double)dRegion) && (counter < m_classSum))
					{
						m_mean[counter] = u;
						m_variance[counter] = v;
						counter++;
					}
				}
			}
		}
	}

	CString strValue, strNum;
	strValue = "ClusterCenter (0-255) : ";

	for (k = 0; k < counter; ++k)
	{
		strNum.Format(_T("%f"), m_mean[k]);
		strValue += strNum;
		strValue += ", ";
	}

	//AfxMessageBox(strValue);

	if (counter == m_classSum)
		return TRUE;
	else
		return FALSE;
}

void ImageClass::CalculateMeanAndVariance(int x, int y, int w, int h, double &mean, double &variance)
{
	if (m_in_img != NULL)
	{
		int i, j;
		double sum = 0.0, sum2 = 0.0;

		for (i = y-h/2; i <= y+h/2; ++i)
		{
			for (j = x-w/2; j <= x+w/2; ++j)
			{
				sum += m_in_img_data[i][j];
				sum2 += m_in_img_data[i][j] * m_in_img_data[i][j];
			}
		}

		mean = sum/(w*h);
		variance = (sum2 - (sum*sum)/(w*h))/(w*h-1);

		if (variance == 0)
			variance = 1e-10;                //1*pow(10, -10)
	}
}

void ImageClass::UpdateVariances(void)
{
	int i, j, k, *num;
	double * sum;

	num = new int[m_classSum];
	sum = new double[m_classSum];

	for (i = 0; i < m_classSum; ++i)
	{
		num[i] = 0;
		sum[i] = 0.0;
	}

	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			k = m_classes[i][j];
			sum[k] += pow((m_in_img_data[i][j] - m_mean[k]), 2.0);
			num[k]++ ;
		}
	}

	for (k = 0; k < m_classSum; ++k)
	{
		m_variance[k]  = sqrt(sum[k] / num[k]);

		if (m_variance[k] == 0)
			m_variance[k] = 1e-10;                //1*pow(10, -10)
	}

	delete [] num;
	delete [] sum;
	num = NULL;
	sum = NULL;
}

bool ImageClass::KMeans(void)
{
	int i, j, k, *totalPixel;
	double *new_mean, e;
	bool s_flag;

	e = 0.01;
	new_mean = new double[m_classSum];
	totalPixel = new int[m_classSum];

	for (i = 0; i < m_classSum; ++i)
	{
		new_mean[i] = 0.0;
		totalPixel[i] = 0;
	}

	//º∆À„æ˘÷µ
	for(i = 0; i < m_height; ++i)                               //√ø∏ˆœÛÀÿµÅE
	{
		for(j = 0; j < m_width; ++j)
		{
			k = m_classes[i][j];
			new_mean[k] += (double)m_in_img_data[i][j];
			++totalPixel[k];
		}
	}

	for (k = 0; k < m_classSum; ++k)
		new_mean[k] /= totalPixel[k];                        //”√À˘”– Ù”⁄’‚∏ˆæ€¿‡µƒµ„µƒª“∂»æ˘÷µ◊˜Œ™–¬µƒæ€¿‡÷––ƒ

	//±»Ωœ‘≠æ€¿‡÷––ƒ∫Õ–¬µƒæ€¿‡÷––ƒµƒŒÛ≤ÅEµÅE˙Ãıº˛≈–∂œ)
	s_flag = false;
	for(k = 0; k < m_classSum; ++k)
	{
		if((double)fabs(new_mean[k]-m_mean[k]) > e)
		{
			s_flag = true;
			break;
		}
	}

	if (s_flag)
		memcpy(m_mean, new_mean, m_classSum * sizeof(double));

	delete [] new_mean;
	new_mean = NULL;

	return s_flag;
}

void ImageClass::K_PixelClass()
{
	int i, j, k, result;
	double delta, temp;

	memset(m_classCounter, 0, m_classSum * sizeof(int));

	for(i = 0; i < m_height; i++)                                   //√ø∏ˆœÛÀÿµÅE
	{
		for(j = 0; j < m_width; j++)
		{
			result = 0;
			delta = fabs(m_in_img_data[i][j] - m_mean[result]);

			for(k = 1; k < m_classSum; k++)
			{
				temp = fabs(m_in_img_data[i][j] - m_mean[k]);

				if (temp < delta)
				{
					result = k;
					delta = temp;
				}
			}

			m_classes[i][j] = result;
			m_classCounter[result]++;
		}
	}
}

void ImageClass::C_PixelClass(void)
{
	int i, j, k, result;
	double u;

	memset(m_classCounter, 0, m_classSum * sizeof(int));

	for(i = 0; i < m_height; i++)                                   //√ø∏ˆœÛÀÿµÅE
		for(j = 0; j < m_width; j++)
		{
			result = 0;
			u = m_u[i*m_width + j][0];

			for(k = 1; k < m_classSum; k++)
			{
				if (u < m_u[i*m_width + j][k])
				{
					result = k;
					u = m_u[i*m_width + j][k];
				}
			}

			m_classes[i][j] = result;
			m_classCounter[result]++;
		}
}

void ImageClass::BrightPartExtraction(void)
{
	int i, j;

	for(i = 0; i < m_height; i++)
	{
		for(j = 0; j < m_width; j++)
		{
			if (m_skeletonFlag)
			{   // Sepcial for ultrasound images
				if (m_in_img_data[i][j] > (m_mean[m_classSum-1]-0.75*m_variance[m_classSum-1]))// + m_variance[m_classSum-1]
				//if (m_classes[i][j] == m_classSum-1)
				{
					m_binaryMark[i][j] = 1;
					m_out_img_bright_part->imageData[i*m_step + j] = 0;  // black color
					//m_skeleton->imageData[i*m_step+j] = 0;
					//m_brightPointSum++;
				}
				else
				{
					m_binaryMark[i][j] = 0;
					m_out_img_bright_part->imageData[i*m_step + j] = 255;
					//m_skeleton->imageData[i*m_step+j] = 255;
				}
			}
			else
			{
				if (m_classes[i][j] == (m_classSum-1))
				{
					m_binaryMark[i][j] = 1;
					m_out_img_bright_part->imageData[i*m_step + j] = 0;  // black color
					//m_skeleton->imageData[i*m_step+j] = 0;
					//m_brightPointSum++;
				}
				else
				{
					m_binaryMark[i][j] = 0;
					m_out_img_bright_part->imageData[i*m_step + j] = 255;
					//m_skeleton->imageData[i*m_step+j] = 255;
				}
			}
		}
	}

	cvNamedWindow("BrightPart", 1);
	cvShowImage("BrightPart", m_out_img_bright_part);
	cvSaveImage("Input\\BrightPart.png", m_out_img_bright_part);
}

void ImageClass::FuzzyClustering(int umatrix_flag, int means_flag, int g_width)
{
	double b, e_matrix, e_means, se_matrix, se_means;               // b > 1
	bool s_flag;

	b = 2.0;
	e_matrix = 0.1;
	e_means = 0.1;
	se_matrix = 0;
	se_means = 0;
	s_flag = true;

	while (s_flag)
	{
		//¡• Ù∂»æÿ’Ûº∆ÀÅE
		se_matrix = UpdateUmatrix(umatrix_flag, b, g_width);

		//æ€¿‡÷––ƒº∆ÀÅE
		se_means = UpdateFuzzyMeans(means_flag, b, g_width);

		//∏˘æ›æ€¿‡÷––ƒ”ÅE• Ù∂»æÿ’Ûº∆À„∑÷¿‡¿‡–Õ
		C_PixelClass();

		//º∆À„∑Ω≤ÅE
		UpdateVariances();

		// ‰≥ˆ∑÷¿‡ÕºœÅE
		CreateOutputImg(0);
		cvNamedWindow("SegmentationResults", 1);
		cvShowImage("SegmentationResults", m_out_img);

		m_iterativeTimes++;

		if ((se_matrix <= e_matrix) && (se_means <= e_means))
			s_flag = false;

		if (m_iterativeTimes >= 100)
			break;
	}
}

double ImageClass::UpdateUmatrix(int flag, double b, int g_width)
{
	int i, j, k, m;
	int isMeans;
	double d_sum, d_nk, d_nm, d;
	double newU;
	double e = 0.0;

	/* For each example in the dataset */
	for (i = 0; i < m_height; i++)    //––
	{
		for (j = 0; j < m_width; j++)    //¡–
		{
			/* Special case: If Example is equal to a Cluster Centroid,then U=1.0 for that cluster and 0 for all others */
			if ( (isMeans = IsMeans(j, i)) != -1 ) 
			{
				for (k = 0; k < m_classSum; k++) 
				{
					if (k == isMeans)
						m_u[i*m_width + j][k] = 1.0;
					else
						m_u[i*m_width + j][k] = 0.0;
				}
				continue;
			}

			/* For each class */
			for (k = 0; k < m_classSum; k++)
			{
				d_sum = 0.0;

				/* Calculate summation */
				d_nk = Dsquare_Gray(flag, i, j, k, g_width);

				for (m = 0; m < m_classSum; m++)
				{
					d_nm = Dsquare_Gray(flag, i, j, m, g_width);
					d = pow( d_nk / d_nm , (1.0/(b-1)));

					d_sum += d;
				}

				newU = 1.0/(double)d_sum;       // Weight is 1/sum

				if (e < fabs(m_u[i*m_width + j][k] - newU))
					e = fabs(m_u[i*m_width + j][k] - newU);

				m_u[i*m_width + j][k] = newU;
			}
		}
	}

	return e;
}

int ImageClass::IsMeans(int x, int y)
{
	int  c;

	for (c = 0; c < m_classSum; c++)
	{
		if (m_in_img_data[y][x] == (int)(m_mean[c]+0.5))     // very important
			return c;
	}

	return -1;
}

double ImageClass::Dsquare_Gray(int flag, int x, int y, int k, int g_width)     //i, j±˙ÊæœÒÀÿµ„µƒ◊¯±Í£¨k±˙Êæ∑÷¿‡µƒ¿‡± ˝
{
	double d_xy;

	switch (flag)
	{
	case 0 :                                             //FCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE
		d_xy = pow(fabs(m_in_img_data[x][y] - m_mean[k]), 2.0);
		break;
	}

	return d_xy;
}

double ImageClass::UpdateFuzzyMeans(int flag, double b, int g_width)
{
	int i, j, k;
	double m_sum, d_sum;
	double newMeans, e;

	e = 0;
	/* For each cluster */
	for (k = 0; k < m_classSum; k++)
	{
		m_sum = 0.0;
		d_sum = 0.0;

		for (i = 0; i < m_height; i++)
		{
			for (j = 0; j < m_width; j++)
			{
				// flag = 0 -- FCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 1 -- IFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 2 -- Sigma-IFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE
				// flag = 3 -- KFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 4 -- MKFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 5 -- Sigma-MKFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE
				// flag = 6 -- WKFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 7 -- WMKFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 8 -- Sigma-WMKFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE
				// flag = 9 -- PP-FCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE flag = 10 -- PP-KFCMÀ„∑®µƒæ‡¿ÅE∆ÀÅE 
				// flag = 19 -- FMRFÀ„∑®µƒæ‡¿ÅE∆ÀÅE
				if (m_u[i*m_width + j][k] > 0)
				{
					switch (flag)
					{
					case 0 :
						m_sum += pow(m_u[i*m_width + j][k], b) * m_in_img_data[i][j];
						d_sum += pow(m_u[i*m_width + j][k], b);
						break;
					}
				}
			}
		}

		newMeans = m_sum / d_sum;

		if (e < fabs(newMeans - m_mean[k]))
			e = fabs(newMeans - m_mean[k]);
		m_mean[k] = newMeans;
	}

	return e ;
}

void ImageClass::SkeletonizationByDistanceTransform()
{
	int i, j;
	int **mark, **skeleton;
	int **signRows, **signColumms;
	IplImage* src = 0;
	IplImage* dist = 0;
	IplImage* dist8u = 0;
	IplImage* temp = 0;
	IplConvKernel* element1 = 0;
	IplConvKernel* element2 = 0;
	IplConvKernel* element3 = 0;

	src = cvCloneImage(m_out_img_bright_part);
	dist = cvCloneImage(m_out_img_bright_part);
	dist8u = cvCloneImage(m_out_img_bright_part);
	temp = cvCloneImage(m_out_img_bright_part);

	element1 = cvCreateStructuringElementEx(1, 1, 0, 0, CV_SHAPE_RECT, 0);
	element2 = cvCreateStructuringElementEx(2, 2, 0, 0, CV_SHAPE_RECT, 0);
	element3 = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);

	// for white background img, cvDilate is to erode image.
	// Morphological Opening
	// œ»∏Ø ¥∫Û≈Ú’Õµƒπ˝≥Ã≥∆Œ™ø™‘ÀÀ„°£”√¿¥œ˚≥˝–°ŒÅEÂ°¢‘⁄œÀœ∏µ„¥¶∑÷¿ÅEÅEÂ°¢∆Ωª¨Ωœ¥ÛŒÅEÂµƒ±ﬂΩÁµƒÕ¨ ±≤¢≤ª√˜œ‘∏ƒ±‰∆‰√Êª˝°£
	cvDilate(src, dist8u, element2, 1);
	cvErode(dist8u, dist8u, element2, 1);

#if Debug1
	cvNamedWindow("imgOpening", 1);
	cvShowImage("imgOpening", dist8u);
#endif

	// Morphological Closing
	// œ»≈Ú’Õ∫Û∏Ø ¥µƒπ˝≥Ã≥∆Œ™±’‘ÀÀ„°£”√¿¥Ã˚œ‰ŒÅEÂƒ⁄œ∏–°ø’∂¥°¢¡¨Ω”¡⁄ΩÅEÅEÂ°¢∆Ωª¨∆‰±ﬂΩÁµƒÕ¨ ±≤¢≤ª√˜œ‘∏ƒ±‰∆‰√Êª˝°£
	cvErode(dist8u, dist8u, element1, 1);

	cvErode(dist8u, dist8u, element2, 1);
	cvDilate(dist8u, dist8u, element2, 1);

#if Debug1
	cvNamedWindow("imgClosing", 1);
	cvShowImage("imgClosing", dist8u);
#endif

	mark = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		mark[i] = new int[m_width];

	skeleton = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		skeleton[i] = new int[m_width];

	signRows = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		signRows[i] = new int[m_width];

	signColumms = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		signColumms[i] = new int[m_width];

	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			skeleton[i][j] = 0;
			signRows[i][j] = 0;
			signColumms[i][j] = 0;
			mark[i][j] = 0;
		}
	}

	// distance transform
	DistanceTransform(dist8u, mark);

	IplImage *imgSignRows = cvCreateImage(cvGetSize(m_out_img_bright_part), 8, 1);

	// set sign in rows
	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			if (j == (m_width-1))
				signRows[i][j] = 0;
			else
				signRows[i][j] = mark[i][j+1] - mark[i][j];

			if (signRows[i][j] == 1)
				imgSignRows->imageData[i*m_step+j] = 255;
			else if (signRows[i][j] == 0)
				imgSignRows->imageData[i*m_step+j] = 150;
			else
				imgSignRows->imageData[i*m_step+j] = 0;
		}
	}

	//cvNamedWindow("imgSignRows", 1);
	//cvShowImage("imgSignRows", imgSignRows);
	
	// set sign in columns
	for (j = 0; j < m_width; ++j)
	{
		for (i = 0; i < m_height; ++i)
		{
			if (i == (m_height-1))
				signColumms[i][j] = 0;
			else
				signColumms[i][j] = mark[i+1][j] - mark[i][j];
		}
	}

	// skeletonization : 2 (check in rows)
	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width-2; ++j)
		{
			if ((signRows[i][j] == 1) && (signRows[i][j+1] == -1))
			{
				skeleton[i][j+1] = 1;
				continue;
			}

			if ((signRows[i][j] == 1) && (signRows[i][j+1] == 0) && (signRows[i][j+2] == -1))
			{
				skeleton[i][j+1] = 1;
				continue;
			}
		}
	}

	// skeletonization : 2 (check in columns)
	for (j = 0; j < m_width; ++j)
	{
		for (i = 0; i < m_height-2; ++i)
		{
			if ((signColumms[i][j] == 1) && (signColumms[i+1][j] == -1))
			{
				skeleton[i+1][j] = 1;
				continue;
			}

			if ((signColumms[i][j] == 1) && (signColumms[i+1][j] == 0) && (signColumms[i+2][j] == -1))
			{
				skeleton[i+1][j] = 1;
				continue;
			}
		}
	}

	// output skeleton image
	m_brightPointSum =  0;

	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			if (skeleton[i][j] == 1)
			{
				m_skeleton->imageData[i*m_step+j] = 0;
				m_brightPointSum++;
			}
			else
				m_skeleton->imageData[i*m_step+j] = 255;
		}
	}

	cvNamedWindow("Skeleton", 1);
	cvShowImage("Skeleton", m_skeleton);
	cvSaveImage("Input\\Skeleton.png", m_skeleton);
}

void ImageClass::Skeletons()
{
	// output skeleton image
	int i, j;
	m_brightPointSum =  0;

	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			if (m_out_img_bright_part->imageData[i*m_step+j] == 0)
			{
				m_skeleton->imageData[i*m_step+j] = 0;
				m_brightPointSum++;
			}
			else
				m_skeleton->imageData[i*m_step+j] = 255;
		}
	}

	cvNamedWindow("Skeletons", 1);
	cvShowImage("Skeletons", m_skeleton);
}

void ImageClass::DistanceTransform(IplImage *img, int **mark)
{
	int i, j;
	int d_M, width, height;
	unsigned char *imgData, bgValue, fgValue;

	// mark maximum
	d_M = 1000;

	width = img->width;
	height = img->height;

	imgData = (uchar *)img->imageData;
	bgValue = imgData[0];
	fgValue = 255 - bgValue;

	for (i = 0; i < height; ++i)
	{
		for (j = 0; j < width; ++j)
		{
			if (imgData[i*m_step + j] == fgValue)
				mark[i][j] = d_M;
			else
				mark[i][j] = 0;
		}
	}

	// distance transform : left -> right, then up -> down
	for (i = 1; i < height; ++i)
	{
		for (j = 1; j < width; ++j)
		{
			if (mark[i][j] != 0)
				mark[i][j] = MIN(MIN(mark[i-1][j]+1, mark[i][j-1]+1), mark[i][j]);
		}
	}

	// distance exchange : right -> left, then down -> up
	for (i = height-2; i >= 0; --i)
	{
		for (j = width-2; j >= 0; --j)
		{
			if (mark[i][j] != 0)
				mark[i][j] = MIN(MIN(mark[i][j+1]+1, mark[i+1][j]+1), mark[i][j]);
		}
	}
}

void ImageClass::SkeletonizationByThinning()
{
	int i, j;
	int **convexMark, **skeleton;
	unsigned char *imgData, bgValue, fgValue;
	IplImage* src = 0;
	IplImage* dist = 0;
	IplImage* dist8u = 0;
	IplConvKernel* element = 0;

	src = cvCloneImage(m_out_img_bright_part);
	dist = cvCloneImage(m_out_img_bright_part);
	dist8u = cvCloneImage(m_out_img_bright_part);

	element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);

	// for white background img, cvDilate is to erode image.
	// Morphological Opening
	cvDilate(src, dist8u, element, 1);
	cvErode(dist8u, dist8u, element, 1);

#if Debug1
	cvNamedWindow("imgOpening", 1);
	cvShowImage("imgOpening", dist8u);
#endif

	// Morphological Closing
	cvErode(dist8u, dist8u, element, 1);
	cvDilate(dist8u, dist8u, element, 1);

#if Debug1
	cvNamedWindow("imgClosing", 1);
	cvShowImage("imgClosing", dist8u);
#endif

	convexMark = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		convexMark[i] = new int[m_width];

	skeleton = new int* [m_height];
	for (i = 0; i < m_height; ++i)
		skeleton[i] = new int[m_width];

	imgData = (uchar *)dist8u->imageData;
	bgValue = imgData[0];
	fgValue = 255 - bgValue;
	
	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			convexMark[i][j] = 0;

			if (imgData[i*m_step+j] ==fgValue)
				skeleton[i][j] = 1;
			else
				skeleton[i][j] = 0;
		}
	}

	// Hit-and-Miss Transform
	int counter, n = 0;
	do 
	{
		n++;
		counter = HitAndMissTransform(dist8u, skeleton, convexMark);

		for (i = 0; i < m_height; ++i)
		{
			for (j = 0; j < m_width; ++j)
			{
				if (convexMark[i][j] == 1)
					dist8u->imageData[i*m_step+j] = 0;
				else
					dist8u->imageData[i*m_step+j] = 255;

				skeleton[i][j] -= convexMark[i][j];
				convexMark[i][j] = 0;

				if (skeleton[i][j] == 1)
					m_skeleton->imageData[i*m_step+j] = 0;
				else
					m_skeleton->imageData[i*m_step+j] = 255;
			}
		}

		if (n > 100)
			break;
	} while (counter != 0);
	
	// output skeleton image
	for (i = 0; i < m_height; ++i)
	{
		for (j = 0; j < m_width; ++j)
		{
			if (skeleton[i][j] == 1)
				m_skeleton->imageData[i*m_step+j] = 0;
			else
				m_skeleton->imageData[i*m_step+j] = 255;
		}
	}

	cvNamedWindow("SkeletonizationByThinning", 1);
	cvShowImage("SkeletonizationByThinning", m_skeleton);
}

int ImageClass::HitAndMissTransform(IplImage *img, int **skeleton, int **convexMark)
{
	int i, j, k;
	int counter = 0;

	int templateRect[8][3][3] = {0, 0, 0,   -1, 1, -1,   1, 1, 1,
		                         1, -1, 0,   1, 1, 0,   1, -1, 0,
							     1, 1, 1,   -1, 1, -1,   0, 0, 0,
						         0, -1, 1,   0, 1, 1,   0, -1, 1,
							     -1, 0, 0,   1, 1, 0,   -1, 1, -1,
						         -1, 1, -1,   1, 1, 0,   -1, 0, 0,
							     -1, 1, -1,   0, 1, 1,   0, 0, -1,
							     0, 0, -1,   0, 1, 1,   -1, 1, -1};

	for (k = 0; k < 8; ++k)
	{
		for (i = 1; i < m_height-1; ++i)
		{
			for (j = 1; j < m_width-1; ++j)
			{
				if (skeleton[i][j] == 0)
					continue;

				if (TemplateMatching(j, i, skeleton, 3, templateRect[k]))
				{
					convexMark[i][j] = 1;
					counter++;
				}
			}
		}

		for (i = 0; i < m_height; ++i)
		{
			for (j = 0; j < m_width; ++j)
			{
				skeleton[i][j] -= convexMark[i][j];
				convexMark[i][j] = 0;
			}
		}
	}

	return counter;
}

bool ImageClass::TemplateMatching(int x, int y, int **binaryMark, int templateSize, int templateRect[3][3])
{
	int i, j;
	int mark[3][3];

	mark[0][0] = binaryMark[y-1][x-1];
	mark[0][1] = binaryMark[y-1][x];
	mark[0][2] = binaryMark[y-1][x+1];

	mark[1][0] = binaryMark[y][x-1];
	mark[1][1] = binaryMark[y][x];
	mark[1][2] = binaryMark[y][x+1];

	mark[2][0] = binaryMark[y+1][x-1];
	mark[2][1] = binaryMark[y+1][x];
	mark[2][2] = binaryMark[y+1][x+1];

	for (i = 0; i < templateSize; ++i)  // rows
	{
		for (j = 0; j < templateSize; ++j)  // columns
		{
			if (templateRect[i][j] >= 0)
			{
				if (templateRect[i][j] != mark[i][j])
					return FALSE;
			}
		}
	}

	return TRUE;
}

void ImageClass::ReadBrightPoints()
{
	int i, j, counter;
	int xMin, xMax, yMin, yMax;
	int step;
	uchar imgData;

	if (m_iterativeTimesIRHT == 0)
		return ;

	IplImage *imgROI = cvCreateImage(cvGetSize(m_in_img), 8, 1);
	cvSet(imgROI, CV_RGB(255, 255, 255));
	step = imgROI->widthStep/sizeof(uchar);

	counter = 0;
	xMin = m_polarOrigin.x - m_roiRegion.width/2;
	xMax = m_polarOrigin.x + m_roiRegion.width/2;
	yMin = m_polarOrigin.y - m_roiRegion.height/2;
	yMax = m_polarOrigin.y + m_roiRegion.height/2;

	for (int i = 0; i < m_brightPointSum; i++)
	{
		double phi = m_cEst.phi;
		CvPoint pt, ptRotation;

		pt.x = m_brightPoints[i].x;
		pt.y = m_brightPoints[i].y;

		if (m_cEst.phi == 0.0)
		{
			ptRotation.x = pt.x;
			ptRotation.y = pt.y;
		}
		else // counter-clockwise if phi > 0
			RotationPoint(pt, -m_cEst.phi, m_polarOrigin, ptRotation);

		if ((xMin <= ptRotation.x) && (ptRotation.x <= xMax) && (yMin <= ptRotation.y) && (ptRotation.y <= yMax))
		{
			
			imgROI->imageData[pt.y*step + pt.x] = 0;

			m_brightPointsROI[counter].x = pt.x;
			m_brightPointsROI[counter].y = pt.y;
			counter++;
		}
	}

	m_brightPointNumROI = counter;

	CString str, strNum;
	str = "The No. of Bright Points in ROI : ";
	strNum.Format(_T("%d"), m_brightPointNumROI);
	str += strNum;

	//AfxMessageBox(str);

	// draw ellipse
	CvPoint center;
	center.x = m_cEst.xc;
	center.y = m_cEst.yc;
	CvSize size;
	size.width = m_cEst.a;
	size.height = m_cEst.b;
	cvEllipse(imgROI, center, size, m_cEst.phi, 0, 360, CV_RGB(150, 150, 150));

	/*cvNamedWindow("imgROI", 1);
	cvShowImage("imgROI", imgROI);
	cvWaitKey(0);*/
}

// Rotate image with rotationAngle by counter-clockwise
IplImage* ImageClass::RotationImage(IplImage* img, double rotationAngle)
{
	double ratationRadian;
	IplImage *src = 0;
	IplImage *dst = 0;
	CvMat* rotationMat;     // ªÿ‹û––¡–

	src = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	dst = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	cvCopyImage(img, src);
	cvZero(dst);

	ratationRadian = (double)rotationAngle * CV_PI / 180.0;

	// ªÿ‹û––¡–˚e”Ú§Œ¥_±£
	rotationMat = cvCreateMat(2,3,CV_32FC1);

	// 30∂»§Œªÿ‹û––¡–§Ú«Û§·§ÅE
	cv2DRotationMatrix(cvPoint2D32f(m_polarOrigin.x, m_polarOrigin.y), rotationAngle, 1, rotationMat);

	// ªÿ‹ÅE
	cvWarpAffine(src, dst, rotationMat);

	return dst;
}

// Rotate point with rotationAngle by counter-clockwise
void ImageClass::RotationPoint(CvPoint p, double rotationAngle, CvPoint polarOrigin, CvPoint &pNew)
{
	double thetaRadian;
	CvMat *x, *y, *magnitude, *radian, *xNew, *yNew;

	x = cvCreateMat(1, 1, CV_64FC1);
	y = cvCreateMat(1, 1, CV_64FC1);
	xNew = cvCreateMat(1, 1, CV_64FC1);
	yNew = cvCreateMat(1, 1, CV_64FC1);

	cvmSet(x, 0, 0, p.x-polarOrigin.x);
	cvmSet(y, 0, 0, polarOrigin.y-p.y);

	magnitude = cvCreateMat(1, 1, CV_64FC1);
	radian = cvCreateMat(1, 1, CV_64FC1);

	// theta : [0, 2*CV_PI]
	cvCartToPolar(x, y, magnitude, radian, 0);
	//r = cvmGet(magnitude, 0, 0);
	thetaRadian = cvmGet(radian, 0, 0);
	thetaRadian -= rotationAngle * CV_PI / 180.0;
	cvmSet(radian, 0, 0, thetaRadian);

	cvPolarToCart(magnitude, radian, xNew, yNew, 0);
	pNew.x = cvmGet(xNew, 0, 0) + polarOrigin.x;
	pNew.y = polarOrigin.y - cvmGet(yNew, 0, 0);
}

void ImageClass::IRHT_Ini(int K)
{
	m_K = K;

	if (m_xc != NULL)
	{
		delete [] m_xc;
		delete [] m_yc;
		delete [] m_a;
		delete [] m_b;
		delete [] m_phi;
		delete [] m_xcCounter;
		delete [] m_ycCounter;
		delete [] m_aCounter;
		delete [] m_bCounter;
		delete [] m_phiCounter;
		delete [] m_ellipsePara;
		delete [] m_ellipseParaCounter;
		delete [] m_brightPoints;
		delete [] m_brightPointsROI;

		m_xc = NULL;
		m_yc = NULL;
		m_a = NULL;
		m_b = NULL;
		m_phi = NULL;
		m_ellipsePara = NULL;
		m_ellipseParaCounter = NULL;
		m_brightPoints = NULL;
		m_brightPointsROI = NULL;
	}

	m_xc = new int[m_K];
	m_yc = new int[m_K];
	m_a = new int[m_K];
	m_b = new int[m_K];
	m_phi = new double[m_K];
	m_xcCounter = new int[m_K];
	m_ycCounter = new int[m_K];
	m_aCounter = new int[m_K];
	m_bCounter = new int[m_K];
	m_phiCounter = new int[m_K];
	m_ellipsePara = new EllipsePara [m_K];
	m_ellipseParaCounter = new int[m_K];
	m_brightPoints = new CvPoint[m_brightPointSum];
	m_brightPointsROI = new CvPoint[m_brightPointSum];

	for (int i = 0; i < m_K; ++i)
	{
		m_xc[i] = m_yc[i] = m_a[i] = m_b[i] = 0;
		m_phi[i] = 0.0;
		m_xcCounter[i] = m_ycCounter[i] = m_aCounter[i] = m_bCounter[i] = m_phiCounter[i] = 0;

		m_ellipsePara[i].xc = m_ellipsePara[i].yc = m_ellipsePara[i].a = m_ellipsePara[i].b = 0;
		m_ellipsePara[i].phi = 0.0;
		m_ellipsePara[i].u = m_ellipsePara[i].v = m_ellipsePara[i].r = m_ellipsePara[i].s = m_ellipsePara[i].t = 0.0;
		m_ellipseParaCounter[i] = 0;
	}

	int counter = 0;

	for (int i = 0; i < m_height; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			uchar imgData;
			imgData = (uchar)m_skeleton->imageData[i*m_step + j];

			if ((imgData-0) < 10)
			{
				m_brightPoints[counter].x = j;
				m_brightPoints[counter].y = i;
				m_brightPointsROI[counter].x = j;
				m_brightPointsROI[counter].y = i;
				counter++;
			}
		}
	}

	m_brightPointNumROI = counter;

	m_ellipseParaNum = 0;
	m_xcNum = m_ycNum = m_aNum = m_bNum = m_phiNum = 0;

	m_cEst.xc = m_cEst.yc = m_cEst.a = m_cEst.b = 0;
	m_cEst.phi = 0.0;
	m_cEst.u = m_cEst.v = m_cEst.r = m_cEst.s = m_cEst.t = 0.0;
	m_cEstPre.xc = m_cEstPre.yc = m_cEstPre.a = m_cEstPre.b = 0;
	m_cEstPre.phi = 0.0;
	m_cEstPre.u = m_cEstPre.v = m_cEstPre.r = m_cEstPre.s = m_cEstPre.t = 0.0;

	m_polarOrigin.x = m_width/2;
	m_polarOrigin.y = m_height/2;

	m_roiRegion.width = m_width;
	m_roiRegion.height = m_height;
}

bool ImageClass::IRHT(int irhtType)
{
	int i;
	int p_i, p;

	p_i = 0;
	p = 1000;

	m_iterativeTimesIRHT = 0;
	m_pointNumOnFinalEllipse = 0;

	do 
	{
		m_ellipseParaNum = 0;
		m_xcNum = m_ycNum = m_aNum = m_bNum = m_phiNum = 0;

		for (i = 0; i < m_K; ++i)
		{
			m_xc[i] = m_yc[i] = m_a[i] = m_b[i] = 0;
			m_phi[i] = 0.0;
			m_xcCounter[i] = m_ycCounter[i] = m_aCounter[i] = m_bCounter[i] = m_phiCounter[i] = 0;

			m_ellipsePara[i].xc = m_ellipsePara[i].yc = m_ellipsePara[i].a = m_ellipsePara[i].b = 0;
			m_ellipsePara[i].phi = 0.0;
			m_ellipsePara[i].u = m_ellipsePara[i].v = m_ellipsePara[i].r = m_ellipsePara[i].s = m_ellipsePara[i].t = 0.0;
			m_ellipseParaCounter[i] = 0;
		}

		if (!RHT(irhtType))
			return FALSE;

		m_iterativeTimesIRHT++;

		if (ComparePar(m_cEst, m_cEstPre, 1))
		{
#if ConePhantomFlag
			if ((m_cEst.a > 43) || (m_cEst.b > 43)) // 43
				continue;

			//if ((m_cEst.a < 15) || (m_cEst.b < 15)) // 43
			//	continue;
#endif
			break;
		}

#if Debug4
		HistogramStat(1, m_aNum, m_aCounter, m_a, "-a");

		int *phi, *phiCounter;
		phi = new int[m_phiNum];
		phiCounter = new int [m_phiNum];

		for (i = 0; i < m_phiNum; ++i)
		{
			phi[i] = m_phi[i];
			phiCounter[i] = m_phiCounter[i];
		}

		HistogramStat(1, m_phiNum, phiCounter, phi, "-phi");
		cvWaitKey(0);
#endif

	} while (1);

	//count the number of points on detected ellipse
	m_pointNumOnFinalEllipse = CounterPointsOnEllipse(m_cEst, NULL);

	// output parameters
	WriteParameters();

	m_exeTimes++;

	if (m_exeTimes == 20)
		AfxMessageBox(_T("IRHT has been executed by 20 times!"));

	return TRUE;
}

bool ImageClass::RHT(int irhtType)
{
	int i, j;
	int p_i, p;
	int rand_num[5];
	CvPoint rand_point[5];

	p_i = 0;
	p = 1000;

	m_samplesNum = 0;
	srand((unsigned)time(0));

	ReadBrightPoints();

	do 
	{
		p_i = 0;
		do 
		{
			p_i++;

			// Choose 5 rand points
			for (i = 0; i < 5;)
			{
				bool continueFlag = FALSE;
				rand_num[i] = rand()%m_brightPointNumROI;

				for (j = 0; j < i; ++j)
				{
					if (rand_num[i] == rand_num[j])
					{
						continueFlag = TRUE;
						break;
					}
				}

				if (continueFlag)
					continue;
				else
					i++;
			}

			for (i = 0; i < 5; ++i)
			{
				rand_point[i].x = m_brightPointsROI[rand_num[i]].x;
				rand_point[i].y = m_brightPointsROI[rand_num[i]].y;
			}

			if (EstimateParameters(rand_point))  // Judge 1
			{
				m_samplesNum++;
				break;
			}

			if (p_i > p)               // No valid ellipse
				return FALSE;

		} while (1);

	} while (m_samplesNum < m_K);                                  // Judge 2

	// Save previous value
	m_cEstPre = m_cEst;

#if EllipseParaAccumulatorType

#if ImprovementFlag
	// Improvement
	int peakPos;
	peakPos = DetectPeaks(m_ellipseParaNum, m_ellipseParaCounter, PeakNum, m_ellipsePara);
	m_cEst = m_ellipsePara[peakPos];
#else
	int peakPos;
	peakPos = DetectPeak(m_ellipseParaNum, m_ellipseParaCounter);
	m_cEst = m_ellipsePara[peakPos];
#endif

#else
	// Detect peaks in accumulators
	int peakPos;
	peakPos = DetectPeak(m_xcNum, m_xcCounter);
	m_cEst.xc = m_xc[peakPos];

	peakPos = DetectPeak(m_ycNum, m_ycCounter);
	m_cEst.yc = m_yc[peakPos];

	peakPos = DetectPeak(m_aNum, m_aCounter);
	m_cEst.a = m_a[peakPos];

	peakPos = DetectPeak(m_bNum, m_bCounter);
	m_cEst.b = m_b[peakPos];

	peakPos = DetectPeak(m_phiNum, m_phiCounter);
	m_cEst.phi = m_phi[peakPos];
#endif

	// Update ROI region
	SetROI();

	//Draw Detected Ellipse
	DrawDetectedEllipse(m_skeleton, m_cEst, 180);

	return TRUE;
}

int ImageClass::DetectPeaks(int num, int *counter, int peakNum, EllipsePara *ellipsePara)
{
	int i, j;
	int *tempArrayNum, *tempCounter, pointCounter[PeakNum];
	int maxCounter, maxPos;
	CString str, strNum;
	CvScalar color;

	maxCounter = 0;
	maxPos = -1;

	tempArrayNum = new int[num];
	tempCounter = new int[num];

	for (i = 0; i < num; i++)
	{
		tempArrayNum[i] = i;
		tempCounter[i] = counter[i];
	}

	for (i = 0; i < PeakNum; ++i)
	{
		pointCounter[i] = 0;
	}

	// Sort by decreasing counter (bubbleSort)
	for (i = num-1; i > 0; i--)
	{
		bool exchange = FALSE;

		for (j = 0; j < i; j++)
		{
			if(tempCounter[j] < tempCounter[j+1])
			{
				SwapInt(tempCounter[j], tempCounter[j+1]);
				SwapInt(tempArrayNum[j], tempArrayNum[j+1]);

				exchange = TRUE;
			}
		}

		if(!exchange)
			break;
	}

#if Debug3
	// output PeakNum ellipse with points
	IplImage *imgTemp[PeakNum];
	color = cvScalar(0, 255, 0, 0);

	for (j = 0; j < PeakNum; j++)
	{
		imgTemp[j] = cvCreateImage(cvGetSize(m_out_img_bright_part), 8, 3);

		if (m_out_img_bright_part != NULL)
			cvMerge(m_skeleton, m_skeleton, m_skeleton, NULL, imgTemp[j]);

		xc = ellipsePara[tempArrayNum[j]].xc;
		yc = ellipsePara[tempArrayNum[j]].yc;
		a = ellipsePara[tempArrayNum[j]].a;
		b = ellipsePara[tempArrayNum[j]].b;
		phi = ellipsePara[tempArrayNum[j]].phi;

		center.x = xc;
		center.y = yc;
		axes.width = a;
		axes.height = b;
		cvEllipse(imgTemp[j], center, axes, phi, 0, 360, color);

		c = sqrt((double)a*a-b*b);
		radian = phi * CV_PI / 180.0;

		p1.x = cvRound(xc - c*cos(radian));
		p1.y = cvRound(yc + c*sin(radian));
		p2.x = cvRound(xc + c*cos(radian));
		p2.y = cvRound(yc - c*sin(radian));

		cvCircle(imgTemp[j], center, 2, color, 2);
		cvCircle(imgTemp[j], p1, 2, color, 2);
		cvCircle(imgTemp[j], p2, 2, color, 2);
	}
#endif

	// count the num of points on ellipse
	color = cvScalar(0, 0, 255, 0);

	cvDestroyAllWindows();

	for (j = 0; j < PeakNum; j++)
	{
		CvPoint center;
		CvSize axes;
		IplImage *img = cvCreateImage(cvGetSize(m_skeleton), 8, 3);

		cvMerge(m_skeleton, m_skeleton, m_skeleton, NULL, img);

		pointCounter[j] = CounterPointsOnEllipse(ellipsePara[tempArrayNum[j]], img);

		/*center.x = ellipsePara[tempArrayNum[j]].xc;
		center.y = ellipsePara[tempArrayNum[j]].yc;
		axes.width = ellipsePara[tempArrayNum[j]].a;
		axes.height = ellipsePara[tempArrayNum[j]].b;
		cvEllipse(img, center, axes, ellipsePara[tempArrayNum[j]].phi, 0, 360, CV_RGB(0, 255, 0));

		char ch[100];
		sprintf_s(ch, "Image-%d(Counter:%d,Num:%d)", j+1, tempCounter[j], pointCounter[j]);
		cvNamedWindow(ch, 1);
		cvShowImage(ch, img);*/
	}

#if Debug3
	for (j = 0; j < PeakNum; j++)
	{
		str = "PointsOnEllipse-";
		strNum.Format("%d", j+1);
		str += strNum;

		cvNamedWindow(str, 1);
		cvShowImage(str, imgTemp[j]);
	}
	cvWaitKey(0);
#endif

	// find maximum of pointCounter on ellipse
	int maxI;
	str = "pointCounter : \n";
	for (j = 0; j < PeakNum; j++)
	{
		strNum.Format(_T("%d, "), pointCounter[j]);
		str += strNum;

		if (maxCounter < pointCounter[j])
		{
			maxCounter = pointCounter[j];
			maxPos = tempArrayNum[j];
			maxI = j;
		}
	}

	str += "\nmaxPos : ";
	strNum.Format(_T("%d"), maxPos);
	str += strNum;
	str += "\nmaxI : ";
	strNum.Format(_T("%d"), maxI);
	str += strNum;

	//AfxMessageBox(str);

#if Debug2
	// output ellipse
	IplImage *img;

	img = cvCreateImage(cvGetSize(m_out_img_bright_part), 8, 3);

	if (m_out_img_bright_part != NULL)
		cvMerge(m_skeleton, m_skeleton, m_skeleton, NULL, img);

	str = "Peaks : \n";

	int xc, yc, a, b;
	double phi, c, radian;
	CvPoint center, p1, p2;
	CvSize axes;

	for (j = 0; j < PeakNum; j++)
	{
		strNum.Format("Counter - %d ; ", tempCounter[j]);
		str += strNum;

		strNum.Format("xc: %d, ", ellipsePara[tempArrayNum[j]].xc);
		str += strNum;
		strNum.Format("yc: %d, ", ellipsePara[tempArrayNum[j]].yc);
		str += strNum;
		strNum.Format("a: %d, ", ellipsePara[tempArrayNum[j]].a);
		str += strNum;
		strNum.Format("b: %d, ", ellipsePara[tempArrayNum[j]].b);
		str += strNum;
		strNum.Format("phi: %f\n", ellipsePara[tempArrayNum[j]].phi);
		str += strNum;

		xc = ellipsePara[tempArrayNum[j]].xc;
		yc = ellipsePara[tempArrayNum[j]].yc;
		a = ellipsePara[tempArrayNum[j]].a;
		b = ellipsePara[tempArrayNum[j]].b;
		phi = ellipsePara[tempArrayNum[j]].phi;

		center.x = xc;
		center.y = yc;
		axes.width = a;
		axes.height = b;

		// draw ellipse
		if (j == 0)
		{
			// draw two focus
			color = cvScalar(0, 255, 0, 0);

			c = sqrt((double)a*a-b*b);
			radian = phi * CV_PI / 180.0;

			p1.x = cvRound(xc - c*cos(radian));
			p1.y = cvRound(yc + c*sin(radian));
			p2.x = cvRound(xc + c*cos(radian));
			p2.y = cvRound(yc - c*sin(radian));

			cvCircle(img, center, 2, color, 2);
			cvCircle(img, p1, 2, color, 2);
			cvCircle(img, p2, 2, color, 2);
		}
		else
			color = cvScalar(i*255/PeakNum, 255, 255, 0);

		cvEllipse(img, center, axes, phi, 0, 360, color);
	}

	// improved objective ellipse
	color = cvScalar(0, 0, 255, 0);

	// draw two focus
	xc = ellipsePara[maxPos].xc;
	yc = ellipsePara[maxPos].yc;
	a = ellipsePara[maxPos].a;
	b = ellipsePara[maxPos].b;
	phi = ellipsePara[maxPos].phi;

	center.x = xc;
	center.y = yc;
	axes.width = a;
	axes.height = b;
	cvEllipse(img, center, axes, phi, 0, 360, color);

	c = sqrt((double)a*a-b*b);
	radian = phi * CV_PI / 180.0;

	p1.x = cvRound(xc - c*cos(radian));
	p1.y = cvRound(yc + c*sin(radian));
	p2.x = cvRound(xc + c*cos(radian));
	p2.y = cvRound(yc - c*sin(radian));

	cvCircle(img, center, 2, color, 2);
	cvCircle(img, p1, 2, color, 2);
	cvCircle(img, p2, 2, color, 2);

	cvNamedWindow("EllipsePeak", 1);
	cvShowImage("EllipsePeak", img);
	cvWaitKey(0);


	//AfxMessageBox(str);

	cvReleaseImage(&img);
#endif

	return maxPos;
}

void ImageClass::DrawDetectedEllipse(IplImage *imgInput, EllipsePara ep, int grayValue)
{
	//if (m_iterativeTimesIRHT > 0)
	{
		// draw ellipse
		CvPoint center;
		CvSize axes;
		CvScalar grayColor = cvScalar(grayValue, grayValue, grayValue, 0);
		IplImage *img = cvCreateImage(cvGetSize(imgInput), 8, 3);

		cvCvtColor(imgInput, img, CV_GRAY2BGR);

		double phi = ep.phi;//180.0 - ep.phi;
		center.x = ep.xc;
		center.y = ep.yc;
		axes.width = ep.a;
		axes.height = ep.b;
		cvEllipse(img, center, axes, phi, 0, 360, grayColor);

		// draw rectangle
		CvPoint pt1, pt2, pt3, pt4, ptNew1, ptNew2, ptNew3, ptNew4;
		grayColor = cvScalar(100, 100, 100, 0);

		pt1.x = m_polarOrigin.x-m_roiRegion.width/2;
		pt1.y = m_polarOrigin.y-m_roiRegion.height/2;
		pt2.x = m_polarOrigin.x-m_roiRegion.width/2;
		pt2.y = m_polarOrigin.y+m_roiRegion.height/2;
		pt3.x = m_polarOrigin.x+m_roiRegion.width/2;
		pt3.y = m_polarOrigin.y+m_roiRegion.height/2;
		pt4.x = m_polarOrigin.x+m_roiRegion.width/2;
		pt4.y = m_polarOrigin.y-m_roiRegion.height/2;

		RotationPoint(pt1, phi, m_polarOrigin, ptNew1);
		RotationPoint(pt2, phi, m_polarOrigin, ptNew2);
		RotationPoint(pt3, phi, m_polarOrigin, ptNew3);
		RotationPoint(pt4, phi, m_polarOrigin, ptNew4);

		CString str, strNum;

		if (m_iterativeTimesIRHT >= 1)
			cvDestroyAllWindows();

		/*str = "DetectedEllipse-";
		strNum.Format("%d", m_iterativeTimesIRHT);
		str += strNum;
		cvNamedWindow(str, 1);
		cvMoveWindow(str, 200, 200);
		cvShowImage(str, img);*/

		cvLine(img, ptNew1, ptNew2, grayColor);
		cvLine(img, ptNew2, ptNew3, grayColor);
		cvLine(img, ptNew3, ptNew4, grayColor);
		cvLine(img, ptNew4, ptNew1, grayColor);

		// Draw Points on Ellipse
		int counter;
		//counter = CounterPointsOnEllipse(ep, img);

		/*str = "";
		strNum.Format("Rectangle-%d(Num:%d,Phi:%f)", m_iterativeTimesIRHT, counter, ep.phi);
		str += strNum;
		cvNamedWindow(str, 1);
		cvMoveWindow(str, 300, 200);
		cvShowImage(str, img);*/

		str = "Num of points on ellipse : ";
		strNum.Format(_T("%d"), counter);
		str += strNum;
		str += "\n\rPhi : ";
		strNum.Format(_T("%f"), ep.phi);
		str += strNum;
		//AfxMessageBox(str);

		//cvWaitKey(0);
	}
}

int ImageClass::CounterPointsOnEllipse(EllipsePara ep, IplImage *img)
{
	int i, counter;
	int x, y;
	int xc, yc, a, b, c, d;
	double phi, radian;
	CvPoint p1, p2, ep1, ep2, polarOrigin;

	counter = 0;
	xc = ep.xc;
	yc = ep.yc;
	a = ep.a;
	b = ep.b;
	phi = ep.phi;

	c = sqrt((double)a*a-(double)b*b);

	radian = phi * CV_PI / 180.0;

	/*p1.x = xc - c*cos(radian);
	p1.y = yc - c*sin(radian);
	p2.x = xc + c*cos(radian);
	p2.y = yc + c*sin(radian);*/

	p1.x = xc - c;
	p1.y = yc;
	p2.x = xc + c;
	p2.y = yc;

	polarOrigin.x = xc;
	polarOrigin.y = yc;

	RotationPoint(p1, phi, polarOrigin, ep1);
	RotationPoint(p2, phi, polarOrigin, ep2);

	CvPoint center;
	/*center.x = xc;
	center.y = yc;
	cvCircle(img, center, 2, cvScalar(255, 0, 0), 2);

	center.x = ep1.x;
	center.y = ep1.y;
	cvCircle(img, center, 2, cvScalar(0, 255, 0), 2);

	center.x = ep2.x;
	center.y = ep2.y;
	cvCircle(img, center, 2, cvScalar(0, 255, 0), 2);*/

	for (i = 0; i < m_brightPointSum; i++)
	{
		x = m_brightPoints[i].x;
		y = m_brightPoints[i].y;
		
		d = sqrt((double)(x-ep1.x)*(x-ep1.x)+(double)(y-ep1.y)*(y-ep1.y));
		d += sqrt((double)(x-ep2.x)*(x-ep2.x)+(double)(y-ep2.y)*(y-ep2.y));

		if (abs(d - 2*a) <= 1.2)
		{
			counter++;
			
			if (img != NULL)
			{
				center.x = x;
				center.y = y;

				cvCircle(img, center, 2, cvScalar(0, 0, 255), 2);
			}
		}
	}

	return counter;
}

void ImageClass::SwapInt(int &x1, int &x2)
{
	int temp;

	temp = x1;
	x1 = x2;
	x2 = temp;
}

int ImageClass::DetectPeak(int num, int *counter)
{
	int i;
	int maxNum, maxPos;
	CString str, strNum;

	maxNum = 0;
	maxPos = -1;
	str = "Sequence : ";

	for (i = 0; i < num; ++i)
	{
		strNum.Format(_T("%d, "), counter[i]);
		str += strNum;

		if (counter[i] > maxNum)
		{
			maxNum = counter[i];
			maxPos = i;
		}
	}

	str += "\r\nmaxNum : ";
	strNum.Format(_T("%d, "), maxNum);
	str += strNum;

	str += "maxPos : ";
	strNum.Format(_T("%d."), maxPos);
	str += strNum;

	//AfxMessageBox(str);

	return maxPos;
}

void ImageClass::SetROI()
{
	int i;
	double sum, sum2, mean, variance;

	// width
	sum = sum2 = mean = variance = 0.0;

	for (i = 0; i < m_aNum; ++i)
	{
		sum += m_a[i];
		sum2 += m_a[i] * m_a[i];
	}

	mean = sum/m_aNum;
	variance = sqrt((sum2 - (sum*sum)/m_aNum)/(m_aNum-1));
	m_roiRegion.width = m_cEst.a*2 + variance;

	CString str, strTemp;
	strTemp.Format(_T("width_variance : %f\n"), variance);
	str = strTemp;

	// height
	sum = sum2 = mean = variance = 0.0;

	for (i = 0; i < m_bNum; ++i)
	{
		sum += m_b[i];
		sum2 += m_b[i] * m_b[i];
	}

	mean = sum/m_bNum;
	variance = sqrt((sum2 - (sum*sum)/m_bNum)/(m_bNum-1));
	m_roiRegion.height = m_cEst.b*2 + variance;

	strTemp.Format(_T("height_variance : %f"), variance);
	str += strTemp;
	//AfxMessageBox(str);

	m_polarOrigin.x = m_cEst.xc;
	m_polarOrigin.y = m_cEst.yc;
}

bool ImageClass::EstimateParameters(CvPoint *p)
{
	int i;
	int xc, yc, a, b;
	double e, eSquare, phi;
	double u, v, r, s, t, ellipseJudge;
	CvMat *A = cvCreateMat(5, 5, CV_64FC1);
	CvMat *x = cvCreateMat(5, 1, CV_64FC1);
	CvMat *B = cvCreateMat(5, 1, CV_64FC1);

	for (i = 0; i < 5; ++i)
	{
		cvmSet(A, i, 0, pow((double)p[i].x, 2.0) - pow((double)p[i].y, 2.0));
		cvmSet(A, i, 1, 2.0*p[i].x*p[i].y);
		cvmSet(A, i, 2, p[i].x);
		cvmSet(A, i, 3, p[i].y);
		cvmSet(A, i, 4, 1.0);

		cvmSet(B, i, 0, pow((double)p[i].x, 2.0) + pow((double)p[i].y, 2.0));
	}

	if (cvSolve(A, B, x) == 0)
		return FALSE;

	u = cvmGet(x, 0, 0);
	v = cvmGet(x, 1, 0);
	r = cvmGet(x, 2, 0);
	s = cvmGet(x, 3, 0);
	t = cvmGet(x, 4, 0);

	// u^2 + v^2 - 1 < 0 is ellipse
	double temp = pow((double)u, 2.0) + pow((double)v, 2.0);
	ellipseJudge = 1.0 - temp;

	if (ellipseJudge <= 0)
		return FALSE;

	xc = cvRound((s*v+r+r*u)/2.0/(1.0-temp));
	yc = cvRound((r*v+s-s*u)/2.0/(1.0-temp));

	if ((xc < 0) || (xc >= m_width) || (yc < 0) || (yc >= m_height))
		return FALSE;

	temp = sqrt(temp);
	eSquare = (1.0 - temp) / (1.0 + temp);
	e = sqrt(eSquare);

	if (m_headDetectionFlag)
	{
		if ((e < 0.651) || (e > 0.915))
			return FALSE;
	}
	
	//phi = acos(u*(1+eSquare)/(1-eSquare));

	//if (v > 0)
		//phi = 2.0*CV_PI - phi;

	phi = atan2(v, u);
	phi = 0.5 * phi * 180.0 / CV_PI;   // angle

	if (phi < 0)
		phi = 180.0 + phi;

	a = cvRound(sqrt((2.0*t+xc*r+yc*s)/2.0/(1.0-temp)));
	b = cvRound(sqrt((2.0*t+xc*r+yc*s)/2.0/(1.0+temp)));

	double diagonal;
	diagonal = sqrt(pow((double)(m_height), 2.0) + pow((double)(m_width), 2.0));

#if ConePhantomFlag
	double c = sqrt((double)a*a - (double)b*b);
	double eccen = c/a;

	if ((a > 85) || (b > 85) || (eccen > 0.7)) // 43
		return FALSE;
#endif

	if ((a <= 0) || (b <= 0) || (a > diagonal/2) || (b > diagonal/2))
		return FALSE;

	if (a < b)
	{
		int temp;
		temp = a;
		a = b;
		b = temp;
		AfxMessageBox(_T("a < b"));
	}

	double times = 1;
	
	// save
#if EllipseParaAccumulatorType
	if (m_ellipseParaNum == 0)
	{
		// One 5-D accumulator
		m_ellipsePara[0].u = u;
		m_ellipsePara[0].v = v;
		m_ellipsePara[0].r = r;
		m_ellipsePara[0].s = s;
		m_ellipsePara[0].t = t;

		m_ellipsePara[0].xc = xc;
		m_ellipsePara[0].yc = yc;
		m_ellipsePara[0].a = a;
		m_ellipsePara[0].b = b;
		m_ellipsePara[0].phi = phi;

		m_ellipseParaCounter[0]++;
		m_ellipseParaNum++;

		// Five 1-D accumulator
		m_a[0] = a;
		m_aCounter[0]++;
		m_aNum++;

		m_b[0] = b;
		m_bCounter[0]++;
		m_bNum++;
	}
	else
	{
		// One 5-D accumulator
		EllipsePara temp;
		temp.u = u;
		temp.v = v;
		temp.r = r;
		temp.s = s;
		temp.t = t;
		temp.xc = xc;
		temp.yc = yc;
		temp.a = a;
		temp.b = b;
		temp.phi = phi;

		CheckNewEllipseParaInPresent(temp, m_ellipseParaNum, m_ellipsePara, m_ellipseParaCounter, times);

		// Five 1-D accumulator
		CheckNewInPresent(a, m_aNum, m_a, m_aCounter, times);
		CheckNewInPresent(b, m_bNum, m_b, m_bCounter, times);
	}
#else
	if (m_xcNum == 0)
	{
		// Five 1-D accumulator
		m_xc[0] = xc;
		m_xcCounter[0]++;
		m_xcNum++;

		m_yc[0] = yc;
		m_ycCounter[0]++;
		m_ycNum++;

		m_a[0] = a;
		m_aCounter[0]++;
		m_aNum++;

		m_b[0] = b;
		m_bCounter[0]++;
		m_bNum++;

		m_phi[0] = phi;
		m_phiCounter[0]++;
		m_phiNum++;
	}
	else
	{
		CheckNewInPresent(xc, m_xcNum, m_xc, m_xcCounter, times);
		CheckNewInPresent(yc, m_ycNum, m_yc, m_ycCounter, times);
		CheckNewInPresent(a, m_aNum, m_a, m_aCounter, times);
		CheckNewInPresent(b, m_bNum, m_b, m_bCounter, times);
		CheckNewDoubleInPresent(phi, m_phiNum, m_phi, m_phiCounter, times);
	}
#endif

#if FivePointsTest
	// Draw 5 points
	CvPoint center;
	CvSize axes;
	
	if (m_skeleton != NULL)
		cvMerge(m_skeleton, m_skeleton, m_skeleton, NULL, m_out_img_ellipse);

	for (i = 0; i < 5; ++i)
	{
		center.x = p[i].x;
		center.y = p[i].y;

		cvCircle(m_out_img_ellipse, center, 4, cvScalar(0, 0, 255), -1);
	}

	// ellipse
	center.x = xc;
	center.y = yc;
	axes.width = a;
	axes.height = b;

	cvEllipse(m_out_img_ellipse, center, axes, phi, 360, 0, cvScalar(0, 255, 0));

	char ch[100];
	sprintf_s(ch, "FivePoins(phi:%f)", phi);
	cvNamedWindow(ch, 1);
	cvShowImage(ch, m_out_img_ellipse);
	cvWaitKey(0);
#endif
	
	return TRUE;
}

void ImageClass::CheckNewEllipseParaInPresent(EllipsePara pt, int &ptNum, EllipsePara *ptGroup, int *ptCount, double times)
{
	int i;

	for (i = 0; i < ptNum; ++i)
	{
		if (ComparePar(pt, ptGroup[i], times))
		{
			ptCount[i]++;
			return;
		}
	}

	ptGroup[ptNum] = pt;
	ptCount[ptNum]++;
	ptNum++;
}

void ImageClass::CheckNewInPresent(int pt, int &ptNum, int *ptGroup, int *ptCount, double times)
{
	int i;

	for (i = 0; i < ptNum; ++i)
	{
		if (abs(pt - ptGroup[i]) <= 2.0*times)
		{
			ptCount[i]++;
			return;
		}
	}

	ptGroup[ptNum] = pt;
	ptCount[ptNum]++;
	ptNum++;
}

void ImageClass::CheckNewDoubleInPresent(double pt, int &ptNum, double *ptGroup, int *ptCount, double times)
{
	int i;
	double d;

	for (i = 0; i < ptNum; ++i)
	{
		d = fabs(pt - ptGroup[i]);

		//if (d > 90.0)
			//d = 180.0 - d;

		if (d <= 2.5*times)
		{
			ptCount[i]++;
			return;
		}
	}

	ptGroup[ptNum] = pt;
	ptCount[ptNum]++;
	ptNum++;
}

bool ImageClass::ComparePar(EllipsePara pt1, EllipsePara pt2, double times)
{
	double d_xc, d_yc, d_a, d_b, d_sum;
	double d_phi;

	d_xc = abs(pt1.xc - pt2.xc);
	d_yc = abs(pt1.yc - pt2.yc);
	d_a = abs(pt1.a - pt2.a);
	d_b = abs(pt1.b - pt2.b);
	d_phi = fabs(pt1.phi - pt2.phi);
	d_sum = d_xc + d_yc + d_a + d_b;

	if (d_phi > 90.0)
		d_phi = 180.0 - d_phi;

	if ((d_sum <= 6.0*times) && (d_xc <= 2.0*times) && (d_yc <= 2.0*times) && (d_a <= 2.0*times) && (d_b <= 2.0*times) && (d_phi <= 2.5*times))
		return TRUE;
	else
		return FALSE;
}

void ImageClass::WriteParameters()
{
	FILE *fp;
	char ch[1000];
	char content[1000];

#if EllipseParaAccumulatorType
	sprintf_s(ch, "Input\\Paras-%d.txt", PeakNum);
#else
	sprintf_s(ch, "Input\\Paras-I.txt");
#endif

	fopen_s(&fp, ch, "at+");

	if (fp == NULL)
	{
		AfxMessageBox(_T("File Open Failed (In WriteParameters)!"));
		return;
	}

	content[0] = '\0';
	sprintf_s(ch, "%d\t", m_cEst.xc);
	strcat_s(content, ch);
	sprintf_s(ch, "%d\t", m_cEst.yc);
	strcat_s(content, ch);
	sprintf_s(ch, "%d\t", m_cEst.a);
	strcat_s(content, ch);
	sprintf_s(ch, "%d\t", m_cEst.b);
	strcat_s(content, ch);
	sprintf_s(ch, "%.2f\t", m_cEst.phi);
	strcat_s(content, ch);
	sprintf_s(ch, "%d\t", m_iterativeTimesIRHT);
	strcat_s(content, ch);
	sprintf_s(ch, "%d\n", m_pointNumOnFinalEllipse);
	strcat_s(content, ch);

	//AfxMessageBox(content);

	fputs(content, fp);

	fflush(fp);

	fclose(fp);
}

void ImageClass::SetRingROI(EllipsePara ep)
{
	int i, j;
	int x, y;
	double xc, yc, aOut, bOut, aIn, bIn, phi, radian, cOut, cIn, dOut, dIn, percent;
	CvPoint2D64f pOut1, pOut2, pIn1, pIn2;
	IplImage *img;

	img = cvCloneImage(m_in_img);

	percent = 0.15;

	// two focus centers
	xc = ep.xc;
	yc = ep.yc;
	aOut = ep.a*(1+percent);
	bOut = ep.b*(1+percent);
	aIn = ep.a*(1-percent);
	bIn = ep.b*(1-percent);
	phi = ep.phi;

	cOut = sqrt((double)aOut*aOut - bOut*bOut);
	cIn = sqrt((double)aIn*aIn - bIn*bIn);
	radian = phi * CV_PI / 180.0;

	pOut1.x = xc - cOut*cos(radian);
	pOut1.y = yc + cOut*sin(radian);
	pOut2.x = xc + cOut*cos(radian);
	pOut2.y = yc - cOut*sin(radian);

	pIn1.x = xc - cIn*cos(radian);
	pIn1.y = yc + cIn*sin(radian);
	pIn2.x = xc + cIn*cos(radian);
	pIn2.y = yc - cIn*sin(radian);

	for (i = 0; i < m_height; i++)
	{
		for (j = 0; j < m_width; j++)
		{
			x = j;
			y = i;

			dOut = sqrt((x-pOut1.x)*(x-pOut1.x)+(y-pOut1.y)*(y-pOut1.y));
			dOut += sqrt((x-pOut2.x)*(x-pOut2.x)+(y-pOut2.y)*(y-pOut2.y));

			dIn = sqrt((x-pIn1.x)*(x-pIn1.x)+(y-pIn1.y)*(y-pIn1.y));
			dIn += sqrt((x-pIn2.x)*(x-pIn2.x)+(y-pIn2.y)*(y-pIn2.y));

			if ((dOut > 2*aOut) || (dIn < 2*aIn))
				img->imageData[y*m_step+x] = 0;
		}
	}

	/*color = cvScalar(255, 255, 255);
	center.x = xc;
	center.y = yc;
	axes.width = aOut;
	axes.height = bOut;
	cvEllipse(img, center, axes, phi, 0, 360, color);

	axes.width = aIn;
	axes.height = bIn;
	cvEllipse(img, center, axes, phi, 0, 360, color);*/

	cvNamedWindow("RingROI", 1);
	cvShowImage("RingROI", img);
	cvSaveImage("Input\\RingROI.png", img);
}

void ImageClass::GetPointsOnEllipse(EllipsePara ep)
{
	int i, j;
	int x, y;
	double xc, yc, a, b, phi, radian, c, d;
	CvPoint2D64f p1, p2;

	// two focus centers
	xc = ep.xc;
	yc = ep.yc;
	a = ep.a;
	b = ep.b;
	phi = ep.phi;

	c = sqrt((double)a*a-b*b);
	radian = phi * CV_PI / 180.0;

	p1.x = xc - c*cos(radian);
	p1.y = yc + c*sin(radian);
	p2.x = xc + c*cos(radian);
	p2.y = yc - c*sin(radian);

	startPoints = new CvPoint[m_width*m_height];
	startPointsNum = 0;

	for (i = 0; i < m_height; i++)
	{
		for (j = 0; j < m_width; j++)
		{
			x = j;
			y = i;

			d = sqrt((x-p1.x)*(x-p1.x)+(y-p1.y)*(y-p1.y));
			d += sqrt((x-p2.x)*(x-p2.x)+(y-p2.y)*(y-p2.y));

			if (fabs(d - 2*a) < 1.0)
			{
				startPoints[startPointsNum].x = x;
				startPoints[startPointsNum].y = y;
				startPointsNum++;
			}
		}
	}

	IplImage *temp;
	temp = cvCreateImage(cvSize(m_width, m_height), 8, 1);
	cvSet(temp, CV_RGB(255, 255, 255));

	for (i = 0; i < startPointsNum; i++)
		temp->imageData[startPoints[i].y*m_step+startPoints[i].x] = 0;

	cvNamedWindow("startPoints", 1);
	cvShowImage("startPoints", temp);
}

void ImageClass::SnakeDeformation(CvPoint *sp, int spNum)
{
	int i;
	float alpha, beta, gamma;
	CvSize size;
	CvTermCriteria criteria;
	IplImage *temp1;

	alpha = 0.2;
	beta = 0.2;
	gamma = 0.2;
	size.width = 3;
	size.height = 3;
	criteria.type = CV_TERMCRIT_ITER;
	criteria.max_iter = 1000;
	criteria.epsilon = 0.1;

	temp1 = cvCloneImage(m_in_img);
	cvThreshold(m_in_img, temp1, 230, 255, CV_THRESH_BINARY);
	cvNamedWindow("Threshold", 1);
	cvShowImage("Threshold", temp1);

	//cvSnakeImage(temp1, sp, spNum, &alpha, &beta, &gamma, CV_VALUE, size, criteria, 1);

	IplImage *temp2;
	temp2 = cvCloneImage(m_in_img);
	
	for (i = 0; i < spNum; i++)
	{
		//temp->imageData[sp[i].y*m_step+sp[i].x] = 255;
		int j = (i+1)%spNum;
		cvLine(temp2, sp[i], sp[j], CV_RGB(255, 255, 255), 1, 8, 0); 
	}

	cvNamedWindow("Snake", 1);
	cvShowImage("Snake", temp2);
}
