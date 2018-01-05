#include <highgui.h>
#include< string.h>
#include <cv.h>
#include <cxcore.h>
#include <math.h>


int main(void)
{
	IplImage* image = 0;

	CvMat* intrinsic = cvCreateMat( 3, 3, CV_32FC1 );

	intrinsic->data.fl[0] = 726.92; 
	intrinsic->data.fl[1] = 0.00; 
	intrinsic->data.fl[2] = 336.45; 
	intrinsic->data.fl[3] = 0.00; 
	intrinsic->data.fl[4] = 627.95;
	intrinsic->data.fl[5] = 200.55;
	intrinsic->data.fl[6] = 0.00;
	intrinsic->data.fl[7] = 0.00;
	intrinsic->data.fl[8] = 1.00;

	CvMat* distortion = cvCreateMat( 4, 1, CV_32FC1 );

	distortion->data.fl[0] = -0.04;
	distortion->data.fl[1] = -1.10;
	distortion->data.fl[2] = -0.02; 
	distortion->data.fl[3] = 0.02;

	image = cvLoadImage("DSCN4073.JPG", 1);

	IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );

	//This initializes rectification matrices
	//
	cvInitUndistortMap( intrinsic, distortion, mapx, mapy );
	IplImage *t = cvCloneImage(image);
	// Rectify our image
	//
	cvRemap( t, image, mapx, mapy );
	// GET THE CHESSBOARD ON THE PLANE
	//
	cvNamedWindow("Lane", 1);

	CvMat *H = cvCreateMat( 3, 3, CV_32F);

	H->data.fl[0] = 21.36484337;
	H->data.fl[1] = -7.59821081;
	H->data.fl[2] = 243.30793762;
	H->data.fl[3] = 0.55356616;
	H->data.fl[4] = 9.41518402;
	H->data.fl[5] = 229.26113892;
	H->data.fl[6] = 1.12514407e-003;
	H->data.fl[7] = -0.02500296;
	H->data.fl[8] = 25;


	//cvGetPerspectiveTransform( objPts, imgPts, H);
	// LET THE USER ADJUST THE Z HEIGHT OF THE VIEW
	//
	float Z = 25;
	int key = 0;
	IplImage *birds_image = cvCloneImage(image);
	cvNamedWindow("Birds_Eye",  1);
	// LOOP TO ALLOW USER TO PLAY WITH HEIGHT:
	//
	// escape key stops
	//
	while(key != 27) {
		// Set the height
		//
		CV_MAT_ELEM(*H,float,2,2) = Z;
		// COMPUTE THE FRONTAL PARALLEL OR BIRD¡¯S-EYE VIEW:
		// USING HOMOGRAPHY TO REMAP THE VIEW
		//
		cvWarpPerspective( image, birds_image, H,
			CV_INTER_LINEAR | CV_WARP_INVERSE_MAP | CV_WARP_FILL_OUTLIERS );
		cvShowImage( "Birds_Eye", birds_image );
		cvShowImage("Lane", image);

		key = cvWaitKey(0);
		if(key == 'u') Z += 0.5;
		if(key == 'd') Z -= 0.5;
	}
	return 0;
}