#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/Image.h>

//octomap - pavement
#include <nav_msgs/OccupancyGrid.h>


//looking for home directory
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>

//file libraries
#include <iostream>
#include <fstream>

#define EDGE_MARKER_WIDTH 6
#define EDGE_MARKER_TYPE 8
#define EDGE_MARKER_SHIFT 0

#define EDGE_MARKER_VERTIKAL_START 130
#define EDGE_MARKER_VERTIKAL_END 670
#define EDGE_MARKER_VERTIKAL_POINT_DIST 25

class CvImage
{
  sensor_msgs::ImagePtr toImageMsg() const;

  // Overload mainly intended for aggregate messages that contain
  // a sensor_msgs::Image as a member.
  void toImageMsg(sensor_msgs::Image& ros_image) const;
};

using namespace cv;

std::string get_directory(std::string file_name, std::string file_num, std::string file_type);
int getLeftPavementPoint(cv::Mat image, int line);
int getRightPavementPoint(cv::Mat image, int line);

int main(int argc, char **argv) {
	//START ros init
	ros::init(argc, argv, "get_video_image_meddle");
	ros::NodeHandle n;

	cv_bridge::CvImage img_bridge;
	sensor_msgs::Image img_msg;
	sensor_msgs::Image img_msg_orig;
	std_msgs::Header header;

	ros::Publisher pub_img = n.advertise<sensor_msgs::Image>("video_image_topic", 1);//output image publisher
	ros::Publisher pub_img_orig = n.advertise<sensor_msgs::Image>("video_image_orig_topic", 1);//output image publisher
	ros::Publisher octomap_pub = n.advertise<nav_msgs::OccupancyGrid>("pavement_map", 1);//map occupancy publisher
	//END ros init

	//init occupancy map
	nav_msgs::OccupancyGrid pavement_octomap;
	pavement_octomap.info.resolution = 0.1;         	// float32
	pavement_octomap.info.width      = 100;           // uint32
	pavement_octomap.info.height     = 1000;           // uint32

	long map[100000];//0 - nothing, 100 - obstacle, 50 - uknown
	for(long i = 0; i < 100000; i++)
	{
		map[i] = 50;
	}
	std::vector<signed char> a(map, map + 100000);
	pavement_octomap.data = a;
		//1m equals to one sqare - with 30fps video moves 0.03m per frame
		//1m eqals 500px at bottom
		//1m eqals 250px in the middle
		//1m eqals 50px at top
	int elapsedDistanceDiscrete = 0;
	int threeToOneSqare = 0;
	//END init occcupancy map

	//START opening file for log of position
	std::ofstream create_pose;
	std::string create_pose_directory;
	create_pose_directory = get_directory("obed", "", "mp4");
	//END opening file for log of position

	Mat image;          //Create Matrix to store image
	Mat imageResult;		//Create Matrix to store processed image
	Mat imageOrig;			//Create Matrix to store orig image with detected pavement

	VideoCapture cap;          //initialize capture
	cap.open(create_pose_directory.c_str());

	//START edge detection variables	
	Point lineStart = Point(100, 100);
	Point lineEnd = Point(300, 300);
	int num_of_edge_points = (EDGE_MARKER_VERTIKAL_END - EDGE_MARKER_VERTIKAL_START - EDGE_MARKER_VERTIKAL_POINT_DIST)/EDGE_MARKER_VERTIKAL_POINT_DIST;
	int leftPoint = 0;
	int rightPoint = 0;
	//END edge detection variables

	while (ros::ok())
	{
		cap >> image;
		imageOrig = image;
		
		cv::blur(image,imageResult,cv::Size(60, 60));//blurr image
		//remove red and blue
		for(int i = 0; i < imageResult.size().width; i++)
		{
			for(int j = 0; j < imageResult.size().height; j++)
			{
				//imageResult.at<cv::Vec3b>(j,i)[0]= 0;
				imageResult.at<cv::Vec3b>(j,i)[1]= 0;
				imageResult.at<cv::Vec3b>(j,i)[2]= 0;
				//increase contrast
				if (imageResult.at<cv::Vec3b>(j,i)[0] > 120)
				{
					imageResult.at<cv::Vec3b>(j,i)[0] = 255;
				}
				else
				{
					imageResult.at<cv::Vec3b>(j,i)[0] = 0;
				}
			}
		}
		
		/*//START robot moved forward
		threeToOneSqare++;
		if (threeToOneSqare > 2)
		{
			threeToOneSqare = 0;
			elapsedDistanceDiscrete++;
			//START perspective - draw pavement to map
			if (elapsedDistanceDiscrete < 999)//check if end of map
			{
				//1m equals to one sqare - with 30fps video moves 0.03m per frame
				//1m eqals 500px at bottom
				//1m eqals 250px in the middle
				//1m eqals 50px at top	
				//map[20 + 100*elapsedDistanceDiscrete] = 100;//demo
				//map[60 + 100*elapsedDistanceDiscrete] = 100;//demo
				leftPoint = getLeftPavementPoint(imageResult, 700);
				rightPoint = getRightPavementPoint(imageResult, 700);
					//to do - check if computed index is valid
				//map[(int)((double)leftPoint/1.2) + 100*elapsedDistanceDiscrete] = 100;
					//to do - check if computed index is valid
				//map[(int)((double)rightPoint/1.2) + 100*elapsedDistanceDiscrete] = 100;
				std::vector<signed char> a(map, map + 100000);
				pavement_octomap.data = a;
			}
			//END perspective - draw pavement to map
		}
		//END robot moved forward*/

		//START draw pavement boundaries
			//left
		lineStart = Point( getLeftPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START), EDGE_MARKER_VERTIKAL_START);
		lineEnd = Point( getLeftPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST), EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST);
		//to do - prepocet
		//treba pocitat od stredu obrazu - tam robot smeruje
		//lineRealCoordStart = coordConvPerspective(lineStart);
		//lineRealCoordEND = coordConvPerspective(lineEnd);
		//putPavementLineIntoMarker(map, lineRealCoordStart, lineRealCoordENDm);
		line(imageResult, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		line(imageOrig, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		for (int i = 0; i < num_of_edge_points; i++)
		{
			lineStart = lineEnd;
			lineEnd = Point( getLeftPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST + EDGE_MARKER_VERTIKAL_POINT_DIST*i), EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST + EDGE_MARKER_VERTIKAL_POINT_DIST*i);
			line(imageResult, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
			line(imageOrig, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		}

			//right
		lineStart = Point( getRightPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START), EDGE_MARKER_VERTIKAL_START);
		lineEnd = Point( getRightPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST), EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST);
		line(imageResult, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		line(imageOrig, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		for (int i = 0; i < num_of_edge_points; i++)
		{
			lineStart = lineEnd;
			lineEnd = Point( getRightPavementPoint(imageResult, EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST + EDGE_MARKER_VERTIKAL_POINT_DIST*i), EDGE_MARKER_VERTIKAL_START + EDGE_MARKER_VERTIKAL_POINT_DIST + EDGE_MARKER_VERTIKAL_POINT_DIST*i);
			line(imageResult, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
			line(imageOrig, lineStart, lineEnd, Scalar(0, 255, 0), EDGE_MARKER_WIDTH, EDGE_MARKER_TYPE, EDGE_MARKER_SHIFT);
		}
		//END draw pavement boundaries

		//START perspective conversion
		//put pavement to octomap
		std::vector<signed char> a(map, map + 100000);
		pavement_octomap.data = a;
		//END perspective conversion

		//publish processed image
		header.stamp = ros::Time::now();
		img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, imageResult);
		img_bridge.toImageMsg(img_msg);
		pub_img_orig.publish(img_msg);//publish processed image
		img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, imageOrig);
		img_bridge.toImageMsg(img_msg_orig);
		pub_img.publish(img_msg_orig);//publish original image
		octomap_pub.publish(pavement_octomap);//publish octomap
		//usleep(10000);
    }

    return 0;
}

std::string get_directory(std::string file_name, std::string file_num, std::string file_type)
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string file_directory;
	file_directory = homedir;
	file_directory = file_directory + "/Videos/MRVKroute/" + file_name + file_num + "." + file_type;
	return file_directory;
}

int getLeftPavementPoint(cv::Mat image, int line)
{
	int isGrass = 0;
	int isEdge = 0;
	for (int i = 0; i < image.size().width; i++)
	{
		if (image.at<cv::Vec3b>(line,i)[0] == 0)
		{
			isGrass = 1;
		}
		else if ((image.at<cv::Vec3b>(line,i)[0] == 255) && (isGrass == 1) && (isEdge == 0))
		{
			isGrass = 0;
			isEdge = 1;
			return i;
		}
	}
	return 0;
}

int getRightPavementPoint(cv::Mat image, int line)
{
	int isGrass = 0;
	int isEdge = 0;
	for (int i = image.size().width; i > 0; i--)
	{
		if (image.at<cv::Vec3b>(line,i)[0] == 0)
		{
			isGrass = 1;
		}
		else if ((image.at<cv::Vec3b>(line,i)[0] == 255) && (isGrass == 1) && (isEdge == 0))
		{
			isGrass = 0;
			isEdge = 1;
			return i;
		}
	}
	return 0;
}

Point coordConvPerspective(Point imagePoint)//real coordinates in cm
{
	Point autobus;
	autobus.x = 10;
	autobus.y = 10;
	return autobus;
}

void putPavementLineIntoMarker(int map[100000], Point lineRealCoordStart, Point lineRealCoordENDm) //real coordinates in mm
{
	
}
