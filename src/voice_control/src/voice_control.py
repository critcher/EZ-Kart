#!/usr/bin/env python

import rospy

from std_msgs.msg import String
from ez_kart_msgs.msg import Command

commandMap = {"easy cart follow":Command.FOLLOW,
              "easy cart stop":Command.STOP,
              "easy cart shutdown":Command.SHUTDOWN}

class voice_control:

    def __init__(self):
        self.msg = Command()
        self.pub_ = rospy.Publisher('commands', Command, queue_size=1)
        rospy.Subscriber('recognizer/output', String, self.speechCallback)

        r = rospy.Rate(10.0)
        while not rospy.is_shutdown():
            r.sleep()
        
    def speechCallback(self, msg):
        for s in commandMap:
            if s in msg.data:
                self.msg.command = commandMap[s]
                self.pub_.publish(self.msg)
                break

if __name__ == "__main__":
    rospy.init_node('voice_control')
    start = voice_control()
