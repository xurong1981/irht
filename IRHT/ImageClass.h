#pragma once

//OpenCV
#include "opencv\cv.h"
#include "opencv\highgui.h"

#define Debug1 0
#define Debug2 0
#define Debug3 0
#define Debug4 0
#define FivePointsTest 0
#define EllipseParaAccumulatorType 1
#define ImgMaxSize 300
#define ImprovementFlag 1
#define PeakNum 10
#define ConePhantomFlag 0

typedef struct 
{
	int xc;
	int yc;
	int a;
	int b;
	double phi;       // angle

	double u;
	double v;
	double r;
	double s;
	double t;
}EllipsePara;

class ImageClass
{
public:
	ImageClass(void);
	~ImageClass(void);

	IplImage *m_in_img, *m_out_img;             // input & output images
	IplImage *m_filter_img, *m_edge_img;
	int **m_in_img_data;
	int m_totalPixel;
	int m_width, m_height, m_step;		        // width and height of the displayed image
	char *m_folderPath;
	char *m_filePath;
	
	// K-mean
	bool m_skeletonFlag;
	bool m_headDetectionFlag;
	int m_iterativeTimes;
	int m_classSum;
	int *m_classCounter;
	double *m_mean;
	double *m_variance;		                             // variances for each region
	int **m_classes;		                             // this is the labeled image
	int **m_binaryMark;
	int m_inputBackgroundType;

	// fcm
	double **m_u;                       // Membership of each pixel in each region

	// IRHT
	IplImage *m_out_img_bright_part, *m_skeleton, *m_out_img_ellipse;
	int m_brightPointSum, m_brightPointNumROI;
	CvPoint *m_brightPoints;
	CvPoint *m_brightPointsROI;
	CvPoint m_polarOrigin;
	CvSize m_roiRegion;
	int m_K;
	int m_samplesNum;
	int m_iterativeTimesIRHT;
	int m_exeTimes;
	int *m_xc, *m_yc, *m_a, *m_b;
	double *m_phi;                                    // angle
	int m_xcNum, m_ycNum, m_aNum, m_bNum, m_phiNum;
	int *m_xcCounter, *m_ycCounter, *m_aCounter, *m_bCounter, *m_phiCounter;
	int m_ellipseParaNum;                            // Num of all c parameter
	EllipsePara *m_ellipsePara;
	int *m_ellipseParaCounter;                         // Counter of each c parameter
	int m_pointNumOnFinalEllipse;
	EllipsePara m_cEst;                                // Cest : final c parameter
	EllipsePara m_cEstPre;

	// Snake
	int startPointsNum;
	CvPoint *startPoints;

public:
	void SetSkeletonFlag(bool flag);
	void SetHeadDectionFlag(bool flag);
	IplImage * LoadImg(int biFilterFlag);
	bool SaveImg(const char* filePath);
	void Init(int n, int inputBackgroundType);
	void Allocation(int n);
	void InitOutImg(void);
	void CreateOutputImg(int flag);
	void CreateOutputEllipse(void);
	void MyCannyDetection(void);
	void DrawDetectedEllipse(IplImage *imgInput, EllipsePara ep, int grayValue);

	// histogram
	void HistogramStat(int dims, int num, int *counter, int *data, char *ch);
	void DrawCoordinate(IplImage *img, CvPoint original, int size, double dPercent);
	void DrawHistogram(IplImage *img, CvPoint original, int num, int *counter, int *data, int size, float zoomY);

	// k-means
	bool InitClusterCenter(void);
	void CalculateMeanAndVariance(int x, int y, int w, int h, double &mean, double &variance);
	void UpdateVariances(void);
	void K_PixelClass();
	bool KMeans(void);
	void BrightPartExtraction(void);

	// fcm
	void FuzzyClustering(int umatrix_flag, int means_flag, int g_width);
	double UpdateUmatrix(int flag, double b, int g_width);
	int IsMeans(int x, int y);
	double Dsquare_Gray(int flag, int x, int y, int k, int g_width);
	double UpdateFuzzyMeans(int flag, double b, int g_width);
	void C_PixelClass(void);

	// skeletonization
	void DistanceTransform(IplImage *img, int **mark);
	void SkeletonizationByDistanceTransform();    // Morphological Operations
	void Skeletons();
	bool TemplateMatching(int x, int y, int **binaryMark, int templateSize, int templateRect[3][3]);
	int HitAndMissTransform(IplImage *img, int **skeleton, int **convexMark);
	void SkeletonizationByThinning();

	// IRHT
	void ReadBrightPoints();
	IplImage* RotationImage(IplImage* img, double rotationAngle);
	void RotationPoint(CvPoint p, double rotationAngle, CvPoint polarOrigin, CvPoint &pNew);
	void IRHT_Ini(int K);
	bool IRHT(int irhtType);
	bool RHT(int irhtType);
	bool EstimateParameters(CvPoint *p);
	void CheckNewEllipseParaInPresent(EllipsePara pt, int &ptNum, EllipsePara *ptGroup, int *ptCount, double times);
	void CheckNewInPresent(int pt, int &ptNum, int *ptGroup, int *ptCount, double times);
	void CheckNewDoubleInPresent(double pt, int &ptNum, double *ptGroup, int *ptCount, double times);
	bool ComparePar(EllipsePara pt1, EllipsePara pt2, double times);
	int DetectPeak(int num, int *counter);
	int DetectPeaks(int num, int *counter, int peakNum, EllipsePara *ellipsePara);
	int CounterPointsOnEllipse(EllipsePara ep, IplImage *img);
	void SwapInt(int &x1, int &x2);
	void SetROI(void);
	void WriteParameters();

	// Snake
	void SetRingROI(EllipsePara ep);
	void GetPointsOnEllipse(EllipsePara ep);
	void SnakeDeformation(CvPoint *sp, int spNum);
};
