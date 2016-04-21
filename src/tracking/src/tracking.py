#!/usr/bin/env python

import rospy

from geometry_msgs.msg import Pose2D, PoseArray
from std_msgs.msg import Int16
from math import atan2, floor
from tf.transformations import euler_from_quaternion

VERBOSE = False

class tracking:

    def __init__(self):
        self.msg = Pose2D()
        self.pub = rospy.Publisher('poses', Pose2D, queue_size=1)
        self.pub_servo = rospy.Publisher('servo', Int16, queue_size=1)
        rospy.Subscriber('/tag_detections_pose', PoseArray, self.tagPoseCallback)
        self.timer = rospy.Timer(rospy.Duration(0.1), self.pidCallback)

        self.anglekP = 0
        self.anglekI = 0
        self.anglekD = 0
        self.angleError = 0
        self.angleDeriv = 0
        self.angleIntegral = 0 

        
    def tagPoseCallback(self, msg):
        if msg.poses:
            pose = msg.poses[0]

            self.msg.x = pose.position.x
            self.msg.y = pose.position.z
            self.msg.theta = atan2(pose.position.x, pose.position.z)

            self.pub.publish(self.msg)

    def pidCallback(self, event):
        newAngleError = 0 - self.msg.theta;
        self.angleDeriv = (newAngleError - self.angleError)/0.1;
        self.angleError = newAngleError;  
        self.angleIntegral = (self.angleError * 0.1) + self.angleIntegral;

        controlAngle = (-1) * floor(((self.angleError * self.anglekP) + (self.angleIntegral * self.anglekI) + (self.angleDeriv * self.anglekD))); 
        #self.pub_servo.publish(90);
        #self.pub_servo.publish(controlAngle)

if __name__ == "__main__":
    rospy.init_node('tracking')
    start = tracking()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        print "Shutting down tracking node"