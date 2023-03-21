/**
 * 本程序发布终端的位置信息,更新频率1HZ
 * author:zhuwenrun
 * date:2023/3/13
**/
#include <ros/ros.h>
#include <tf/transform_broadcaster.h>

std::string car_name;
double car_x;
double car_y;

int main(int argc, char** argv)
{
    // 初始化ROS节点
	ros::init(argc, argv, "car_tf_broadcaster");
	// 创建节点句柄
	ros::NodeHandle node;
	// 创建tf的广播器
	tf::TransformBroadcaster br;
	tf::Transform transform;
	tf::Quaternion q;

	// 输入参数小车的名字和坐标
	if (argc != 4)
	{
		ROS_ERROR("请输入参数小车的名字和坐标，按照'car_tf_broadcaster carx x y'的格式！"); 
		return -1;
	}

	car_name = argv[1];
	car_x = atof(argv[2]);
	car_y = atof(argv[3]);

	ros::Rate rate(1.0);
	while (node.ok())
	{
		// 广播world与car之间的tf数据
		transform.setOrigin( tf::Vector3(car_x, car_y, 0.0) );
		q.setRPY(0, 0, 0);
		transform.setRotation(q);
		br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "world", car_name));

		rate.sleep();
	}
	return 0;
};


