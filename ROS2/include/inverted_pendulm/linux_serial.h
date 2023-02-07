#ifndef MBOT_LINUX_SERIAL_H
#define MBOT_LINUX_SERIAL_H

#include <functional>
#include <stdexcept>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>


#include <signal.h>
#include <stdio.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <termios.h>
# include <unistd.h>
#endif
#include<boost/asio.hpp>

extern void serialInit();
extern void writeSpeed(double Left_v, double Right_v,unsigned char ctrlFlag);
extern bool readSpeed(double &Left_v,double &Right_v,double &Angle,unsigned char &ctrlFlag);
unsigned char getCrc8(unsigned char *ptr, unsigned short len);

#endif
