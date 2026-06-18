add_library(
  minimal_publisher
  minimal_publisher.cc
)

target_link_libraries(minimal_publisher
  rclcpp::rclcpp
  ${std_msgs_TARGETS}
)

# Generate the skill's protobuf files, protobuf library, and descriptor set file.
intrinsic_sdk_protobuf_generate(
  NAME talker_skill
  SOURCES talker_skill.proto
  TARGET talker_skill_protos
  DESCRIPTOR_SET_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/talker_skill.desc"
)

# Generate the skill config file.
intrinsic_sdk_generate_skill_config(
  TARGET talker_skill_skill_config
  SKILL_NAME talker_skill
  MANIFEST talker_skill.manifest.textproto
  PROTO_DESCRIPTOR_FILE "${CMAKE_CURRENT_BINARY_DIR}/talker_skill.desc"
  SKILL_CONFIG_FILE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/talker_skill_config.pbbin"
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/talker_skill_config.pbbin"
    talker_skill.manifest.textproto
  DESTINATION share/${PROJECT_NAME}
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/talker_skill.desc"
  DESTINATION share/${PROJECT_NAME}
  RENAME talker_skill_protos.desc
)

# Generate the c++ main file for the skill.
intrinsic_sdk_generate_skill_main_cc(
  MANIFEST talker_skill.manifest.textproto
  HEADER_FILES talker_skill.h
  MAIN_FILE_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/talker_skill_main.cc"
)

# Build the skill main.
add_executable(talker_skill_main
  # Generated main file
  "${CMAKE_CURRENT_BINARY_DIR}/talker_skill_main.cc"
  # User created skill class
  talker_skill.cc
)

target_include_directories(talker_skill_main
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(talker_skill_main
  # SDK library
  intrinsic_sdk_cmake::intrinsic_sdk_cmake
  # Target that generates skill's protobuf files and library
  talker_skill_protos
  minimal_publisher
)

install(TARGETS talker_skill_main DESTINATION lib/${PROJECT_NAME} OPTIONAL)
add_dependencies(talker_skill_main talker_skill_skill_config)

# Create the skill container image:
#
# ros2 run intrinsic_sdk_bundle_library_py intrinsic_sdk_build container \
#   --skill_name talker_skill \
#   --skill_package sdk_examples_ros_skills \
#   --skill_type cpp

# To build the bundle, source install/setup.bash and then run:
#
# ros2 run intrinsic_sdk_bundle_library_py intrinsic_sdk_build bundle \
#   --skill_name talker_skill \
#   --skill_package sdk_examples_ros_skills \
#   --manifest_path `ros2 pkg prefix --share sdk_examples_ros_skills`/talker_skill.manifest.textproto
