/**
 * 本程序监听终端的位置信息
 * author:zhuwenrun
 * date:2023/3/13
**/

#include <ros/ros.h>
#include <tf/transform_listener.h>

#define RAD2DEG(x) ((x)*180./M_PI)

int main(int argc, char** argv)
{
	// 初始化ROS节点
	ros::init(argc, argv, "car_tf_listener");
    // 创建节点句柄
	ros::NodeHandle node;
	// 创建tf的监听器
	tf::TransformListener listener;
	ros::Rate rate(1.0);
	while (node.ok())
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
		float theta = RAD2DEG(atan2(transform.getOrigin().y(),transform.getOrigin().x()));
		//car2到car1的直线距离
		float distance = sqrt(pow(transform.getOrigin().x(),2)+pow(transform.getOrigin().y(),2));
		
		ROS_INFO("%f---%f",theta,distance);

		rate.sleep();
	}
	return 0;
};
