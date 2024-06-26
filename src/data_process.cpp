#include "obu/data_process.hpp"
data_process::data_process(rclcpp::Node::SharedPtr node){
    node_ = node;
    traffic_signal_array_pub_ = node_->create_publisher<TrafficSignalArray>("/perception/traffic_light_recognition/traffic_signals", rclcpp::QoS(1));

    heading_sub_ = node_->create_subscription<Float32>("sensing/gnss/chc/heading", 10,
    std::bind(&data_process::callback_heading, this, std::placeholders::_1));
};
data_process:: ~data_process(){};
std::string data_process::get_string(){
    json sceneData = {
        {"scene", {
            {"fcw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"icw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"direction", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"bsw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"direction", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"lcw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"direction", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"dnpw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"direction", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"ebw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"direction", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"avw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"clw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"distance", 20.0},
                {"alarm_class", 1}
            }},
            {"evw", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"hdistance", 10.0},
                {"vdistance", 20.0},
                {"alarm_class", 1}
            }},
            {"lta", {
                {"vehicleID", 12548456},
                {"position", 0},
                {"hdistance", 10.0},
                {"ttp", 0.0},
                {"alarm_class", 1}
            }},
            {"ivs", json::array({
                {
                    {"id", 1},
                    {"sign type", 10},
                    {"description", "注意行人"},
                    {"distance", 120},
                    {"latitude", 0.0},
                    {"longitude", 0.0}
                },
                {
                    {"id", 2},
                    {"sign type", 38},
                    {"description", "施工"},
                    {"distance", 120},
                    {"latitude", 0.0},
                    {"longitude", 0.0}
                }
            })},
            {"tjw", {
                {"congestion_level", 1},
                {"range_latitude", 0.0},
                {"range_longitude", 0.0}
            }},
            {"slw", {
                {"speed", 10.0},
                {"latitude", 0.0},
                {"longitude", 0.0}
            }},
            {"preempt", {
                {"direction", 1}
            }},
            {"glosa", {
                {"latitude", 0.0},
                {"longitude", 0.0},
                {"glosa_info", json::array({
                    {
                        {"state_active", 1},
                        {"state", 3},
                        {"timing", 22},
                        {"maneuvers", 2},
                        {"advisory_active", 1},
                        {"max_speed", 0},
                        {"min_speed", 0}
                    },
                    {
                        {"state_active", 0},
                        {"state", 0},
                        {"timing", 0},
                        {"maneuvers", 0},
                        {"advisory_active", 0},
                        {"max_speed", 0},
                        {"min_speed", 0}
                    }
                })}
            }},
            {"rlvw", {
                {"distance", 120},
                {"maneuvers", 0},
                {"state", 0},
                {"timing", 12},
                {"latitude", 0},
                {"longitude", 0}
            }},
            {"vrucw", {
                {"type", 0},
                {"latitude", 0.0},
                {"longitude", 0.0},
                {"position", 0},
                {"planning_count", 1},
                {"planning_list", json::array({
                    {
                        {"path_planning_count", 1},
                        {"path_planning_points", json::array({
                            {
                                {"latitude", 0.0},
                                {"longitude", 0.0}
                            },
                            {
                                {"latitude", 0.0},
                                {"longitude", 0.0}
                            }
                        })}
                    }
                })}
            }}
        }}
    };
    std::string jsonStr = sceneData.dump();
    return jsonStr;
}

void data_process::callback_heading(const Float32::SharedPtr msg){
    if (270<msg->data || msg->data< 90){
        traffic_signal_msg_.map_primitive_id = 11071;
    }
    else{
        traffic_signal_msg_.map_primitive_id = 15666;
    }
    std::cout<<"test"<<std::endl;
}

std::vector<uint8_t> data_process::packed_data(){
    Message msg;
    time_t timep;
    std::vector<uint8_t> byte_stream_data;
    std::string msgbody = get_string();
    time(&timep); /*当前time_t类型UTC时间*/
    msg.timestamp = (uint64_t)timep;
    msg.message_length = msgbody.length();
    msg.message_body = msgbody.data();
    msg.check_code = 0x05;
    byte_stream_data = msg;
    // 输出字节流数据
    // show(byte_stream_data);
    return byte_stream_data;
}

void data_process::parse_data(std::vector<uint8_t> received_data){
    receive_message = received_data;
    traffic_signal_msg_.lights.clear();
    traffic_signal_array_msg_.signals.clear();
    if (receive_message.data_type == 0x01){
        json parsed_json = json::parse(receive_message.message_body);
        // 3：红，6：绿，7：黄
        if (sizeof(parsed_json["scene"])>1){
            // std::cout<<parsed_json<<std::endl;
            // std::cout<<parsed_json["scene"]["glosa"]["glosa_info"][Lanlet_id]["state"]<<std::endl;
            for (int i=0;i<=3;i++){
                if ((parsed_json["scene"]["glosa"]["glosa_info"][i]["state_active"]) == 1 && (parsed_json["scene"]["glosa"]["glosa_info"][i]["state"]) != 0){
                    std::cout<<parsed_json["scene"]["glosa"]["glosa_info"][i]<<std::endl;
                    if ((parsed_json["scene"]["glosa"]["glosa_info"][i]["state"]) == 3){
                        traffic_light_msg_.color = traffic_light_msg_.RED;
                        traffic_light_msg_.shape = traffic_light_msg_.UP_ARROW;
                        traffic_light_msg_.status = traffic_light_msg_.SOLID_ON;
                    }
                    else if ((parsed_json["scene"]["glosa"]["glosa_info"][i]["state"]) == 6){
                        traffic_light_msg_.color = traffic_light_msg_.GREEN;
                        traffic_light_msg_.shape = traffic_light_msg_.UP_ARROW;
                        traffic_light_msg_.status = traffic_light_msg_.SOLID_ON;
                    }
                    else if ((parsed_json["scene"]["glosa"]["glosa_info"][i]["state"]) == 7){
                        traffic_light_msg_.color = traffic_light_msg_.AMBER;
                        traffic_light_msg_.shape = traffic_light_msg_.UP_ARROW;
                        traffic_light_msg_.status = traffic_light_msg_.SOLID_ON;
                    }
                    // std::cout<<"state_active:"<<std::endl;

                    traffic_light_msg_.confidence = 1;
                    traffic_signal_msg_.lights.push_back(traffic_light_msg_);

                    traffic_signal_array_msg_.header.stamp = node_->now();
                    traffic_signal_array_msg_.header.frame_id = "obu";
                    traffic_signal_array_msg_.signals.push_back(traffic_signal_msg_);
                    traffic_signal_array_pub_->publish(traffic_signal_array_msg_);
            }
            else{
                std::cout<<"没有找到灯数据"<<std::endl;
            }
            }
        }
    }
    else{
        std::cout<<"未收到数据"<<std::endl;
    }
}