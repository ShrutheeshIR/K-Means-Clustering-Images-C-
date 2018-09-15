#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
using namespace cv;
using namespace std;

int ** KMeansInitCentroids(int **, int, int);
int *** runKMeans(int **, int **, int, int, int, int *);
int * findClosestCentroids(int **, int **, int, int *, int, int);
int ** computeCentroids(int **, int *, int, int, int **);

string opt1()
{
	printf("\nFollowing are a few sample fixtures. Press enter and input the image number you want to compress :P\n");
	int imno;
	std::string name("x.tiff");
	Mat i1;
	for (int i = 1; i<=8; i++)
	{
		name[0]=(char)(i+48);
		i1 = imread(name, IMREAD_COLOR);
		namedWindow(name, WINDOW_NORMAL);
		resizeWindow(name, 380, 380);
		moveWindow(name, ((i-1)%4)* 380, ((int)i/5 )*380);
		imshow(name, i1);
		cvWaitKey(500);
	}
	cvWaitKey(0);
	destroyAllWindows();
	printf("\nEnter Image Number: ");
	scanf("%d", &imno);
	name[0] = (char)(48 + imno);
	return name;
}

int main(int argc, char** argv)
{
	int option, opt2;
	std::string imageName("x.tiff");
	printf("________________________________________WELCOME TO IMAGE COMPRESSION___________________________________");
	printf("\n\n    Press 1. To compress one of standard images\n          2. To take a picture from camera and compress:  ");
	scanf("%d", &option);
	
	if (option == 1)
		imageName=opt1();

	else
	{
		VideoCapture cap(0); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return -1;
		
		// Show the image captured from the camera in the window and repeat
		while (1) {
			// Get one frame

			Mat frame;
			cap >> frame;

			//cvShowImage("mywindow", frame);
			// Do not release the frame!
			namedWindow("Original Image", WINDOW_NORMAL); // Create a window for display.
			resizeWindow("Original Image", 750, 600);
			moveWindow("Original Image", 25, 100);
			imshow("Original Image", frame);
			if ((cvWaitKey(10) & 255) == 's') {
				//CvSize size = cvGetSize(frame);
				//IplImage* img = cvCreateImage(size, IPL_DEPTH_16S, 1);
				//img = frame;
				imwrite("9.tiff", frame);
				break;
			}
		}
		imageName[0] = '9';
	}
	//cvWaitKey(1000);
	destroyAllWindows();
	printf("\nChoose the minimum ratio of compression you want to achieve.\nPress\n1. 4x\n2. 3x\n3. 2x ");
	scanf("%d", &opt2);
	namedWindow("Original Image", WINDOW_NORMAL); // Create a window for display.
	resizeWindow("Original Image", 700, 600);
	moveWindow("Original Image", 35, 100);

	int K , iter = 20;
	if (opt2 == 1)
		K = 16;
	else if (opt2 == 2)
		K = 32;
	else
		K = 64;
	const char* imname = imageName.c_str();
	FILE *f;
	f = fopen(imname, "rb");
	float size = 0;

	if (f == NULL)
		printf("error"); //handle error
	else
	{
		fseek(f, 0, SEEK_END);
		size = ftell(f);
	}
	float sizeo = size;
	char abc;
	if (argc > 1)
	{
		imageName = argv[1];
	}
	Mat image;
	image = imread(imageName, IMREAD_COLOR); // Read the file
	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	imshow("Original Image", image);
	int img_size[2];
	img_size[0] = image.rows;
	img_size[1] = image.cols;
	long int sizeX = img_size[0] * img_size[1];
	int ***A = (int ***)malloc(image.rows * sizeof(int**));
	for (int i = 0; i<image.rows; i++)
	{
		A[i] = (int **)malloc(image.cols * sizeof(int *));
		for (int j = 0; j<image.cols; j++)
		{
			A[i][j] = (int *)malloc(3 * sizeof(int));
			for (int k = 0; k<3; k++)
			{
				A[i][j][k] = (uint8_t)image.at<Vec3b>(i, j)[k];
				//std::cout << "Pixel at position "<<i<<","<< j <<": "<<(uint8_t) image.at<Vec3b>(i,j)[k]+0 << std::endl;
			}
		}
	}
	//scanf("%c", &abc);
	for (int i = 0; i<image.rows; i++)
	{
		for (int j = 0; j<image.cols; j++)
		{
			for (int k = 0; k<3; k++);

			// std::cout << "\nPixel at position "<<i<<","<< j <<": "<<A[i][j][k];
		}
	}
	int index = 0;
	int **B = (int **)malloc(img_size[0] * img_size[1] * sizeof(int *));
	//scanf("%c", &abc);
	for (int i = 0; i<img_size[0] * img_size[1]; i++)
		B[i] = (int *)malloc(3 * sizeof(int));


	for (int i = 0; i<image.rows; i++)
	{
		for (int j = 0; j<image.cols; j++)
		{
			for (int k = 0; k<3; k++)
			{
				B[index][k] = A[i][j][k];

			}
			index++;
		}
	}

	int i, j;
	int **initial_centroids = (int **)malloc(K * sizeof(int *));
	initial_centroids = KMeansInitCentroids(B, K, sizeX);

	A = runKMeans(B, initial_centroids, K, iter, sizeX, img_size);

	for (int i = 0; i<img_size[0]; i++)
	{
		for (int j = 0; j<img_size[1]; j++)
		{

			for (int k = 0; k<3; k++)
			{
				image.at<Vec3b>(i, j)[k] = (uint8_t)A[i][j][k];

			}
		}
	}


	namedWindow("Compressed Image", WINDOW_NORMAL); // Create a window for display.
	resizeWindow("Compressed Image", 700, 600);
	moveWindow("Compressed Image", 740, 100);
	imshow("Compressed Image", image);
	cvWaitKey(0);
	cv::imwrite("compressed.tiff", image);
	f = fopen("compressed.tiff", "rb");
	if (f == NULL)
		printf("error"); //handle error
	else
	{
		fseek(f, 0, SEEK_END);
		size = ftell(f);
	}
	float sizec = size;
	printf("\n\n........................The size of original image is: %f KB", sizeo / 1024);
	printf("\n\n.......................The size of the compressed image is: %f KB\n\n", sizec/1024);
	printf("\nThe ratio of file sizes is:  %f", sizeo / sizec);
	return 0;
}


int ** KMeansInitCentroids(int **B, int K, int sizeX)
{
	int **initial_centroids = (int **)malloc(K * sizeof(int *));
	for (int i = 0; i<K; i++)
		initial_centroids[i] = (int *)malloc(4 * sizeof(int));
	int i = 0;
	int j = i;
	for (i = 0; i<K; i++)
	{

		initial_centroids[i] = B[j];
		j = j + (int)sizeX / K;

	}
	return initial_centroids;
}



int *** runKMeans(int **B, int **initial_centroids, int K, int iter, int sizeX, int* img_size)
{
	float sum = 0;
	int **cent = (int **)malloc(K * sizeof(int *));
	for (int i = 0; i<K; i++)
		cent[i] = (int *)malloc(4 * sizeof(int));
	int i = 0;
	int j = i, index = 0;
	cent = initial_centroids;
	for (int i = 0; i < K; i++)
		cent[i][3] = 0;
	int *idx = (int *)malloc(sizeX * sizeof(int));
	for (i = 0; i<iter; i++)
	{
		idx = findClosestCentroids(B, cent, K, idx, sizeX, i);

		cent = computeCentroids(B, idx, K, sizeX, cent);
	}
	idx = findClosestCentroids(B, cent, K, idx, sizeX, i);

	for (int i = 0; i < K; i++)
	{

		printf("\nPixel value and Count of centroid %3d:  ", i + 1);
		printf("%4d, %4d, %4d:  %4d", cent[i][0], cent[i][1], cent[i][2], cent[i][3]);
	}
	
	int **X_rec = (int **)malloc(sizeX * sizeof(int *));
	for (int i = 0; i<sizeX; i++)
		X_rec[i] = (int *)malloc(3 * sizeof(int));

	int ***X_recovered = (int ***)malloc(img_size[0] * sizeof(int**));
	for (int i = 0; i<img_size[0]; i++)
	{
		X_recovered[i] = (int **)malloc(img_size[1] * sizeof(int *));
		for (int j = 0; j<img_size[1]; j++)
		{
			X_recovered[i][j] = (int *)malloc(3 * sizeof(int));
		}
	}

	for (int i = 0; i<sizeX; i++)
		for (int j = 0; j<3; j++)
			X_rec[i][j] = 0;
	for (int i = 0; i<K; i++)
	{
		for (int j = 0; j<sizeX; j++)
		{
			if (idx[j] == i)
			{
				for (int k = 0; k<3; k++)
				{
					X_rec[j][k] = (uint8_t)cent[i][k];
				}
			}
		}

	}

	for (int i = 0; i<img_size[0]; i++)
	{
		for (int j = 0; j<img_size[1]; j++)
		{
			for (int k = 0; k<3; k++)
			{
				X_recovered[i][j][k] = (uint8_t)X_rec[index][k];

			}
			index++;
		}
	}
	return X_recovered;
}


int * findClosestCentroids(int **B, int **cent, int K, int *idx, int sizeX, int p)
{

	int i, j, min = 0, min1 = 0;
	for (int i = 0; i < K; i++)
	{
		cent[i][3] = 0;
	}
	for (i = 0; i<sizeX; i++)
	{
		min = 0;
		idx[i] = 0;
		for (j = 0; j<3; j++)
		{
			min = min + abs(B[i][j] - cent[0][j]);
		}
		for (int k = 1; k<K; k++)
		{
			min1 = 0;
			for (j = 0; j<3; j++)
			{
				min1 = min1 + abs(B[i][j] - cent[k][j]);
			}
			if (min1<min)
			{

				min = min1;
				idx[i] = (uint8_t)k;
			}

		}
		cent[idx[i]][3]++;
	}
	return idx;
}

int ** computeCentroids(int **B, int *idx, int K, int sizeX, int **cent)
{

	int m[3];
	float p1[3];

	int *n = (int *)malloc(K * sizeof(int));
	for (int i = 0; i<K; i++)
		n[i] = 0;
	for (int i = 0; i<3; i++)
	{
		m[i] = 0;
		p1[i] = 0;
	}
	for (int i = 0; i<K; i++)
	{
		m[0] = m[1] = m[2] = 0;
		for (int j = 0; j<sizeX; j++)
		{
			if (idx[j] == i)
			{
				for (int k = 0; k<3; k++)
					m[k] = m[k] + B[j][k];
				n[i]++;
			}
		}
		if (n[i] != 0)
		{
			for (int j = 0; j<3; j++)
				p1[j] = m[j] / n[i];

			for (int j = 0; j<3; j++)
				cent[i][j] = (int)p1[j];
		}

	}
	return cent;
}
