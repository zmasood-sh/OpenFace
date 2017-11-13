///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017, Tadas Baltrusaitis, all rights reserved.
//
// ACADEMIC OR NON-PROFIT ORGANIZATION NONCOMMERCIAL RESEARCH USE ONLY
//
// BY USING OR DOWNLOADING THE SOFTWARE, YOU ARE AGREEING TO THE TERMS OF THIS LICENSE AGREEMENT.  
// IF YOU DO NOT AGREE WITH THESE TERMS, YOU MAY NOT USE OR DOWNLOAD THE SOFTWARE.
//
// License can be found in OpenFace-license.txt
//
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite at least one of the following works:
//
//       OpenFace: an open source facial behavior analysis toolkit
//       Tadas Baltru�aitis, Peter Robinson, and Louis-Philippe Morency
//       in IEEE Winter Conference on Applications of Computer Vision, 2016  
//
//       Rendering of Eyes for Eye-Shape Registration and Gaze Estimation
//       Erroll Wood, Tadas Baltru�aitis, Xucong Zhang, Yusuke Sugano, Peter Robinson, and Andreas Bulling 
//       in IEEE International. Conference on Computer Vision (ICCV),  2015 
//
//       Cross-dataset learning and person-speci?c normalisation for automatic Action Unit detection
//       Tadas Baltru�aitis, Marwa Mahmoud, and Peter Robinson 
//       in Facial Expression Recognition and Analysis Challenge, 
//       IEEE International Conference on Automatic Face and Gesture Recognition, 2015 
//
//       Constrained Local Neural Fields for robust facial landmark detection in the wild.
//       Tadas Baltru�aitis, Peter Robinson, and Louis-Philippe Morency. 
//       in IEEE Int. Conference on Computer Vision Workshops, 300 Faces in-the-Wild Challenge, 2013.    
//
///////////////////////////////////////////////////////////////////////////////

#include "RecorderCSV.h"

// For sorting
#include <algorithm>

// For standard out
#include <iostream>

using namespace Utilities;

// Default constructor initializes the variables
RecorderCSV::RecorderCSV():output_file(){};

// TODO the other 4 constructors + destructors?

// Opening the file and preparing the header for it
bool RecorderCSV::Open(std::string output_file_name, bool is_sequence, bool output_2D_landmarks, bool output_3D_landmarks, bool output_model_params, bool output_pose, bool output_AUs, bool output_gaze,
	int num_face_landmarks, int num_model_modes, int num_eye_landmarks, const std::vector<std::string>& au_names_class, const std::vector<std::string>& au_names_reg)
{

	output_file.open(output_file_name, std::ios_base::out);

	if (!output_file.is_open())
		return false;

	this->is_sequence = is_sequence;

	// Set up what we are recording
	this->output_2D_landmarks = output_2D_landmarks;
	this->output_3D_landmarks = output_3D_landmarks;
	this->output_AUs = output_AUs;
	this->output_gaze = output_gaze;
	this->output_model_params = output_model_params;
	this->output_pose = output_pose;

	this->au_names_class = au_names_class;
	this->au_names_reg = au_names_reg;

	// Different headers if we are writing out the results on a sequence or an individual image
	if(this->is_sequence)
	{
		output_file << "frame, timestamp, confidence, success";
	}
	else
	{
		output_file << "face, confidence";
	}

	if (output_gaze)
	{
		output_file << ", gaze_0_x, gaze_0_y, gaze_0_z, gaze_1_x, gaze_1_y, gaze_1_z, gaze_angle_x, gaze_angle_y";

		for (int i = 0; i < num_eye_landmarks; ++i)
		{
			output_file << ", eye_lmk_x_" << i;
		}
		for (int i = 0; i < num_eye_landmarks; ++i)
		{
			output_file << ", eye_lmk_y_" << i;
		}

		for (int i = 0; i < num_eye_landmarks; ++i)
		{
			output_file << ", eye_lmk_X_" << i;
		}
		for (int i = 0; i < num_eye_landmarks; ++i)
		{
			output_file << ", eye_lmk_Y_" << i;
		}
		for (int i = 0; i < num_eye_landmarks; ++i)
		{
			output_file << ", eye_lmk_Z_" << i;
		}
	}

	if (output_pose)
	{
		output_file << ", pose_Tx, pose_Ty, pose_Tz, pose_Rx, pose_Ry, pose_Rz";
	}

	if (output_2D_landmarks)
	{
		for (int i = 0; i < num_face_landmarks; ++i)
		{
			output_file << ", x_" << i;
		}
		for (int i = 0; i < num_face_landmarks; ++i)
		{
			output_file << ", y_" << i;
		}
	}

	if (output_3D_landmarks)
	{
		for (int i = 0; i < num_face_landmarks; ++i)
		{
			output_file << ", X_" << i;
		}
		for (int i = 0; i < num_face_landmarks; ++i)
		{
			output_file << ", Y_" << i;
		}
		for (int i = 0; i < num_face_landmarks; ++i)
		{
			output_file << ", Z_" << i;
		}
	}

	// Outputting model parameters (rigid and non-rigid), the first parameters are the 6 rigid shape parameters, they are followed by the non rigid shape parameters
	if (output_model_params)
	{
		output_file << ", p_scale, p_rx, p_ry, p_rz, p_tx, p_ty";
		for (int i = 0; i < num_model_modes; ++i)
		{
			output_file << ", p_" << i;
		}
	}

	if (output_AUs)
	{
		std::sort(this->au_names_reg.begin(), this->au_names_reg.end());
		for (std::string reg_name : this->au_names_reg)
		{
			output_file << ", " << reg_name << "_r";
		}

		std::sort(this->au_names_class.begin(), this->au_names_class.end());
		for (std::string class_name : this->au_names_class)
		{
			output_file << ", " << class_name << "_c";
		}
	}

	output_file << std::endl;

	return true;

}

// TODO check if the stream is open
void RecorderCSV::WriteLine(int observation_count, double time_stamp, bool landmark_detection_success, double landmark_confidence,
	const cv::Mat_<double>& landmarks_2D, const cv::Mat_<double>& landmarks_3D, const cv::Mat_<double>& pdm_model_params, const cv::Vec6d& rigid_shape_params, cv::Vec6d& pose_estimate,
	const cv::Point3f& gazeDirection0, const cv::Point3f& gazeDirection1, const cv::Vec2d& gaze_angle, const std::vector<cv::Point2d>& eye_landmarks2d, const std::vector<cv::Point3d>& eye_landmarks3d,
	const std::vector<std::pair<std::string, double> >& au_intensities, const std::vector<std::pair<std::string, double> >& au_occurences)
{

	if (!output_file.is_open())
	{
		std::cout << "The output CSV file is not open" << std::endl;
	}

	if(is_sequence)
	{
		output_file << observation_count << ", " << time_stamp << ", " << landmark_confidence << ", " << landmark_detection_success;
	}
	else
	{
		output_file << observation_count << ", " << landmark_confidence;

	}
	// Output the estimated gaze
	if (output_gaze)
	{
		output_file << ", " << gazeDirection0.x << ", " << gazeDirection0.y << ", " << gazeDirection0.z
			<< ", " << gazeDirection1.x << ", " << gazeDirection1.y << ", " << gazeDirection1.z;

		// Output gaze angle (same format as head pose angle)
		output_file << ", " << gaze_angle[0] << ", " << gaze_angle[1];

		// Output the 2D eye landmarks
		for (auto eye_lmk : eye_landmarks2d)
		{
			output_file << ", " << eye_lmk.x;
		}

		for (auto eye_lmk : eye_landmarks2d)
		{
			output_file << ", " << eye_lmk.y;
		}

		// Output the 3D eye landmarks
		for (auto eye_lmk : eye_landmarks3d)
		{
			output_file << ", " << eye_lmk.x;
		}

		for (auto eye_lmk : eye_landmarks3d)
		{
			output_file << ", " << eye_lmk.y;
		}

		for (auto eye_lmk : eye_landmarks3d)
		{
			output_file << ", " << eye_lmk.z;
		}
	}

	// Output the estimated head pose
	if (output_pose)
	{
		output_file << ", " << pose_estimate[0] << ", " << pose_estimate[1] << ", " << pose_estimate[2]
				<< ", " << pose_estimate[3] << ", " << pose_estimate[4] << ", " << pose_estimate[5];
	}

	// Output the detected 2D facial landmarks
	if (output_2D_landmarks)
	{
		// Output the 2D eye landmarks
		for (auto lmk : landmarks_2D)
		{
			output_file << ", " << lmk;
		}
	}

	// Output the detected 3D facial landmarks
	if (output_3D_landmarks)
	{
		// Output the 2D eye landmarks
		for (auto lmk : landmarks_3D)
		{
			output_file << ", " << lmk;
		}
	}

	if (output_model_params)
	{
		for (int i = 0; i < 6; ++i)
		{
			output_file << ", " << rigid_shape_params[i];
		}
		// Output the non_rigid shape parameters
		for (auto lmk : pdm_model_params)
		{
			output_file << ", " << lmk;
		}
	}

	if (output_AUs)
	{

		// write out ar the correct index
		for (std::string au_name : au_names_reg)
		{
			for (auto au_reg : au_intensities)
			{
				if (au_name.compare(au_reg.first) == 0)
				{
					output_file << ", " << au_reg.second;
					break;
				}
			}
		}

		if (au_intensities.size() == 0)
		{
			for (size_t p = 0; p < au_names_reg.size(); ++p)
			{
				output_file << ", 0";
			}
		}

		// write out ar the correct index
		for (std::string au_name : au_names_class)
		{
			for (auto au_class : au_occurences)
			{
				if (au_name.compare(au_class.first) == 0)
				{
					output_file << ", " << au_class.second;
					break;
				}
			}
		}

		if (au_occurences.size() == 0)
		{
			for (size_t p = 0; p < au_names_class.size(); ++p)
			{
				output_file << ", 0";
			}
		}
	}
	output_file << std::endl;
}

// Closing the file and cleaning up
void RecorderCSV::Close()
{
	output_file.close();
}