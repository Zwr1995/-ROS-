/**
 * 本程序将执行/show_person服务，服务数据类型car_test::Authentication_S
 * author:zhuwenrun
 * date:2023/3/13
**/
 
#include <ros/ros.h>
#include "car_test/Authentication_S.h"
#include "sensor_msgs/LaserScan.h"
#include <tf/transform_listener.h>

#include<iostream>
#include<fstream>
#include<string>
using namespace std;

#define RAD2DEG(x) ((x)*180./M_PI)

typedef struct
{
    float degree;
    float distance;
}LASER_INFO;

LASER_INFO L_F0[2048];
int LF_cnt;

float tf_degree;
float tf_distance;

void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int count = scan->scan_time / scan->time_increment;
    ROS_INFO("I heard a laser scan %s[%d]:", scan->header.frame_id.c_str(), count);
    ROS_INFO("angle_range, %f, %f", RAD2DEG(scan->angle_min), RAD2DEG(scan->angle_max));
  
    for(int i = 0; i < count; i++) {
        float degree = RAD2DEG(scan->angle_min + scan->angle_increment * i);
        ROS_INFO(": [%f, %f]", degree, scan->ranges[i]);
        L_F0[i].degree = degree;
        L_F0[i].distance = scan->ranges[i];
    }
    LF_cnt = count;
}

bool authenticationCallback(car_test::Authentication_S::Request  &req,
         			car_test::Authentication_S::Response &res)
{
    char buff[33] = {0};
    int Auth_flag = 0;
    // 显示请求数据
    ROS_INFO("car: name:%s  x:%f  y:%f", req.name.c_str(), req.x, req.y);

    for(int i = 0; i < LF_cnt; i++)
    {
        if((fabs(L_F0[i].degree-tf_degree)<10)&&(fabs(L_F0[i].distance-tf_distance)<0.3))
        {
            Auth_flag = 1;
            break;
        }
    }

    ifstream readFile;
	readFile.open("/home/zwr/quickstart_pytorch-0.18.0/server_para.md5", ios::in);
	if (readFile.is_open())
	{
		for(int index = 0 ; index < 32 ; index ++)
		{
			if((buff[index]=readFile.get())==EOF)
			{
				break;
			}
		}
		buff[32] = '\0';
	}
	readFile.close();

    string buff_s=buff;
    if(buff_s==req.name)
    {
        Auth_flag = 1;
    }

    ROS_INFO("%s---%s---%d",buff,req.name.c_str(),Auth_flag);

	// 设置反馈数据
    if(Auth_flag == 1)
    {
	    res.result = "Authentication OK!";
    }
    else
    {
        res.result = "Authentication failed!"; 
    }

    return true;
}

int main(int argc, char **argv)
{
    // ROS节点初始化
    ros::init(argc, argv, "authentication_server");
    // 创建节点句柄
    ros::NodeHandle n;
    // 创建一个名为/assist_auth的server，注册回调函数authenticationCallback
    ros::ServiceServer authentication_service = n.advertiseService("/assist_auth", authenticationCallback);
    ros::Subscriber sub = n.subscribe<sensor_msgs::LaserScan>("/scan", 1000, scanCallback);

    // 创建tf的监听器
	tf::TransformListener listener;
	ros::Rate rate(10.0);

    while (n.ok())
	{
		// 获取car1与car2坐标系之间的tf数据
		tf::StampedTransform transform;
		try
		{
			listener.waitForTransform("/car2", "/car1", ros::Time(0), ros::Duration(3.0));
			listener.lookupTransform("/car2", "/car1", ros::Time(0), transform);
		}
		catch (tf::TransformException &ex) 
		{
			ROS_ERROR("%s",ex.what());
			ros::Duration(1.0).sleep();
			continue;
		}

		//从car2到car1的角度
		tf_degree = RAD2DEG(atan2(transform.getOrigin().y(),transform.getOrigin().x()));
		//car2到car1的直线距离
		tf_distance = sqrt(pow(transform.getOrigin().x(),2)+pow(transform.getOrigin().y(),2));
		
		//ROS_INFO("%f---%f",tf_degree,tf_distance);

        ros::spinOnce();
		rate.sleep();
	}
    return 0;
}


