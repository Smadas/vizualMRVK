#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/Image.h>


//#include <opencv\cv.h>   // This is the original code, but I couldn't get VideoCapture work correctly.
//#include <opencv2/opencv.hpp>
//#include <opencv\highgui.h>


class CvImage
{
  sensor_msgs::ImagePtr toImageMsg() const;

  // Overload mainly intended for aggregate messages that contain
  // a sensor_msgs::Image as a member.
  void toImageMsg(sensor_msgs::Image& ros_image) const;
};




using namespace cv;

int main(int argc, char **argv) {
	//START ros init

	ros::init(argc, argv, "get_camera_image");
	ros::NodeHandle n;

	cv_bridge::CvImage img_bridge;
	sensor_msgs::Image img_msg;

	ros::Publisher pub_img = n.advertise<sensor_msgs::Image>("/camera_image_topic", 1);
	//image_transport::ImageTransport it_;
	//image_transport::Publisher camera_image_pub = it_.advertise("camera_image_topic", 1);
	//END ros init

	Mat image;          //Create Matrix to store image
    VideoCapture cap;          //initialize capture
    cap.open(0);
    std_msgs::Header header;
    //cv_bridge::CvImagePtr cv_ptr;

    while (ros::ok()){
    	cap >> image;
        header.stamp = ros::Time::now();
        img_bridge = cv_bridge::CvImage(header, sensor_msgs::image_encodings::RGB8, image);
        img_bridge.toImageMsg(img_msg);
        pub_img.publish(img_msg);
        //publish
        waitKey(33);          //delay 33ms
    }
    return 0;
}


