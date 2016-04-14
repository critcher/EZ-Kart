/* This node runs the control algorithm for basic skid steer 
motion. The robot first corrects its angle error then 
corrects the distance error. */
#include <ros/ros.h>
#include "ez_kart_msgs/motorControl.h"
#include "ez_kart_msgs/Status.h"
#include <math.h>
#include <geometry_msgs/Pose2D.h>

using namespace std; 

class Control {
  public: 
    Control();
  private:
    /*Variables of Control Class */ 
    float distkP; 
    float distkI; 
    float distkD;
    float anglekP;
    float anglekI;
    float anglekD;
    float kartX; 
    float kartY; 
    float kartTheta; 
    float angleError; 
    float angleDeriv; 
    float angleIntegral; 
    float distError; 
    float distDeriv; 
    float distIntegral;
    float maxPWM; 
    bool lost; 
    bool obstacle; 
    bool low_voltage; 
    bool following; 
    bool running; 
    ros::NodeHandle n; 
    ros::Timer timer; 
    ros::Subscriber poseSub;
    ros::Subscriber statusSub; 
    ros::Publisher controlsPub;

    /*Functions of the control class*/
    //Control();
    void runPID(const ros::TimerEvent&); 
    void poseSubCallback(const geometry_msgs::Pose2D& msg); 
    void statusSubCallback(const ez_kart_msgs::Status& msg); 
};



Control::Control() {
  distkP = 100; 
  distkI = 0; 
  distkD = 0;
  anglekP = 100; 
  anglekI = 0; 
  anglekD = 0; 
  angleError = 0; 
  angleDeriv = 0; 
  angleIntegral = 0; 
  distError = 0; 
  distDeriv = 0; 
  distIntegral = 0; 
  kartX = 0; 
  kartY = 0; 
  kartTheta = 0;
  maxPWM = 127;  

  timer = n.createTimer(ros::Duration(0.1), &Control::runPID,this); 
  poseSub = n.subscribe("poses", 1000, &Control::poseSubCallback, this);
  statusSub = n.subscribe("status", 1000, &Control::statusSubCallback, this); 
  controlsPub = n.advertise<ez_kart_msgs::motorControl>("/ezKartControl",1000); 
}

void Control::poseSubCallback(const geometry_msgs::Pose2D& msg) {
  kartX = msg.x;
  kartY = msg.y; 
  kartTheta = msg.theta; 
}

void Control::statusSubCallback( const ez_kart_msgs::Status &msg) {
  lost = msg.lost; 
  obstacle = msg.obstacle; 
  low_voltage = msg.low_voltage; 
  following = msg.following;
  running = msg.running; 
}

void Control::runPID(const ros::TimerEvent&) {
  /*Giving higher priority to angle error vs distance error.
  I.e. correct for angle offset first, then distance. 
  Assuming 0 degrees is straight forward*/

  float dist = sqrt(pow(kartX,2) + pow(kartY,2)); 
  float newDistError = 0.6 - dist;
  distDeriv = (newDistError - distError)/0.1; 
  distError = newDistError;
  distIntegral = (distError * 0.1) + distIntegral;

  float newAngleError = 0 - kartTheta;
  angleDeriv = (newAngleError - angleError)/0.1;
  angleError = newAngleError;  
  angleIntegral = (angleError * 0.1) + angleIntegral;



  //Forward = 1; Backward = 0; 
  ez_kart_msgs::motorControl ezKartControl;

  /*Angle offset.*/
  if (abs(0 - angleError) > 0.01) {
    if(angleError > 0) { /*user to the left. Want kart to turn right*/
      ezKartControl.leftMotorDir = 1; /*left forward*/
      ezKartControl.rightMotorDir = 0; /*right reverse*/
    } 
    else { /*user to the right, want kart to turn left*/
      ezKartControl.leftMotorDir = 0; /*left reverse*/
      ezKartControl.rightMotorDir = 1; /*right forward*/
    }

    float control = floor(abs((angleError * anglekP) + (angleIntegral * anglekI) + (angleDeriv * anglekD))); 
    if (control < 0) {
      ezKartControl.leftMotorPWM = 0; 
      ezKartControl.rightMotorPWM = 0; 
    }
    else if (0 < control && control < maxPWM) {
      ezKartControl.leftMotorPWM = control; 
      ezKartControl.rightMotorPWM = control;
    } else if (control > maxPWM) {
      ezKartControl.leftMotorPWM = maxPWM; 
      ezKartControl.rightMotorPWM = maxPWM;      
    }
  } 
  /*There is some distance offset but no angle offset*/
  else {
    if (distError < 0) {
      /*Kart further away from user than 2ft (0.6m) */
      ezKartControl.leftMotorDir = 0; /* left and right reverse*/
      ezKartControl.rightMotorDir = 0; 
    } else {
      /*Kart close to the user than 2ft (0.6m) */
      ezKartControl.leftMotorDir = 1; /* left and right forward*/
      ezKartControl.rightMotorDir = 1;
    }
 
    float control = floor(abs((distError * distkP) + (distIntegral * distkI) + (distDeriv * distkD))); 
   
    if (control < 0) {
      ezKartControl.leftMotorPWM = 0; 
      ezKartControl.rightMotorPWM = 0; 
    } else if (0 < control && control < maxPWM) {
      ezKartControl.leftMotorPWM = control; 
      ezKartControl.rightMotorPWM = control;
    }
    else if (control > maxPWM) {
      ezKartControl.leftMotorPWM = maxPWM; 
      ezKartControl.rightMotorPWM = maxPWM;      
    }
  }

  if (!lost && !obstacle && following) {
    controlsPub.publish(ezKartControl); 
  } else {
    ezKartControl.leftMotorPWM = 0;
    ezKartControl.rightMotorPWM = 0; 
    ezKartControl.leftMotorDir = 0;
    ezKartControl.rightMotorDir = 0; 
    controlsPub.publish(ezKartControl); 
  }
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
