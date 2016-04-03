#!usr/bin/env python

import rospy

from geometry_msgs.msg import PoseArray
from ez_kart_msgs.msg import Status
from ez_kart_msgs.msg import Voltmeter
from ez_kart_msgs.msg import Command
from sensor_msgs.msg import Range


class monitoring:

	def __init__(self):
		self.msg = Status()
		self.msg.obstacle = False
		self.pub = rospy.Publisher('status', Status)
		self.last_received_time = rospy.get_rostime()
		self.timer = rospy.Timer(rospy.Duration(5), timer_callback)
		rospy.Subscriber('/tag_detections_pose', PoseArray, self.lostCallback)
		rospy.Subscriber('/sensor_msgs/Range', Range , self.obstacleCallback)
		rospy.Subscriber('/voltage', Voltmeter, self.voltageCallback)
		rospy.Subscriber('/commands', Command, self.voiceCallback)

	def lostCallback(self, msg):
		self.last_received_time = time_received

	def voiceCallback(self, msg):
		if (msg.command == Command.FOLLOW):
			self.msg.following = True
		if (msg.command == Command.STOP):
			self.msg.following = False
		if (msg.command == Command.SHUTDOWN):
			self.msg.following = False

		self.pub.publish(self.msg)

	def obstacleCallback(self, msg):
		if (self.msg.following || self.msg.obstacle):
			if (msg.range < .4): #tweak this
				self.msg.obstacle = True
				self.msg.following = False
			else:
				self.msg.obstacle = False
				self.msg.following True
			self.pub.publish(self.msg)

	def voltageCallback(self, msg):
		if (msg.voltage < 11.2):
			self.msg.low_voltage = True
			self.msg.message = "Low battery"
			self.pub.publish(self.msg)

	def timer_callback(event):
		print 'Timer called at ' + str(event.current_real)
		print 'Last received time ' + str(self.last_received_time)
		if (event.current_real - self.last_received_time > 5):
			self.msg.lost = True
			self.msg.following = False
			self.pub.publish(self.msg)


if __name__ == "__main__":
	rospy.init_node("status_monitor")
	start = monitoring()
	try:
		rospy.spin()
	except KeyboardInterrupt:
		print "Shutting down status monitor"