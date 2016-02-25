#include "ros/ros.h"

/* This node runs the control algorithm for basic skid steer 
   motion. The robot first corrects its angle error then 
   corrects the distance error. */

/* Define the PID gains. */ 

static int kP = 0; 
static int kI = 0; 
static int kD = 0; 

void runPID()
{
  
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "controller"); 
  ros::NodeHandle n; 

  /*Setup timer */ 
  ros::Timer timer1 = n.createTimer(ros::Duration(0.1), runPID);
  
  /*Setup subscriber to pose message from tracking. */

  /*Setup publisher to publish control message */ 

  ros::spin(); 

  return 0; 
}
