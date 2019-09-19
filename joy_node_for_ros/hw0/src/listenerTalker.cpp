#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"

#include <sstream>

#define MAX_LINEAR_VELOCITY 5
#define MAX_ANGULAR_VELOCITY 10

// twist command variable
geometry_msgs::Twist TwistCmd;

/**
 * call back function to process the joystick input and processing it and publishing it to the robot
 * inputs:	 joystick data 
 * output:	 None
 */
void joyCallback(const sensor_msgs::Joy::ConstPtr& joy) 
{
	//Twist
	geometry_msgs::Vector3 linear, angular;
	float x,y;
	
	
	x = joy->axes[2];
	y = joy->axes[3];
	
	// for forward and backward moving of the robot
	if(y>0){
		linear.x = sqrt(pow(x,2)+pow(y,2))*MAX_LINEAR_VELOCITY;
	}
	else{
		linear.x = -sqrt(pow(x,2)+pow(y,2))*MAX_LINEAR_VELOCITY;
	}
	// not relevant assigning it with zeros
	linear.y = 0.0;
	linear.z = 0.0;
	angular.x = 0.0;
	angular.y = 0.0;
	
	if(x!=0){
		angular.z = atan(y/x)*MAX_ANGULAR_VELOCITY;
	}
	else{
		angular.z = 0;
	}
	
	TwistCmd.linear = linear;
	TwistCmd.angular = angular;
}


int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "listener_talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;
  
  ros::Publisher chatter_pub_Twist = n.advertise<geometry_msgs::Twist>("vrep/twistCommand", 1000);
  
  ros::Subscriber sub = n.subscribe("joy", 1000, joyCallback);

  ros::Rate loop_rate(10);

  
  int count = 0;
  while (ros::ok())
  {
    
    std_msgs::String msg;

    std::stringstream ss;
    ss << "[" << count << "] " << "TwistCmd:" << TwistCmd;
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    // publishing the data over the topic to the robot
    chatter_pub_Twist.publish(TwistCmd);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
