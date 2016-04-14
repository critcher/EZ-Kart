#include <ros.h>
#include <ez_kart_msgs/Voltmeter.h>
#include <ez_kart_msgs/Status.h>
#include <ez_kart_msgs/motorControl.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Int16.h>

#include <Arduino.h>
#include <Servo.h>

#define VOLTMETER_PIN A0
#define ULTRASONIC_PIN A2

#define LOST_PIN 2
#define OBSTACLE_PIN 7
#define BATTERY_PIN 4
#define SERVO_PIN 5
#define LEFT_MOTOR_PWM 6
#define RIGHT_MOTOR_PWM 3
#define LEFT_MOTOR_DIR 11
#define RIGHT_MOTOR_DIR 12

#define VOLTAGE_SCALE .07666
#define DISTANCE_SCALE .005

ros::NodeHandle nh;
Servo servo;

void updateLEDs(bool lost, bool obstacle, bool battery) {
  digitalWrite(LOST_PIN, lost);
  digitalWrite(OBSTACLE_PIN, obstacle);
  digitalWrite(BATTERY_PIN, battery);
}

void statusCb(const ez_kart_msgs::Status& msg) {
  updateLEDs(msg.lost, msg.obstacle,msg.low_voltage);
}

void servoCb(const std_msgs::Int16& msg) {
  servo.write(msg.data);
}

void motorCb(const ez_kart_msgs::motorControl& msg) {
  digitalWrite(LEFT_MOTOR_DIR, msg.leftMotorDir);
  digitalWrite(RIGHT_MOTOR_DIR, msg.rightMotorDir);
  analogWrite(RIGHT_MOTOR_PWM, msg.rightMotorPWM);
  analogWrite(LEFT_MOTOR_PWM, msg.leftMotorPWM);
  analogWrite(OBSTACLE_PIN, msg.rightMotorPWM);
}

ros::Subscriber<ez_kart_msgs::Status> statusSub("status", &statusCb );
ros::Subscriber<std_msgs::Int16> servoSub("servo", &servoCb );
ros::Subscriber<ez_kart_msgs::motorControl> motorSub("ezKartControl", &motorCb );

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

void motorSetup() {
  pinMode(LEFT_MOTOR_PWM, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  digitalWrite(LEFT_MOTOR_DIR, 0);
  digitalWrite(RIGHT_MOTOR_DIR, 0);
  analogWrite(RIGHT_MOTOR_PWM, 0);
  analogWrite(LEFT_MOTOR_PWM, 0);
}

void setup()
{
  pinMode(LOST_PIN, OUTPUT);
  pinMode(BATTERY_PIN, OUTPUT);
  pinMode(OBSTACLE_PIN, OUTPUT);
  motorSetup();
  servo.attach(SERVO_PIN);
  updateLEDs(false, false, false);
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(motorSub);
  nh.subscribe(statusSub);
  nh.subscribe(servoSub);
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
  delay(20);
}