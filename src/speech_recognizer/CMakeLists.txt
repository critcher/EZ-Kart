cmake_minimum_required(VERSION 2.8.3)
project(speech_recognizer)

find_package(catkin REQUIRED)
catkin_package(DEPENDS)

install(PROGRAMS nodes/recognizer.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
