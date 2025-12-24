cmake_minimum_required(VERSION 3.10)
project(talker_pkg)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(intrinsic_sdk_cmake REQUIRED)

# 1. Intrinsic structure generation
intrinsic_sdk_generate_service_manifest(
  SERVICE_NAME talker_pkg
  MANIFEST talker_pkg.manifest.textproto
  DEFAULT_CONFIGURATION talker_pkg_default_config.pbtxt
  PARAMETER_DESCRIPTOR ros_config.proto
  PROTOS_TARGET talker_pkg_protos
)

add_custom_target(generate_manifests ALL
  DEPENDS talker_pkg_manifest talker_pkg_default_config
)
# ------------------------

# 2. Install Python
ament_python_install_package(${PROJECT_NAME})

# 3. Install the executable node
install(PROGRAMS
  scripts/minimal_publisher
  DESTINATION lib/${PROJECT_NAME}
)

# 4. Install the binaries
install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/talker_pkg_protos.desc
  DESTINATION "share/${PROJECT_NAME}/talker_pkg"
)

# Install the service manifest
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/service_manifest.binarypb
        DESTINATION "share/${PROJECT_NAME}/talker_pkg"
)
# Install the default configuration
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/default_config.binarypb
        DESTINATION "share/${PROJECT_NAME}/talker_pkg"
)

# Generate the ros_config_pb2.py from .proto
find_program(PROTOC_EXECUTABLE protoc)
execute_process(
  COMMAND ${PROTOC_EXECUTABLE} 
          --python_out=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME} 
          --proto_path=${CMAKE_CURRENT_SOURCE_DIR} 
          ${CMAKE_CURRENT_SOURCE_DIR}/ros_config.proto
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the ros_config_pb2.py generated
install(FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/ros_config_pb2.py
  DESTINATION lib/python3.12/site-packages/${PROJECT_NAME}
)

ament_package()
