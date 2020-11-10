#pragma once

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

#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64.h>
#include <carma_utils/CARMAUtils.h>
#include <cav_msgs/DriverStatus.h>
#include <cav_msgs/RobotEnabled.h>
#include <cav_msgs/CarlaEnabled.h>
#include <cav_msgs/GuidanceState.h>
#include <ackermann_msgs/AckermannDrive.h>
#include <carla_msgs/CarlaEgoVehicleControl.h>
#include <carla_msgs/CarlaEgoVehicleInfo.h>
#include <carla_msgs/CarlaEgoVehicleStatus.h>
#include <carla_msgs/CarlaEgoVehicleInfoWheel.h>
#include <cav_srvs/SetEnableRobotic.h>
#include <autoware_msgs/VehicleCmd.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include "carla_ackermann_control_wrapper_worker.h"
#include <ros/console.h>

namespace carla_ackermann_control_wrapper {

class CarlaAckermannControlWrapper
{
    public:

        CarlaAckermannControlWrapper();

        /*!
         * \brief Begin the execution of Carla Ackermann COntrol Wrapper node.
         * 
         * \return The exit status of this program
         */
        int run();

         // node handles
        ros::CARMANodeHandle nh_, pnh_;

        // topic subscribers
        ros::Subscriber vehicle_cmd_sub_;
        ros::Subscriber carla_enabled_sub_;
        ros::Subscriber guidance_state_sub_;

        // topic publishers
        ros::Publisher ackermanndrive_pub_;
        ros::Publisher robot_status_pub_;
        ros::Publisher vehicle_info_pub_;
        ros::Publisher driver_status_pub_;

    private:
       
        // delegate logic implementation to worker class
        CarlaAckermannControlWrapperWorker worker_;

        // initialize method
        void init();

        // message/service callbacks
        void vehicle_cmd_cb(const autoware_msgs::VehicleCmd::ConstPtr& vehicle_cmd);
        void carla_enabled_cb(const cav_msgs::CarlaEnabledConstPtr& msg);
        void guidance_state_cb(const cav_msgs::GuidanceStateConstPtr& msg);

        // check controller health status
        void update_controller_health_status();
        
        // update the robot_status
        void update_robot_status();
        
        // publish ego vehicle info
        void publish_ego_veh_info();

        bool spin_cb();

        // local variables
        autoware_msgs::VehicleCmd vehicle_cmd_;
        ackermann_msgs::AckermannDrive ackermann_drive_;
        cav_msgs::CarlaEnabled carla_status_;
        cav_msgs::RobotEnabled robotic_status_;
        cav_msgs::DriverStatus driver_status_;
        carla_msgs::CarlaEgoVehicleInfo ego_info_;

        double wheel_base_;
        double max_steer_angle_;
        double max_speed_;
        double vehicle_mass_;
        double max_accel_;
        double max_decel_;
};
}