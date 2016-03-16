#!/usr/bin/env python

import rospy

from geometry_msgs.msg import Pose2D, PoseArray
from math import atan2

VERBOSE = false

class tracking:

    def __init__(self):
        self.msg = Pose()
        self.pub = rospy.Publisher('poses', Pose)
        rospy.Subscriber('/tag_detections_pose', Pose2D, self.tagPoseCallback)

        
    def tagPoseCallback(self, msg):
        pose = msg.poses[0]

        self.msg.x = pose.position.x
        self.msg.y = pose.position.y
        self.msg.theta = 90.0 - atan2(self.msg.y, self.msg.x)

        self.pub.publish(self.msg)

if __name__ == "__main__":
    rospy.init_node('tracking')
    start = tracking()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        print "Shutting down tracking node"