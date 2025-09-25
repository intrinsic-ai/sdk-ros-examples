#include <fstream>
#include <memory>

#include "intrinsic/resources/proto/runtime_context.pb.h"
#include "minimal_subscriber.h"
#include "rclcpp/rclcpp.hpp"
#include "ros_config.pb.h"

intrinsic_proto::config::RuntimeContext GetRuntimeContext() {
  intrinsic_proto::config::RuntimeContext runtime_context;
  std::ifstream runtime_context_file;
  runtime_context_file.open("/etc/intrinsic/runtime_config.pb",
                            std::ios::binary);
  if (!runtime_context.ParseFromIstream(&runtime_context_file)) {
    // Return default context for running locally
    std::cerr << "Warning: using default RuntimeContext\n";
    ros::RosConfig default_ros_config;
    runtime_context.mutable_config()->PackFrom(default_ros_config);
  }
  return runtime_context;
}

int main(int argc, char* argv[]) {
  auto runtime_context = GetRuntimeContext();
  ros::RosConfig ros_config;
  if (!runtime_context.config().UnpackTo(&ros_config)) {
    std::cerr << "Unable to unpack runtime_context\n";
    return EXIT_FAILURE;
  }

  // Get ROS arguments
  std::vector<const char *> ros_argv;
  for (int i = 0; i < argc; i++) {
    ros_argv.push_back(argv[i]);
  }

  // Insert --ros-args at beginning
  ros_argv.emplace_back("--ros-args");

  // Copy all other arguments
  for (int i = 0; i < ros_config.ros_args_size(); ++i) {
    ros_argv.emplace_back(ros_config.ros_args(i).c_str());
    std::cerr << "ROS argument: " << ros_argv.back() << "\n";
  }

  rclcpp::init(ros_argv.size(), ros_argv.data());
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return EXIT_SUCCESS;
}
