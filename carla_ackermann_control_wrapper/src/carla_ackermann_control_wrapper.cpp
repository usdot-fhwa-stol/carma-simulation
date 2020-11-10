/*
 * Copyright (C) 2019-2020 LEIDOS.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "carla_ackermann_control_wrapper.h"

namespace carla_ackermann_control_wrapper
{
CarlaAckermannControlWrapper::CarlaAckermannControlWrapper(){}

void CarlaAckermannControlWrapper::init()
{
    ROS_INFO("Initalizing carla ackermann control wrapper node...");

    // Load parameters from config/parametrs.yaml
    max_speed_ = pnh_.param<double>("max_speed", 50);
    vehicle_mass_ = pnh_.param<double>("vehicle_weight", 1500);
    // Load parameters from carma-config/VehicleConfigParams.yaml
    pnh_.getParam("vehicle_wheel_base", wheel_base_);
    pnh_.getParam("vehicle_steer_lim_deg", max_steer_angle_);
    pnh_.getParam("vehicle_acceleration_limit", max_accel_);
    pnh_.getParam("vehicle_deceleration_limit", max_decel_);
    
    // Set driver type
    driver_status_.controller = true;
    driver_status_.name = "/hardware_interface/carla_ackermann_control_wrapper";

    robotic_status_.robot_active = false;
    robotic_status_.robot_enabled = false;
}

// Publish ego vehicle info
void CarlaAckermannControlWrapper::publish_ego_veh_info()
{
    ego_info_.rolename = "ego_vehicle";
    for (auto wheel : ego_info_.wheels){
        wheel.max_steer_angle = max_steer_angle_;
        ego_info_.wheels.push_back(wheel);
    }
    ego_info_.mass = vehicle_mass_;
    vehicle_info_pub_.publish(ego_info_);
}

// Publish controller driver status
void CarlaAckermannControlWrapper::update_controller_health_status()
{
    driver_status_.status = cav_msgs::DriverStatus::OPERATIONAL;
    driver_status_pub_.publish(driver_status_);
}

void CarlaAckermannControlWrapper::update_robot_status()
{
    robot_status_pub_.publish(robotic_status_);
}

// Publish robotic status
void CarlaAckermannControlWrapper::carla_enabled_cb(const cav_msgs::CarlaEnabledConstPtr& msg)
{
    robotic_status_.robot_enabled = msg->carla_enabled;
    update_robot_status();
    publish_ego_veh_info();
}

void CarlaAckermannControlWrapper::guidance_state_cb(const cav_msgs::GuidanceStateConstPtr& msg)
{
    robotic_status_.robot_active = (msg->state == cav_msgs::GuidanceState::ACTIVE || msg->state == cav_msgs::GuidanceState::ENGAGED);
    update_robot_status();
}

// Publish AckermannDrive
void CarlaAckermannControlWrapper::vehicle_cmd_cb(const autoware_msgs::VehicleCmd::ConstPtr& vehicle_cmd)
{
    vehicle_cmd_ = *vehicle_cmd;
    ackermann_drive_ = worker_.update_ackermanndrive_cmd(vehicle_cmd_, wheel_base_);
    ackermanndrive_pub_.publish(ackermann_drive_);
}

bool CarlaAckermannControlWrapper::spin_cb()
{
    update_controller_health_status();
    update_robot_status();
    return true;
}

int CarlaAckermannControlWrapper::run()
{
    init();

    // Initialize all publishers
    ackermanndrive_pub_ = nh_.advertise<ackermann_msgs::AckermannDrive>("/carla/ego_vehicle/ackermann_cmd", 1);
    robot_status_pub_ = nh_.advertise<cav_msgs::RobotEnabled>("controller/robot_status", 1);
    vehicle_info_pub_ = nh_.advertise<carla_msgs::CarlaEgoVehicleInfo>("/carla/ego_vehicle/vehicle_info", 1);
    driver_status_pub_ = nh_.advertise<cav_msgs::DriverStatus>("driver_discovery",1);

    // Initialize all subscribers
    vehicle_cmd_sub_ = nh_.subscribe("vehicle_cmd", 1, &CarlaAckermannControlWrapper::vehicle_cmd_cb, this);
    carla_enabled_sub_ = nh_.subscribe("carla_enabled", 1, &CarlaAckermannControlWrapper::carla_enabled_cb, this);
    guidance_state_sub_ = nh_.subscribe("/guidance/state", 1, &CarlaAckermannControlWrapper::guidance_state_cb, this);

    nh_.setSpinCallback(std::bind(&CarlaAckermannControlWrapper::spin_cb, this));
    nh_.setSpinRate(10); //Spin rate in Hz. Normally we use 10, 20 or 50 depending on the application.
    nh_.spin();

    return 0;
}
}