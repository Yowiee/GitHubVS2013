// Minimallist Hello World example

//#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>

#include <opencv\highgui.h>
#include <opencv\cv.h>

using namespace cv;
using namespace std;

#if defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) \
    || defined(WIN64)    || defined(_WIN64) || defined(__WIN64__) 

#include <windows.h>
bool _qpcInited = false;
double PCFreq = 0.0;
__int64 CounterStart = 0;
void InitCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		std::cout << "QueryPerformanceFrequency failed!\n";
	}
	PCFreq = double(li.QuadPart) / 1000.0f;
	_qpcInited = true;
}
double CLOCK()
{
	if (!_qpcInited) InitCounter();
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart) / PCFreq;
}

#endif

#if defined(unix)        || defined(__unix)      || defined(__unix__) \
    || defined(linux)       || defined(__linux)     || defined(__linux__) \
    || defined(sun)         || defined(__sun) \
    || defined(BSD)         || defined(__OpenBSD__) || defined(__NetBSD__) \
    || defined(__FreeBSD__) || defined __DragonFly__ \
    || defined(sgi)         || defined(__sgi) \
    || defined(__MACOSX__)  || defined(__APPLE__) \
    || defined(__CYGWIN__) 
double CLOCK()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_sec * 1000) + (t.tv_nsec*1e-6);
}
#endif

double _avgdur = 0;
double _fpsstart = 0;
double _avgfps = 0;
double _fps1sec = 0;

double avgdur(double newdur)
{
	_avgdur = 0.98*_avgdur + 0.02*newdur;
	return _avgdur;
}

double avgfps()
{
	if (CLOCK() - _fpsstart>1000)
	{
		_fpsstart = CLOCK();
		_avgfps = 0.7*_avgfps + 0.3*_fps1sec;
		_fps1sec = 0;
	}
	_fps1sec++;
	return _avgfps;
}

int main() {
	//Start and end times
	time_t tStart, tEnd;

	const string wRawVideo = "Raw Video";
	const string wStats = "Stats";
	const string tbStats = "tb" + wStats;

	VideoCapture cap("..\\MovingCar1.mp4"); // open the video file
	if (!cap.isOpened())
	{
		// check if we succeeded
		return -1;
	}

	// get some video states
	int c_fps = (int)cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	int c_width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of the frames
	int c_height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of the frames
	int c_frameCnt = (int)cap.get(CV_CAP_PROP_FRAME_COUNT); //get the number of frames

	int cntFrame = 0;

	cout << "Frame size : " << c_width << ", " << c_height << endl;
	cout << "Frame per seconds : " << c_fps << endl;
	cout << "No Frames : " << c_frameCnt << endl;

	Mat c_frame;
	cv::namedWindow(wRawVideo, CV_WINDOW_AUTOSIZE);

	cv::namedWindow(wStats, CV_WINDOW_AUTOSIZE);
	cv::createTrackbar(tbStats, wStats, &cntFrame, c_frameCnt);

	while (1)
	{
		double start = CLOCK();
		time(&tStart);

		bool bSuccess = cap.read(c_frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		// process frame
		imshow("Raw Video", c_frame); //show the frame in "Raw Video" window

		cv::setTrackbarPos(tbStats, wStats, ++cntFrame);

		if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << endl << "Press any key to exit" << endl;
			break;
		}

		time(&tEnd);
		double dur = CLOCK() - start;
		avgdur(dur);
		cout << (cntFrame / difftime(tEnd, tStart)) << ", " << avgfps() << endl;
	}

	// wait for second key press before exiting
	cv::waitKey();

	destroyAllWindows();
	cap.release();

	return 0;
}

