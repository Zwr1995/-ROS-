/**
 * 该例程将请求/assist_auth服务，服务数据类型car_test::Authentication_S
**/

#include <ros/ros.h>
#include "car_test/Authentication_S.h"

#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int main(int argc, char** argv)
{
	char buff[33] = {0};
    // 初始化ROS节点
	ros::init(argc, argv, "end_client");

    // 创建节点句柄
	ros::NodeHandle node;

    // 发现/assist_auth服务后，创建一个服务客户端，连接名为/assist_auth的service
	ros::service::waitForService("/assist_auth");
	ros::ServiceClient end_client = node.serviceClient<car_test::Authentication_S>("/assist_auth");
	
	// 输入参数小车的名字和坐标
	if (argc != 4)
	{
		ROS_ERROR("请输入参数小车的名字和坐标，按照'end_client carx x y'的格式！"); 
		return -1;
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

    // 初始化car_test::Authentication_S的请求数据
	car_test::Authentication_S srv;
	srv.request.name = buff;//argv[1];
	srv.request.x  = atof(argv[2]);
	srv.request.y  = atof(argv[3]);

	// 请求服务调用
	ROS_INFO("Call service to authentication[name:%s, x:%f, y:%f]", srv.request.name.c_str(), srv.request.x, srv.request.y);
	end_client.call(srv);
	// 显示服务调用结果
	ROS_INFO("Authentication result : %s", srv.response.result.c_str());

	return 0;
};


