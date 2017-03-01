/*#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/Image.h>


//looking for home directory
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>

//file libraries
#include <iostream>
#include <fstream>




*/
/*
using namespace cv;
std::string get_directory(std::string file_name, std::string file_num, std::string file_type);
int main() {

	//START opening file for log of position
	std::ofstream create_pose;
	std::string create_pose_directory;
	create_pose_directory = get_directory("izba", "", "mp4");
	//END opening file for log of position

    Mat image;          //Create Matrix to store image
    VideoCapture cap;          //initialize capture
    cap.open(0);//create_pose_directory.c_str());
    namedWindow("window", 1);          //create window to show image
    while (1) {
        cap >> image;          //copy webcam stream to image
        imshow("window", image);          //print image to screen
        waitKey(33);          //delay 33ms
    }
    return 0;
}
*/


/*class CvImage
{
  sensor_msgs::ImagePtr toImageMsg() const;

  // Overload mainly intended for aggregate messages that contain
  // a sensor_msgs::Image as a member.
  void toImageMsg(sensor_msgs::Image& ros_image) const;
};


using namespace cv;

std::string get_directory(std::string file_name, std::string file_num, std::string file_type);

int main(int argc, char **argv) {
	//START ros init
	ros::init(argc, argv, "get_video_image");
	ros::NodeHandle n;

	cv_bridge::CvImage img_bridge;
	sensor_msgs::Image img_msg;

	ros::Publisher pub_img = n.advertise<sensor_msgs::Image>("camera_image_topic", 1);
	//END ros init

	//START opening file for log of position
	std::ofstream create_pose;
	std::string create_pose_directory;
	create_pose_directory = get_directory("obed", "", "mp4");
	//END opening file for log of position

	Mat image;          //Create Matrix to store image
	Mat imageResult;
    VideoCapture cap;          //initialize capture
    cap.open(create_pose_directory.c_str());
    std_msgs::Header header;
    //namedWindow("window", 1);          //create window to show image
    //cv_bridge::CvImagePtr cv_ptr;

    while (ros::ok()){
    	cap >> image;
			cv::blur(image,imageResult,cv::Size(2,2));
			//cv::sharpen(image,imageResult,cv::Size(2,2));
        header.stamp = ros::Time::now();

        img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, imageResult);
        img_bridge.toImageMsg(img_msg);
        pub_img.publish(img_msg);
        //imshow("window", image);          //print image to screen
        //publish
        //waitKey(40);          //delay 33ms
        usleep(40000);
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
}*/


#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/Image.h>


//looking for home directory
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>

//file libraries
#include <iostream>
#include <fstream>

class CvImage
{
  sensor_msgs::ImagePtr toImageMsg() const;

  // Overload mainly intended for aggregate messages that contain
  // a sensor_msgs::Image as a member.
  void toImageMsg(sensor_msgs::Image& ros_image) const;
};


using namespace cv;

std::string get_directory(std::string file_name, std::string file_num, std::string file_type);

int main(int argc, char **argv) {
	//START ros init
	ros::init(argc, argv, "get_video_image");
	ros::NodeHandle n;

	cv_bridge::CvImage img_bridge;
	sensor_msgs::Image img_msg;

	ros::Publisher pub_img = n.advertise<sensor_msgs::Image>("camera_image_topic", 1);
	//END ros init

	//START opening file for log of position
	std::ofstream create_pose;
	std::string create_pose_directory;
	create_pose_directory = get_directory("obed", "", "mp4");
	//END opening file for log of position

	Mat image;          //Create Matrix to store image
	Mat imageResult;
	VideoCapture cap;          //initialize capture
	cap.open(create_pose_directory.c_str());
	std_msgs::Header header;

	while (ros::ok()){
		cap >> image;
		
		cv::blur(image,imageResult,cv::Size(60,60));//blurr image
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
					imageResult.at<cv::Vec3b>(j,i)[1] = 255;
					imageResult.at<cv::Vec3b>(j,i)[2] = 255;
				}
				else
				{
					imageResult.at<cv::Vec3b>(j,i)[0] = 0;
				}
			}
		}
		

		//highlight red areas
			//increase contrast

		//publish processed image
		header.stamp = ros::Time::now();
		img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, imageResult);
		img_bridge.toImageMsg(img_msg);
		pub_img.publish(img_msg);
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

