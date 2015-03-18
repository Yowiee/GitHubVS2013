// Minimallist Hello World example

//#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>

#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <ExpSmooth.hpp>
#include <HRTimer.hpp>

using namespace cv;
using namespace std;

int main() {
	//Start and end times
	time_t tStart, tEnd;

	// smoothed FPS
	EXPSMOOTH esFrameTime(0.2);

	// process timer
	HRTIMER ProcTimer;

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

	time(&tStart);
	while (1)
	{
		ProcTimer.Start();

		bool bSuccess = cap.read(c_frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		// process frame
		imshow("Raw Video", c_frame); //show the frame in "Raw Video" window

		cv::setTrackbarPos(tbStats, wStats, ++cntFrame);

		if (cv::waitKey(20) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << endl << "Press any key to exit" << endl;
			break;
		}

		double deltaT = ProcTimer.Stop();
		esFrameTime.AddData(deltaT);
		cout << std::fixed << std::setprecision(2) << 1.0 / esFrameTime.Read() << endl;
	}

	// wait for second key press before exiting
	cv::waitKey();

	destroyAllWindows();
	cap.release();

	return 0;
}

