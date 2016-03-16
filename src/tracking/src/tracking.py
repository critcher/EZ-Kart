#!/usr/bin/env python

import rospy

from geometry_msgs.msg import Pose2D, PoseArray
from math import atan2
from tf.transformations import euler_from_quaternion

VERBOSE = False

class tracking:

    def __init__(self):
        self.msg = Pose2D()
        self.pub = rospy.Publisher('poses', Pose2D)
        rospy.Subscriber('/tag_detections_pose', PoseArray, self.tagPoseCallback)

        
    def tagPoseCallback(self, msg):
        if msg.poses:
            pose = msg.poses[0]

            self.msg.x = pose.position.x
            self.msg.y = pose.position.z
            self.msg.theta = atan2(pose.position.x, pose.position.z)

            self.pub.publish(self.msg)

if __name__ == "__main__":
    rospy.init_node('tracking')
    start = tracking()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        print "Shutting down tracking node"