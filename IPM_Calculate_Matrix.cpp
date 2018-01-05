#include <highgui.h>
#include< string.h>
#include <cv.h>
#include <cxcore.h>
#include <math.h>


int main(void) {

	// INPUT PARAMETERS:
	//
	int board_w = 6;
	int board_h = 8;
	int board_n = board_w * board_h;
	CvSize board_sz = cvSize( board_w, board_h );
	//CvMat* intrinsic = (CvMat*)cvLoad(argv[3]);
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

	//CvMat* distortion = (CvMat*)cvLoad(argv[4]);
	CvMat* distortion = cvCreateMat( 4, 1, CV_32FC1 );

	distortion->data.fl[0] = -0.04;
	distortion->data.fl[1] = -1.10;
	distortion->data.fl[2] = -0.02; 
	distortion->data.fl[3] = 0.02;

	IplImage* image = 0;
	IplImage* gray_image = 0;

	image = cvLoadImage("Board.JPG", 1);

	gray_image = cvCreateImage( cvGetSize(image), 8, 1 );
	cvCvtColor(image, gray_image, CV_BGR2GRAY );
	// UNDISTORT OUR IMAGE
	//
	IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );

	//This initializes rectification matrices
	//
	cvInitUndistortMap(	intrinsic,	distortion,	mapx, mapy );
	IplImage *t = cvCloneImage(image);

	// Rectify our image
	//

	cvRemap( t, image, mapx, mapy );
	// GET THE CHESSBOARD ON THE PLANE
	//
	cvNamedWindow("Lane", 1);
	CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
	int corner_count = 0;
	int found = cvFindChessboardCorners( image,	board_sz, corners, &corner_count,
		CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );

	//Get Subpixel accuracy on those corners:
	cvFindCornerSubPix( gray_image,	corners, corner_count, cvSize(11,11), cvSize(-1,-1),
		cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1 ));
	//GET THE IMAGE AND OBJECT POINTS:
	// We will choose chessboard object points as (r,c):
	// (0,0), (board_w-1,0), (0,board_h-1), (board_w-1,board_h-1).
	//
	CvPoint2D32f objPts[4], imgPts[4];
	objPts[0].x = 0;            objPts[0].y = 0;
	objPts[1].x = board_w-1;    objPts[1].y = 0;
	objPts[2].x = 0;            objPts[2].y = board_h-1;
	objPts[3].x = board_w-1;    objPts[3].y = board_h-1;
	imgPts[0] = corners[0];

	imgPts[1] = corners[board_w-1];
	imgPts[2] = corners[(board_h-1)*board_w];
	imgPts[3] = corners[(board_h-1)*board_w + board_w-1];
	// DRAW THE POINTS in order: B,G,R,YELLOW
	//
	cvCircle( image, cvPointFrom32f(imgPts[0]), 9, CV_RGB(0,0,255), 3);
	cvCircle( image, cvPointFrom32f(imgPts[1]), 9, CV_RGB(0,255,0), 3);
	cvCircle( image, cvPointFrom32f(imgPts[2]), 9, CV_RGB(255,0,0), 3);
	cvCircle( image, cvPointFrom32f(imgPts[3]), 9, CV_RGB(255,255,0), 3);
	// DRAW THE FOUND CHESSBOARD
	//
	cvDrawChessboardCorners( image, board_sz, corners, corner_count, found );
	cvShowImage( "Lane", image );
	// FIND THE HOMOGRAPHY
	//
	CvMat *H = cvCreateMat( 3, 3, CV_32F);



	cvGetPerspectiveTransform( objPts, imgPts, H);
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

		key = cvWaitKey(0);
		if(key == 'u') Z += 0.5;
		if(key == 'd') Z -= 0.5;
	}
	cvSave("H.txt",H); //We can reuse H for the same camera mounting
	return 0;
}