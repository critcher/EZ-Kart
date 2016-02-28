/* This node runs the control algorithm for basic skid steer 
   motion. The robot first corrects its angle error then 
   corrects the distance error. */
#include <ros/ros.h>
#include "ez_kart_msgs/motorControl.h"
#include <math.h>
#include <geometry_msgs/Pose2D.h>

/*!!!!!TO DO SUBSCRIBE TO STATUS MESSAGES !!!!!!!!*/

using namespace std; 

class Control
{
public: 
  Control();
private:
  /*Variables of Control Class */ 
  float kP; 
  float kI; 
  float kD;
  float kartX; 
  float kartY; 
  float kartTheta; 
  float angleError; 
  float angleDeriv; 
  float angleIntegral; 
  float distError; 
  float distDeriv; 
  float distIntegral;
  ros::NodeHandle n; 
  ros::Timer timer; 
  ros::Subscriber poseSub; 
  ros::Publisher controlsPub;

  /*Functions of the control class*/
  //Control();
  void runPID(const ros::TimerEvent&); 
  void poseSubCallback(const geometry_msgs::Pose2D& msg); 
};



Control::Control()
{
  kP = 0; 
  kI = 0; 
  kD = 0; 
  angleError = 0; 
  angleDeriv = 0; 
  angleIntegral = 0; 
  distError = 0; 
  distDeriv = 0; 
  distIntegral = 0; 
  kartX = 0; 
  kartY = 0; 
  kartTheta = 0; 

  timer = n.createTimer(ros::Duration(0.1), &Control::runPID,this); 
  poseSub = n.subscribe("name_pose_2d", 1000, &Control::poseSubCallback, this); 
  controlsPub = n.advertise<ez_kart_msgs::motorControl>("/ezKartControl",1000); 
}

void Control::poseSubCallback(const geometry_msgs::Pose2D& msg)
{
  kartX = msg.x;
  kartY = msg.y; 
  kartTheta = msg.theta; 

}

void Control::runPID(const ros::TimerEvent&)
{
  /*Giving higher priority to angle error vs distance error.
    I.e. correct for angle offset first, then distance. 
    Assuming 0 degrees is straight forward*/

  float dist = sqrt(pow(kartX,2) + pow(kartY,2)); 
  distError = 0.6 - dist; 
  distDeriv = distError/0.1; 
  distIntegral = (distError * 0.1) + distIntegral;
  
  angleError = 0 - kartTheta; 
  angleDeriv = angleError/0.1; 
  angleIntegral = (angleError * 0.1) + angleIntegral;

  /*!!!!!!TO DO CHECK MOTOR DIRECTION WITH MOTOR CONTROLLER!!!!!!*/

  ez_kart_msgs::motorControl ezKartControl;
  if (abs(0 - angleError) > 0.01)
    /*Angle offset.*/
    {
      if(angleError > 0) /*kart turned to left*/
	{
	  ezKartControl.leftMotorDir = 1; /*left forward*/
	  ezKartControl.rightMotorDir = 0; /*right reverse*/
	}
      else
	/*Kart turned to right*/
	{
	  ezKartControl.leftMotorDir = 0; /*left reverse*/
	  ezKartControl.rightMotorDir = 1; /*right forward*/
	}
      float control = floor(abs((angleError * kP) + (angleIntegral * kI) + (angleDeriv * kD))); 
      ezKartControl.leftMotorPWM = control; 
      ezKartControl.rightMotorPWM = control; 
      
    }
  else 
    /*There is some distance offset but no angle offset*/
    {
      if (distError > 0.6) 
	/*Kart further away from user than 2ft (0.6m) */
	{
	  ezKartControl.leftMotorDir = 0; /* left and right reverse*/
	  ezKartControl.rightMotorDir = 0; 
	}
      else
	/*Kart close to the user than 2ft (0.6m) */
	{
	  ezKartControl.leftMotorDir = 1; /* left and right forward*/
	  ezKartControl.rightMotorDir = 1;
	}
      float control = floor(abs((distError * kP) + (distIntegral * kI) + (distDeriv * kD))); 
      ezKartControl.leftMotorPWM = control; 
      ezKartControl.rightMotorPWM = control;
      
    }
  controlsPub.publish(ezKartControl); 
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "controller"); 
  Control ezKartController;
  while(ros::ok())
  { 		    
    ros::spinOnce();
  }
  return 0; 
}
