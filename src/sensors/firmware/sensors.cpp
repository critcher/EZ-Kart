#include <ros.h>
#include <ez_kart_msgs/Voltmeter.h>
#include <ez_kart_msgs/Status.h>
#include <sensor_msgs/Range.h>

#include <Arduino.h>

#define VOLTMETER_PIN A0
#define ULTRASONIC_PIN A2

#define LOST_PIN 2
#define OBSTACLE_PIN 3
#define BATTERY_PIN 4

#define VOLTAGE_SCALE .07666
#define DISTANCE_SCALE .005

ros::NodeHandle nh;

void updateLEDs(bool lost, bool obstacle, bool battery) {
  digitalWrite(LOST_PIN, lost);
  digitalWrite(OBSTACLE_PIN, obstacle);
  digitalWrite(BATTERY_PIN, battery);
}

void statusCb(const ez_kart_msgs::Status& msg) {
  updateLEDs(msg.lost, msg.obstacle,msg.low_voltage);
}

ros::Subscriber<ez_kart_msgs::Status> sub("status", &statusCb );

ez_kart_msgs::Voltmeter voltage_msg;
ros::Publisher voltage_pub("voltage", &voltage_msg);

sensor_msgs::Range distance_msg;
ros::Publisher distance_pub("ultrasonic_range", &distance_msg);

float getVoltage() {
	return analogRead(VOLTMETER_PIN) * VOLTAGE_SCALE;
}

float getDistance() {
	return analogRead(ULTRASONIC_PIN) * DISTANCE_SCALE;
}

void setup()
{
  pinMode(LOST_PIN, OUTPUT);
  pinMode(BATTERY_PIN, OUTPUT);
  pinMode(OBSTACLE_PIN, OUTPUT);
  updateLEDs(false, false, false);
  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(voltage_pub);
  nh.advertise(distance_pub);
}

void loop()
{
  voltage_msg.voltage = getVoltage();
  distance_msg.range = getDistance();
  voltage_pub.publish(&voltage_msg);
  distance_pub.publish(&distance_msg);
  nh.spinOnce();
  delay(1000);
}