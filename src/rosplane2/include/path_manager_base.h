/**
 * @file path_manager_base.h
 *
 * Base class definition for autopilot path follower in chapter 10 of UAVbook, see http://uavbook.byu.edu/doku.php
 *
 * @author Gary Ellingson <gary.ellingson@byu.edu>
 * adapted by Judd Mehr and Brian Russel for RosPlane software
 */

#ifndef PATH_MANAGER_BASE_H
#define PATH_MANAGER_BASE_H

#include <rclcpp/rclcpp.hpp> 
#include <rosplane_msgs/msg/State.hpp>
#include <rosplane_msgs/msg/Current_Path.hpp>
#include <rosplane_msgs/msg/Waypoint.hpp>
#include <sensor_msgs/Imu.hpp>
#include <std_msgs/msg/Float32.hpp>
#include <std_msgs/msg/Float32MultiArray.hpp>
#include <sensor_msgs/msg/FluidPressure.hpp>
#include <math.hpp>
#include <Eigen/Eigen>
#include <rosplane/ControllerConfig.hpp>

namespace rosplane
{
class path_manager_base
{
public:
  path_manager_base();

protected:

  struct waypoint_s
  {
    float w[3];
    float chi_d;
    bool  chi_valid;
    float Va_d;
  };

  std::vector<waypoint_s> waypoints_;
  int num_waypoints_;
  int idx_a_;                 /** index to the waypoint that was most recently achieved */

  struct input_s
  {
    float pn;               /** position north */
    float pe;               /** position east */
    float h;                /** altitude */
    float chi;              /** course angle */
  };

  struct output_s
  {
    bool  flag;             /** Inicates strait line or orbital path (true is line, false is orbit) */
    float Va_d;             /** Desired airspeed (m/s) */
    float r[3];             /** Vector to origin of straight line path (m) */
    float q[3];             /** Unit vector, desired direction of travel for line path */
    float c[3];             /** Center of orbital path (m) */
    float rho;              /** Radius of orbital path (m) */
    int8_t lambda;          /** Direction of orbital path (cw is 1, ccw is -1) */
  };

  struct params_s
  {
    double R_min;
  };

  virtual void manage(const struct params_s &params, const struct input_s &input, struct output_s &output) = 0;

private:

  rclcpp::NodeHandle nh_;
  rclcpp::NodeHandle nh_private_;
  rclcpp::Subscriber vehicle_state_sub_;     /**< vehicle state subscription */
  rclcpp::Subscriber new_waypoint_sub_;      /**< new waypoint subscription */
  rclcpp::Publisher  current_path_pub_;      /**< controller commands publication */

  struct params_s params_;

  rosplane_msgs::State vehicle_state_;     /**< vehicle state */

  double update_rate_;
  rclcpp::Timer update_timer_;

  void vehicle_state_callback(const rosplane_msgs::StateConstPtr &msg);
  bool state_init_;
  void new_waypoint_callback(const rosplane_msgs::Waypoint &msg);
  void current_path_publish(const rclcpp::TimerEvent &);
};
} //end namespace
#endif // PATH_MANAGER_BASE_H
