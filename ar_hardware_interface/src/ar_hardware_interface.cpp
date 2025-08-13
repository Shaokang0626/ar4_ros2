#include <ar_hardware_interface/ar_hardware_interface.hpp>
#include <sstream>

namespace ar_hardware_interface {

hardware_interface::CallbackReturn ARHardwareInterface::on_init(
    const hardware_interface::HardwareInfo& info) {
  RCLCPP_INFO(logger_, "Initializing hardware interface...");

  if (hardware_interface::SystemInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS) {
    return hardware_interface::CallbackReturn::ERROR;
  }

  info_ = info;
  
  init_variables();

  // init motor driver
  serialPort.setPort("/dev/ttyUSB0");
  serialPort.setBaudrate(115200);
  serial::Timeout _time = serial::Timeout::simpleTimeout(200);
  serialPort.setTimeout(_time);

  std::string ar_model = info_.hardware_parameters.at("ar_model");
  // std::string velocity_control_p =
  //     info_.hardware_parameters.at("velocity_control_enabled");
  // bool velocity_control_enabled =
  //     velocity_control_p == "True" || velocity_control_p == "true";
 

  // calibrate joints if needed


  return hardware_interface::CallbackReturn::SUCCESS;
}

void ARHardwareInterface::init_variables() {
  // resize vectors
  int num_joints = info_.joints.size();
  actuator_pos_commands_.resize(num_joints);
  actuator_vel_commands_.resize(num_joints);
  actuator_positions_.resize(num_joints);
  actuator_velocities_.resize(num_joints);
  joint_positions_.resize(num_joints);
  joint_velocities_.resize(num_joints);
  joint_efforts_.resize(num_joints);
  joint_position_commands_.resize(num_joints);
  joint_velocity_commands_.resize(num_joints);
  joint_effort_commands_.resize(num_joints);
  joint_offsets_.resize(num_joints);
  for (int i = 0; i < num_joints; ++i) {
    joint_offsets_[i] =
        std::stod(info_.joints[i].parameters["position_offset"]);
  }
}

hardware_interface::CallbackReturn ARHardwareInterface::on_activate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
  RCLCPP_INFO(logger_, "Activating hardware interface...");
  if(serialPort.isOpen())
  {
    RCLCPP_INFO(logger_, "serial port is open");
  }
  else 
  {
    RCLCPP_ERROR(logger_, "serial port error");
  }


  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ARHardwareInterface::on_deactivate(
    const rclcpp_lifecycle::State& /*previous_state*/) {
  RCLCPP_INFO(logger_, "Deactivating hardware interface...");
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface>
ARHardwareInterface::export_state_interfaces() {
  std::vector<hardware_interface::StateInterface> state_interfaces;
  
  for (size_t i = 0; i < info_.joints.size(); ++i) {
    state_interfaces.emplace_back(info_.joints[i].name, "position",
                                  &joint_positions_[i]);
    state_interfaces.emplace_back(info_.joints[i].name, "velocity",
                                  &joint_velocities_[i]);
  }
  return state_interfaces;
}

std::vector<hardware_interface::CommandInterface>
ARHardwareInterface::export_command_interfaces() {
  std::vector<hardware_interface::CommandInterface> command_interfaces;
  for (size_t i = 0; i < info_.joints.size(); ++i) {
    command_interfaces.emplace_back(info_.joints[i].name, "position",
                                    &joint_position_commands_[i]);
    command_interfaces.emplace_back(info_.joints[i].name, "velocity",
                                    &joint_velocity_commands_[i]);
  }
  return command_interfaces;
}

hardware_interface::return_type ARHardwareInterface::read(
    const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {

  for (auto i = 0ul; i < joint_position_commands_.size(); i++){
    joint_positions_[i] = joint_position_commands_[i];
  }

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type ARHardwareInterface::write(
    const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
  
  serialPort.
  std::string logInfo = "Joint Cmd: ";
  std::vector<uint8_t> data;
  for (size_t i=0;i < info_.joints.size();++i){
    
    //std::fixed:设置输出格式为定点表示（即不使用科学计数法）;std::setprecision(2) 设置输出保留两位小数
    
    if(joint_positions_[i] != joint_position_commands_[i] ){
    std::stringstream jointPositionStm;
    jointPositionStm << std::fixed << std::setprecision(2)
                     << radToDeg(joint_position_commands_[i]);
    logInfo += info_.joints[i].name + ": " + jointPositionStm.str() + " | ";
    data.push_back(radToDeg(joint_position_commands_[i]));
    RCLCPP_INFO(logger_, logInfo.c_str());
    
    }
  
  }   
  

  return hardware_interface::return_type::OK;
}

}  // namespace ar_hardware_interface

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(ar_hardware_interface::ARHardwareInterface,
                       hardware_interface::SystemInterface)
