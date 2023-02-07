#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/string.hpp" //use data struct of std_msgs/String  
#include "linux_serial.h"
/*===================================================================
程序功能：串口通信测试程序

=====================================================================*/

//test send value
float testSend1=0;
//double testSend2=2222.0;
unsigned char testSend3=0x07;

//test receive value
float testRece1=0;
float testRece2=0;
float testRece3=0;
float testRece4=0;
float testRece5=0;
unsigned char testRece6=0x00;

bool ReceFlag;

float Balance_KP=60,Balance_KD=10,Position_KP=(60+20)/100,Position_KD=(300+200)/100;


int main(int agrc, char  *argv[])
{
    rclcpp::init(agrc, argv);
 //   ros::NodeHandle nh;

    //串口初始化
    serialInit();

   // ros::Rate loop_rate(20);
   rclcpp::Rate loop_rate(150);
    while(rclcpp::ok())
    {
         //testSend1++;
         
        // RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"%f\n",testSend1);
       //从STM32接收数据，输入参数依次转化为~~
	 if (readSpeed(testRece1,testRece2,testRece3,testRece4,testRece5,testRece6))
	 
	 {
	   RCLCPP_INFO(rclcpp::get_logger("rclcpp"),"%f,%f,%f,%f,%f,%d\n",testRece1,testRece2,testRece3,testRece4,testRece5,testRece6);//打印数据
	   writeSpeed(testSend1,testSend3); 
           loop_rate.sleep();
           testSend1=-(Balance_KP*testRece1+Balance_KD*testRece2+Position_KP*testRece3+Position_KD*testRece4);
	 }
	
	 else
	 {
	 RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Cannot get four number!");
	 }
         
          
         
          
	  
	  
        //向STM32端发送数据，前两个为double类型，最后一个为unsigned char类型
	  
    }
    return 0;
}
