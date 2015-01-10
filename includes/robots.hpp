#include "utils.hpp"
#include "state.hpp"

#include <iostream>

#ifndef __ROBOTS_HPP__
#define __ROBOTS_HPP__

typedef Eigen::Matrix<double,U_DIM,1> control;

class Robot
{
public:
	Robot() {
		int temp;
		CAL_CreateGroup(&(this->robot_model), 0, false, "Robot Model");
		CAL_CreateGroup(&(this->robot_group), 0, true, "Robot Group");

		CAL_SetGroupVisibility(this->robot_group, 0, false, true);
		CAL_SetGroupColor(this->robot_group, 0, 0, 0);

		this->hide();
	}

	void show() {
		CAL_SetGroupVisibility(this->robot_model, 0, true, true);
	}

	void hide() {
		CAL_SetGroupVisibility(this->robot_model, 0, false, false);
	}

	virtual void rotate(const state& s) = 0;

	virtual void position(const state& s) {
		int x_pos = s[0];
		int y_pos = s[1];
#if POSITION_DIM == 3
		int z_pos = s[2];
#else
		int z_pos = 0;
#endif

		int result = CAL_SetGroupPosition(this->robot_group, x_pos, y_pos, z_pos);
		if (result != CAL_SUCCESS) {
			std::cout << "CAL_SetObjectPosition failed (" << result << ")." << std::endl;
			_getchar();
			exit(1);
		}
	}

	virtual int checkCollision(const int obstacle_group, int * collisions) {
		return CAL_CheckGroupCollision(this->robot_group, obstacle_group, false, collisions);
	}

protected:
	int robot_model;
	int robot_group;
	int robot_object;

	void _rotate(const Eigen::Quaternion<double>& q) {
		int result = CAL_SetGroupQuaternion(robot_group,(float)q.x(),(float)q.y(),(float)q.z(),(float)q.w());
		if (CAL_SUCCESS != result) {
			std::cout << "CAL_SetGroupQuaternion failed (" << result << ")" << std::endl;
			_getchar();
			exit(1);
		}
	}
};

class Puck
	: public Robot
{
public:
	Puck() : Robot()
	{
		CAL_CreateCylinder(this->robot_group, 5, 3, 0, 0, 0, &robot_object);
		CAL_CreateCylinder(this->robot_model, 5, 3, 0, 0, 0);
		CAL_SetGroupColor(this->robot_model, 0.05, 0.05, 0.05);
		CAL_SetObjectOrientation(this->robot_object, M_PI/2.0, 0, 0);
	}

	virtual void rotate(const state& s) {}
};

class Quadrotor : public Robot
{
public:
	Quadrotor() : Robot()
	{
		int obj;

		// Visualization parameters
		double beamWidth     = 0.015; // m
		double beamHeight    = 0.0065; // m
		double beamRadius    = 0.02; // m
		double motorRadius   = 0.015; // m
		double motorHeight   = 0.02; // m
		double rotorRadius   = 0.10; // m
		double rotorHeight   = 0.005; // m
		double centerSide    = 0.0889; // m
		double centerHeight  = 0.0365; // m
		double centerTopSide = 0.03; // m
		double flagLength    = 0.0508; // m
		double tileSize      = 1;  // m

		CAL_CreateCylinder(this->robot_group, length+rotorRadius, length/2.0, 0, 0, 0, &this->robot_object);
		CAL_SetGroupOrientation(this->robot_group, M_PI/2.0, 0, 0);

		// Quadrotor
		CAL_SetGroupColor(this->robot_model, 0.05, 0.05, 0.05);
		CAL_CreateBox(this->robot_model, 2*length, beamWidth, beamHeight, 0, 0, 0);
		CAL_CreateBox(this->robot_model, beamWidth, 2*length, beamHeight, 0, 0, 0);
		CAL_CreateCylinder(this->robot_model, motorRadius, motorHeight, length, 0, beamHeight / 2 + motorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, motorRadius, motorHeight, -length, 0, beamHeight / 2 + motorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, motorRadius, motorHeight, 0, length, beamHeight / 2 + motorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, motorRadius, motorHeight, 0, -length, beamHeight / 2 + motorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, beamRadius, beamHeight, length, 0, 0, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, beamRadius, beamHeight, -length, 0, 0, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, beamRadius, beamHeight, 0, length, 0, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, beamRadius, beamHeight, 0, -length, 0, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_CreateCylinder(this->robot_model, rotorRadius, rotorHeight, length, 0, beamHeight / 2 + motorHeight + rotorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_SetObjectColor(obj, 0, 0, 0, 0.1);
		CAL_CreateCylinder(this->robot_model, rotorRadius, rotorHeight, -length, 0, beamHeight / 2 + motorHeight + rotorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_SetObjectColor(obj, 0, 0, 0, 0.1);
		CAL_CreateCylinder(this->robot_model, rotorRadius, rotorHeight, 0, length, beamHeight / 2 + motorHeight + rotorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_SetObjectColor(obj, 0, 0, 0, 0.1);
		CAL_CreateCylinder(this->robot_model, rotorRadius, rotorHeight, 0, -length, beamHeight / 2 + motorHeight + rotorHeight / 2, &obj);
		CAL_SetObjectOrientation(obj, (float) (M_PI*0.5), 0, 0);
		CAL_SetObjectColor(obj, 0, 0, 0, 0.1);
		CAL_CreateBox(this->robot_model, centerSide, centerSide, beamHeight, 0, 0, 0, &obj);
		CAL_SetObjectOrientation(obj, 0, 0, (float) (M_PI*0.25));
		CAL_CreateBox(this->robot_model, flagLength, beamWidth + 0.001, beamHeight + 0.001, length / 1.65, 0, 0, &obj);
		CAL_SetObjectColor(obj, 1, 0.15, 0);

		float flagTriangle[18] = {length / 1.65 - flagLength / 2, 0, -beamHeight / 2,
			length / 1.65, 0, -beamHeight / 2 - flagLength / 2,
			length / 1.65 + flagLength / 2, 0, -beamHeight / 2,
			length / 1.65 + flagLength / 2, 0, -beamHeight / 2,
			length / 1.65, 0, -beamHeight / 2 - flagLength / 2,
			length / 1.65 - flagLength / 2, 0, -beamHeight / 2};
		CAL_CreateTriangles(this->robot_model, 2, flagTriangle, &obj);
		//CAL_SetObjectColor(obj, 1, 0.15, 0);
		CAL_SetObjectColor(obj, 1,1,1);

		float polygon1[18] = {-sqrt(2.0)*centerSide/2, 0, 0,
			-sqrt(2.0)*centerSide/2+centerHeight, 0, centerHeight,
			sqrt(2.0)*centerSide/2-centerHeight, 0, centerHeight,
			sqrt(2.0)*centerSide/2, 0, 0,
			sqrt(2.0)*centerSide/2-centerHeight, 0, -centerHeight,
			-sqrt(2.0)*centerSide/2+centerHeight, 0, -centerHeight};
		CAL_CreatePolygon(this->robot_model, 6, polygon1, &obj);
		CAL_SetObjectColor(obj, 0.15, 0.15, 0.15);
		float polygon2[18] = {-sqrt(2.0)*centerSide/2, 0, 0,
			-sqrt(2.0)*centerSide/2+centerHeight, 0, -centerHeight,
			sqrt(2.0)*centerSide/2-centerHeight, 0, -centerHeight,
			sqrt(2.0)*centerSide/2, 0, 0,
			sqrt(2.0)*centerSide/2-centerHeight, 0, centerHeight,
			-sqrt(2.0)*centerSide/2+centerHeight, 0, centerHeight};
		CAL_CreatePolygon(this->robot_model, 6, polygon2, &obj);
		//CAL_SetObjectColor(obj, 0.15, 0.15, 0.15);
		CAL_SetObjectColor(obj,1,0,0);
		float polygon3[18] = {0, -sqrt(2.0)*centerSide/2, 0,
			0, -sqrt(2.0)*centerSide/2+centerHeight, centerHeight,
			0, sqrt(2.0)*centerSide/2-centerHeight, centerHeight,
			0, sqrt(2.0)*centerSide/2, 0,
			0, sqrt(2.0)*centerSide/2-centerHeight, -centerHeight,
			0, -sqrt(2.0)*centerSide/2+centerHeight, -centerHeight};
		CAL_CreatePolygon(this->robot_model, 6, polygon3, &obj);
		CAL_SetObjectColor(obj, 0.15, 0.15, 0.15);
		float polygon4[18] = {0, -sqrt(2.0)*centerSide/2, 0,
			0, -sqrt(2.0)*centerSide/2+centerHeight, -centerHeight,
			0, sqrt(2.0)*centerSide/2-centerHeight, -centerHeight,
			0, sqrt(2.0)*centerSide/2, 0,
			0, sqrt(2.0)*centerSide/2-centerHeight, centerHeight,
			0, -sqrt(2.0)*centerSide/2+centerHeight, centerHeight};
		CAL_CreatePolygon(this->robot_model, 6, polygon4, &obj);
		CAL_SetObjectColor(obj, 0.15, 0.15, 0.15);
	}

	virtual void rotate(const state& s)
	{
		Eigen::Matrix<double,3,3> rot = Eigen::Matrix<double,3,3>::Zero();
		rot(0,2) = s[4];
		rot(1,2) = -s[3];
		rot(2,0) = -s[4];
		rot(2,1) = s[3];
		Eigen::Matrix<double,3,3> R = rot.exp();
		Eigen::Quaternion<double> q(R);
		this->_rotate(q);
		//this->_rotate(Eigen::Quaternion<double>(rot.exp()));
	}
};

class Nonholonomic : public Robot
{
public:
	Nonholonomic() : Robot()
	{
		CAL_CreateBox(this->robot_group, 5, 3, 2.5, 2.5, 1.5, 0);

		CAL_CreateBox(this->robot_model, 5, 3, 2.5, 2.5, 1.5, 0);
		CAL_SetGroupColor(this->robot_model, 0.05, 0.05, 0.05);
	}

	virtual void rotate(const state& s, bool model = false)
	{
		double rot = s[2] - 0.5*M_PI;
		while (rot < 0) rot += 2*M_PI;

		int result = CAL_SetGroupOrientation(robot_group, 0, 0, rot);
		if (result != CAL_SUCCESS) {
			std::cout << "CAL_SetGroupOrientation failed (" << result << ")." << std::endl;
			_getchar();
			exit(1);
		}

		if (model) {
			result = CAL_SetGroupOrientation(robot_group, 0, 0, rot);
			if (result != CAL_SUCCESS) {
				std::cout << "CAL_SetGroupOrientation failed (" << result << ")." << std::endl;
				_getchar();
				exit(1);
			}
		}
	}
};

#endif // __ROBOTS_HPP__