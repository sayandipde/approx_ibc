#include <iostream>
//#include <sstream>
#include <opencv2/opencv.hpp>
#include "config.hpp"

int main(int argc, char **argv) {
	using namespace std;
	using namespace cv;

	if (argc != 2) {
		cout << "Usage: ./cpp_vrep_main pipeline_version\n";
		return -1;
	}
	// ------- simulation parameters ----------//		
	float simstep = 0.005; // V-rep simulation step time
	int simulation_time = 7;
	//string analysis = argv[2];
	//string env = argv[3];
	//string result_dir = "/home/sayandipde/Approx_IBC/final_app/results/";
	//string ret_ref = argv[4];
	//string Q_value = argv[5];
	//bool ret_reference = (ret_ref == "True");
	// cout << ret_reference << endl;
	
	// ------- single image wo control -------- //
	// vector<float> period_s {0.040, 0.025, 0.035, 0.040, 0.020, 0.035, 0.025, 0.020};
	// vector<float> tau_s {0.038, 0.0205, 0.0329, 0.0379, 0.0193, 0.0328, 0.0205, 0.0191};
	
	// ------- multiple image wo control -------- //
	// vector<float> period_s {0.040, 0.025, 0.030, 0.040, 0.020, 0.030, 0.020, 0.020};
	// vector<float> tau_s {0.0374, 0.0201, 0.0298, 0.0368, 0.0159, 0.0294, 0.0193, 0.0155};
	
	// ------- multiple image with control -------- //
	vector<float> period_s {0.040, 0.025, 0.035, 0.040, 0.020, 0.030, 0.020, 0.020, 0.040};
	vector<float> tau_s {0.0379, 0.0206, 0.0303, 0.0373, 0.0164, 0.0299, 0.0198, 0.0160, 0.0359};
	
	int pipeline_version = atoi(argv[1]);
	cout << "pipeline_version: v" << pipeline_version << endl;

	// simulation main loop parameters
    Mat img_vrep, img_isp;
	vector<long double> steering_angles(2);
	float time = 0.0f;
	float curr_period = 0.0f;
	long double yL = 0.0L; // lateral deviation
    int it_counter = 0;
	int time_step;
	// ------- init simulation ----------//

	vrepAPI VrepAPI;
    laneDetection Lane_detection;
    lateralController Controller;
    //IBCController Controller;
    imageSignalProcessing ISP;

	// --- delay 2.5 sec to reach desired velocity ---//		
	//time_step = 2.5 / simstep; 
	//VrepAPI.sim_delay(time_step);
	curr_period = period_s[pipeline_version];

	// ------- simulation main loop -------//
	cout << "control period :" << curr_period << " msec" << endl;
	
	while(time < simulation_time - curr_period) {
		cout << "simulation_time: " << time << "\t";
		// ------- sensing ----------//	
		img_vrep = VrepAPI.sim_sense();
	
		// ------- imageSignalProcessing ----------//
		img_isp = ISP.approximate_pipeline(img_vrep, pipeline_version);
			
		// ------- laneDetection -----------------//
		yL = Lane_detection.lane_detection_pipeline(img_isp);
		cout << "lateral deviation: " << yL << endl;

		// ------- control_compute --------------//
		Controller.compute_steering_angles(yL, it_counter, pipeline_version);
		steering_angles = Controller.get_steering_angles();

		// ------- control_next_state  -----------//
		Controller.estimate_next_state(it_counter, pipeline_version);
			
		// ------- sensor-to-actuator delay ----------//
		time_step = tau_s[pipeline_version] / simstep;
			
		VrepAPI.sim_delay(time_step);  

		// ----- actuating -----//
		// [TODO] add actuation delay here (too small, neglected)
		VrepAPI.sim_actuate(steering_angles); 

		// ------- remaining delay to sampling period ----------//
		time_step = (period_s[pipeline_version] - tau_s[pipeline_version])/ simstep;
		VrepAPI.sim_delay(time_step);  


		// ------- handle simulation time -------//
		time += period_s[pipeline_version];
		
		it_counter++;
	}
	
	cout << "images: " << it_counter << endl; 

	return 0;
}
