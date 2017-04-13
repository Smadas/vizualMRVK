//FANN libraries
#include "fann.h"
#include "floatfann.h"

//files libraries
#include <iostream>
#include <fstream>

//looking for home directory
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string> 

//opencv
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/Image.h>

std::string get_directory(std::string file_name, std::string file_num, std::string file_type);

int main()
{
	int num_layers = 3;
	int num_input = 3;
	int num_neurons_hidden = 50;
	int num_output = 3;
	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	//START set training parameters
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_learning_momentum(ann, 0.9);
	fann_set_learning_rate(ann, 0.6);
	//END set training parameters

	int desired_error = 0.1;
	int epochs_between_reports = 1;
	int max_epochs = 100;
	fann_train_on_file(ann, get_directory("trainData", "", "txt").c_str(), max_epochs, epochs_between_reports, desired_error);

	std::cout << "trololo working" << std::endl;

	return 0;
}

std::string get_directory(std::string file_name, std::string file_num, std::string file_type)
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string file_directory;
	file_directory = homedir;
	file_directory = file_directory + "/catkin_opencv/src/opencv_test/src/" + file_name + file_num + "." + file_type;
	return file_directory;
}

/*
tvar vstupenho suboru
pocetDatovychVektorov pocetVstupov pocetVystupov
vstupnyVektor1
vystupnyVektor1
.
.
.
*/
