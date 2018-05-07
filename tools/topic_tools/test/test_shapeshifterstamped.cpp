// -*- mode: C++ -*-
/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2018, JSK Lab
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/o2r other materials provided
 *     with the distribution.
 *   * Neither the name of the JSK Lab nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
/*
 * test_shapeshifterstamped.cpp
 * Author: Yuki Furuta <furushchev@jsk.imi.i.u-tokyo.ac.jp>
 */

#include <ros/ros.h>
#include <topic_tools/shape_shifter_stamped.h>
#include <geometry_msgs/PoseStamped.h>

#include <gtest/gtest.h>


class ShapeShifterStampedSubscriber : public testing::Test
{
public:
  bool success;
  void messageCallback(const topic_tools::ShapeShifterStamped::ConstPtr &msg)
  {
    try {
      geometry_msgs::PoseStamped::Ptr p = msg->instantiate<geometry_msgs::PoseStamped>();
      if (p->pose.orientation.w == 1.0 &&
          p->header.stamp == msg->header.stamp)
        success = true;
    }
    catch (topic_tools::ShapeShifterException& e)
    {
    }
  }
protected:
  ShapeShifterStampedSubscriber(){}
  void SetUp()
  {
    success = false;
  }

  void TearDown() {}
};

TEST_F(ShapeShifterStampedSubscriber, testShapeShifterStamped)
{
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe<topic_tools::ShapeShifterStamped>(
    "input", 1,
    &ShapeShifterStampedSubscriber::messageCallback,
    (ShapeShifterStampedSubscriber*)this);

  ros::Time t1(ros::Time::now() + ros::Duration(10.0));
  while (ros::Time::now() < t1 && !success)
  {
    ros::WallDuration(0.01).sleep();
    ros::spinOnce();
  }

  EXPECT_FALSE(topic_tools::ShapeShifter::uses_old_API_);

  if (success) SUCCEED();
  else         FAIL();
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "test_shapeshifterstamped");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
