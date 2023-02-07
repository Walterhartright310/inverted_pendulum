#include "linux_serial.h"

#include <unistd.h>
#include "std_msgs/msg/string.hpp"
#include "rclcpp/rclcpp.hpp"
/*===================================================================
程序功能：串口通信测试程序
程序编写：公众号：小白学移动机器人
其他    ：如果对代码有任何疑问，可以私信小编，一定会回复的。
=====================================================================
------------------关注公众号，获得更多有趣的分享---------------------
===================================================================*/

using namespace std;
using namespace boost::asio;
//串口相关对象
boost::system::error_code err;
boost::asio::io_service iosev;
boost::asio::serial_port sp(iosev);

/********************************************************
            串口发送接收相关常量、变量、共用体对象
********************************************************/
const unsigned char ender[2]  = {0x0d, 0x0a};
const unsigned char header[2] = {0x55, 0xaa};

/***********发送左右轮速控制速度共用体
union sendData
{
	short d;
	unsigned char data[2];
}leftVelSet,rightVelSet;
**************************************/
//发送数据，控制电机的PWM转速
union floatData_1 
{
    short d;
    unsigned char data[2];
}controlu;


/***********接收数据（左轮速、右轮速、角度）共用体（-32767 - +32768）
*****************************/
//接收来自倒立摆的数据，分别为倒立摆的角度，角速度，小车的位置和速度
union floatData 
{
    short d;
    unsigned char data[2];
} AngularNow,AngularVelNow,PositionNow,PositionVelNow,ControlNow;

/********************************************************
函数功能：串口参数初始化
入口参数：无
出口参数：
********************************************************/
void serialInit()
{
    sp.open("/dev/ttyUSB0", err);
    if(err){
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"请检查您的串口/dev/ttyUSB0，是否已经准备好：\n 1.读写权限是否打开（默认不打开) \n 2.串口名称是否正确");
        //std::cout << "Error: " << err << std::endl;
       // std::cout << "请检查您的串口/dev/ttyUSB0，是否已经准备好：\n 1.读写权限是否打开（默认不打开) \n 2.串口名称是否正确" << std::endl;
        rclcpp::shutdown();
        return ;
    }
    sp.set_option(serial_port::baud_rate(115200));
    sp.set_option(serial_port::flow_control(serial_port::flow_control::none));
    sp.set_option(serial_port::parity(serial_port::parity::none));
    sp.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
    sp.set_option(serial_port::character_size(8));    

    iosev.run();
}

/********************************************************
函数功能：将对机器人的左右轮子控制速度，打包发送给下位机
入口参数：机器人线速度、角速度
出口参数：
********************************************************/
void writeSpeed(short u,unsigned char ctrlFlag)
{
    unsigned char buf[10] = {0};        //
    int i = 0;

    unsigned char length = 0;
     
    controlu.d=u;
    // 设置消息头
     for(i = 0; i < 2; i++)
       {
        buf[i] = header[i]; 
       //buf[0] = '+';                   //buf[0]  buf[1]
       }
    // 设置机器人左右轮速度
    length = 3;
    buf[2] = length;                      //buf[2]
    for(i = 0; i < 2; i++)
    {
        buf[i + 3]  =  controlu.data[i];   //buf[]
      //buf[i + 5] =   rightVelSet.data[i]; //buf[5] buf[6]
    }
    // 预留控制指令
    buf[5] = ctrlFlag;       //buf[7]

    // 设置校验值、消息尾
    buf[3 + length] = getCrc8(buf, 3 + length);//buf[8]
    buf[3 + length + 1] = ender[0];     //buf[9]
    buf[3 + length + 2] = ender[1];     //buf[10]
    // 通过串口下发数据
    boost::asio::write(sp, boost::asio::buffer(buf));

}

std::string string2hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

/********************************************************
函数功能：从下位机读取数据

入口参数：倒立摆杆的角度，角速度，滑块的位移，速度
出口参数：bool
********************************************************/
bool readSpeed(float &Angular,float &AngularVel,float &Position,float &PositionVel,float &Control,unsigned char &ctrlFlag)
{
    unsigned char length = 0;
    unsigned char checkSum;
    unsigned char buf[50]={0};
    bool succeedReadFlag = false;
    //=========================================================
    /*boost::array<char, 17> buf;  
    //boost::system::error_code ec;  
    
    std::size_t n = boost::asio::read(sp,  boost::asio::buffer(buf), boost::asio::transfer_all(),err);  
  
   if (err)  
    {  
     RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Read error!");
     return false;  
    }  
   else  
     {  
  // n == 17  
   if (buf[0]!= header[0] || buf[1] != header[1])   //buf[0] buf[1]
    {
       RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Received message header error!");
        return false;
    }
    // 数据长度
    length = buf[2];                                 //buf[2]
   

    // 检查信息校验值
    checkSum = getCrc8(buf, 3 + length);             //buf[12] 计算得出
    if (checkSum != buf[3 + length])                 //buf[12] 串口接收
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Received data check sum error!");
        return false;
    }    

    // 读取速度值
    for(int i = 0; i < 2; i++)
    {
        AngularNow.data[i]    = buf[i + 3];   //buf[3-6]
        AngularVelNow.data[i] = buf[i + 5];   //buf[7-10]
        PositionNow.data[i]   = buf[i + 7];   //buf[11-14]
        PositionVelNow.data[i] = buf[i + 9];   //buf[15-18]
        ControlNow.data[i]    = buf[i + 11];    //buf[15-18]
    }

    // 读取控制标志位
    ctrlFlag = buf[13];
    
    Angular = AngularNow.d;
    AngularVel = AngularVelNow.d;
    Position   = PositionNow.d;
    PositionVel   = PositionVelNow.d;
    Control = ControlNow.d;
    return true; 
   }  
}*/
   // 此段代码可以读数据的结尾，进而来进行读取数据的头部
    try
    {
        boost::asio::streambuf response;
       
        
        
        boost::asio::read_until(sp, response, "\r\n", err);   // 第一次分割数据 根据数据尾"\r\n"

       std::istream is(&response);       
        std::string str;
        
       
        copy(istream_iterator<unsigned char>(istream(&response)>>noskipws),istream_iterator<unsigned char>(),buf); 
        
        /* while(response.size() != 0)
        {
            std::getline(is, str, (char)header[0]);          // 第二次分割数据 根据数据头 这个会丢 0x55
            // std::cout <<"筛选前："<<" {"<< string2hex(str) << "} " <<std::endl; 
             std::cout << "str size = " << str.size() << std::endl;
            if(str.size() == 16) 
            {
               
                
                std::string finalStr(1, header[0]);
                finalStr = finalStr + str;
             //  std::cout << "str size = " << finalStr.size() << std::endl;
                // std::cout <<"筛选后："<<" {"<< string2hex(finalStr) << "} " <<std::endl;   
                for (size_t i = 0; i < finalStr.size(); i++)
                {
                    buf[i] = finalStr[i];
                }
                //succeedReadFlag = true;
                break;
            }
            else
            {
                continue;
            }
        }*/
    }  
    catch(boost::system::system_error &err)
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"read_until error");
    } 
    //=========================================================        

    // 检查信息头
    if (buf[0]!= header[0] || buf[1] != header[1])   //buf[0] buf[1]
    {
       RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Received message header error!");
        return false;
    }
    // 数据长度
    length = buf[2];                                 //buf[2]
   

    // 检查信息校验值
    checkSum = getCrc8(buf, 3 + length);             //buf[12] 计算得出
    if (checkSum != buf[3 + length])                 //buf[12] 串口接收
    {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"),"Received data check sum error!");
        return false;
    }    

    // 读取速度值
    for(int i = 0; i < 2; i++)
    {
        AngularNow.data[i]    = buf[i + 3];   //buf[3-6]
        AngularVelNow.data[i] = buf[i + 5];   //buf[7-10]
        PositionNow.data[i]   = buf[i + 7];   //buf[11-14]
        PositionVelNow.data[i] = buf[i + 9];   //buf[15-18]
        ControlNow.data[i]     = buf[i + 11];    //buf[15-18]
    }

    // 读取控制标志位
    ctrlFlag = buf[13];
    
    Angular = AngularNow.d;
    AngularVel = AngularVelNow.d;
    Position   = PositionNow.d;
    PositionVel   = PositionVelNow.d;
    Control = ControlNow.d;
    return true;
}
/********************************************************
函数功能：获得8位循环冗余校验值
入口参数：数组地址、长度
出口参数：校验值
******************************************************* */
 unsigned char getCrc8(unsigned char *ptr, unsigned short len)
{
    unsigned char crc;
    unsigned char i;
    crc = 0;
    while(len--)
    {
        crc ^= *ptr++;
        for(i = 0; i < 8; i++)
        {
            if(crc&0x01)
                crc=(crc>>1)^0x8C;
            else 
                crc >>= 1;
        }
    }
    return crc;
}
