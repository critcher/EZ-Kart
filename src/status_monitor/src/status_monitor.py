#!/usr/bin/env python

import rospy

from geometry_msgs.msg import Pose2D
from ez_kart_msgs.msg import Status
from ez_kart_msgs.msg import Voltmeter
from ez_kart_msgs.msg import Command
from sensor_msgs.msg import Range
from sound_play.msg import SoundRequest
from sound_play.libsoundplay import SoundClient


class monitoring:

	def __init__(self):
		self.msg = Status()
		self.soundhandle = SoundClient()
		self.msg.obstacle = False
		self.msg.following = False
		self.pub = rospy.Publisher('status', Status, queue_size=1)
		self.last_received_time = rospy.get_rostime()
		self.timer = rospy.Timer(rospy.Duration(.3), self.timerCallback)
		self.timer2 = rospy.Timer(rospy.Duration(10), self.speechCallback)
		rospy.Subscriber('/poses', Pose2D, self.lostCallback)
		rospy.Subscriber('/ultrasonic_range', Range , self.obstacleCallback)
		rospy.Subscriber('/voltage', Voltmeter, self.voltageCallback)
		rospy.Subscriber('/commands', Command, self.voiceCallback)

	def lostCallback(self, msg):
		self.last_received_time = rospy.get_rostime()
		self.msg.lost = False
		self.pub.publish(self.msg)

	def voiceCallback(self, msg):
		if (msg.command == Command.FOLLOW):
			self.msg.following = True
		if (msg.command == Command.STOP):
			self.msg.following = False
		if (msg.command == Command.SHUTDOWN):
			self.msg.following = False

		self.pub.publish(self.msg)

	def obstacleCallback(self, msg):
		if (self.msg.following or self.msg.obstacle):
			if (msg.range < .9): #tweak this
				self.msg.obstacle = True
			else:
				self.msg.obstacle = False
			self.pub.publish(self.msg)

	def voltageCallback(self, msg):
		# TODO maybe find out real low voltage?
		if (msg.voltage < 11.2):
			if (self.msg.low_voltage == False):
				self.msg.low_voltage = True
		else:
			self.msg.low_voltage = False
		self.pub.publish(self.msg)

	def timerCallback(self, event):
		if (event.current_real - self.last_received_time > rospy.Duration(.3)):
			self.msg.lost = True
		self.pub.publish(self.msg)

	def speechCallback(self, event):
		if (self.msg.lost):
			self.soundhandle.say("Robot lost!")
		if (self.msg.low_voltage):
			self.soundhandle.say("Low battery!")
		if (self.msg.obstacle):
			self.soundhandle.say("Obstacle in the way!")



if __name__ == "__main__":
	rospy.init_node("status_monitor")
	start = monitoring()
	try:
		rospy.spin()
	except KeyboardInterrupt:
		soundhandle.stopAll()
		print "Shutting down status monitor"