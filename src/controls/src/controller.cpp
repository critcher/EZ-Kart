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
    float newAngleError; 
    float angleDeriv; 
    float angleIntegral; 
    float distError; 
    float newDistError; 
    float distDeriv; 
    float distIntegral;
    int maxPWM; 
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
  distkP = 180; //250
  distkI = 0; 
  distkD = 50; //-100
  anglekP = 120; //120
  anglekI = 0; 
  anglekD = 20; //20
  angleError = 0; 
  angleDeriv = 0; 
  angleIntegral = 0; 
  distError = 0; 
  distDeriv = 0; 
  distIntegral = 0; 
  kartX = 0; 
  kartY = 0; 
  kartTheta = 0;
  maxPWM = 60;   

  timer = n.createTimer(ros::Duration(0.1), &Control::runPID,this); 
  poseSub = n.subscribe("poses", 1000, &Control::poseSubCallback, this);
  statusSub = n.subscribe("status", 1000, &Control::statusSubCallback, this); 
  controlsPub = n.advertise<ez_kart_msgs::motorControl>("/ezKartControl",1000); 
}

void Control::poseSubCallback(const geometry_msgs::Pose2D& msg) {
  kartX = msg.x;
  kartY = msg.y; 
  kartTheta = msg.theta; 
  float dist = sqrt(pow(kartX,2) + pow(kartY,2)); 
  distError = newDistError;
  newDistError = 1.0 - dist;
  angleError = newAngleError;
  newAngleError = 0 - kartTheta;
  //ROS_INFO("Distance Error %f", distError);
  //ROS_INFO("Angle Error %f", (newAngleError - angleError)/0.1);
  

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

  
  distDeriv = (newDistError - distError)/0.1; 
  distIntegral = (distError * 0.1) + distIntegral;

  
  angleDeriv = (newAngleError - angleError)/0.1;
    
  angleIntegral = (angleError * 0.1) + angleIntegral;




  //Forward = 1; Backward = 0; 
  ez_kart_msgs::motorControl ezKartControl;
  float R_controlAngle = 0;
  float L_controlAngle = 0; 
  float R_controlDist = 0;
  float L_controlDist = 0;
  float L_control = 0;
  float R_control = 0; 

  /*Angle offset.*/
  /*if (abs(0 - angleError) > 0.12) {
    ROS_INFO("*****ANGLE PID %f*****", angleError);
    if(angleError > 0) { //user to the left. Want kart to turn right
      ezKartControl.leftMotorDir = 1; //left forward
      ezKartControl.rightMotorDir = 0; //right reverse
    } 
    else { //user to the right, want kart to turn left
      ezKartControl.leftMotorDir = 0; //left reverse
      ezKartControl.rightMotorDir = 1; //right forward
    }*/


    L_controlAngle = -floor(((angleError * anglekP) + (angleIntegral * anglekI) + (angleDeriv * anglekD))); 
    R_controlAngle = -L_controlAngle; 
    /*if (abs(0- angleError) > 0.12)
    {//user to the left. Want kart to turn right
      R_controlAngle = -R_controlAngle;
      L_controlAngle = L_controlAngle; 
    }
    else
    {//user to the right, want kart to turn left
      R_controlAngle = R_controlAngle;
      L_controlAngle = -L_controlAngle;
    }
    /*if (control < 0) {
      ezKartControl.leftMotorPWM = 0; 
      ezKartControl.rightMotorPWM = 0; 
    }
    else if (0 < control && control < maxAnglePWM) {
      ezKartControl.leftMotorPWM = control; 
      ezKartControl.rightMotorPWM = control;
    } else if (control > maxAnglePWM) {
      ezKartControl.leftMotorPWM = maxAnglePWM; 
      ezKartControl.rightMotorPWM = maxAnglePWM;      
    }
  } 
  /*There is some distance offset but no angle offset*/
  /*else {
    if (distError < 0) {
      //Kart further away from user than 3.33ft (1m) 
      ROS_INFO("*****DISTANCE PID %f*****", distError);
      ezKartControl.leftMotorDir = 0; // left and right reverse
      ezKartControl.rightMotorDir = 0; 
    } else {
      //Kart close to the user than 3.33ft (1m) 
      ezKartControl.leftMotorDir = 1; //left and right forward
      ezKartControl.rightMotorDir = 1;
    }*/
 
    L_controlDist = floor(((distError * distkP) + (distIntegral * distkI) + (distDeriv * distkD))); 
    R_controlDist = L_controlDist;  
    
    if (L_controlDist >= maxPWM)
    {
      L_controlDist = maxPWM;
      R_controlDist = maxPWM;
    }
    if (L_controlDist <= -maxPWM)
    {
      L_controlDist = -maxPWM;
      R_controlDist = -maxPWM; 
    }

    /*if (distError < 0)
    {
      L_controlDist = -L_controlDist; 
      R_controlDist = -R_controlDist; 
    }
    /*if (control < 0) {
      ezKartControl.leftMotorPWM = 0; 
      ezKartControl.rightMotorPWM = 0; 
    } else if (0 < control && control < maxDistPWM) {
        ezKartControl.leftMotorPWM = control; 
        ezKartControl.rightMotorPWM = control;
      }
      else if (control > maxDistPWM) {
        ezKartControl.leftMotorPWM = maxDistPWM; 
        ezKartControl.rightMotorPWM = maxDistPWM;      
      }
    }*/

  L_control = L_controlDist + L_controlAngle;
  R_control = R_controlDist +  R_controlAngle; 

  ezKartControl.rightMotorDir = R_control >= 0; 
  ezKartControl.leftMotorDir = L_control >= 0; 

  /*if (abs(R_control) > maxPWM)
  {
    R_control = maxPWM; 
  }
  if (abs(L_control) > maxPWM)
  {
    L_control = maxPWM; 
  }*/
  ezKartControl.rightMotorPWM = abs(R_control); 
  ezKartControl.leftMotorPWM = abs(L_control);



  
  /*if (control < 0) {
      ezKartControl.leftMotorPWM = 0; 
      ezKartControl.rightMotorPWM = 0; 
    } else if (0 < control && control < maxDistPWM) {
        ezKartControl.leftMotorPWM = control; 
        ezKartControl.rightMotorPWM = control;
      }
      else if (control > maxDistPWM) {
        ezKartControl.leftMotorPWM = maxDistPWM; 
        ezKartControl.rightMotorPWM = maxDistPWM;      
      }*/

  if (!lost && !obstacle && following)
  {
    controlsPub.publish(ezKartControl); 
  } 
  else if (!lost && obstacle && following && ezKartControl.leftMotorDir == 0 && ezKartControl.rightMotorDir == 0)
  {
    controlsPub.publish(ezKartControl);
  }
  else  
  {
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
