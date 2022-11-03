#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <yaml-cpp/yaml.h>

uint16_t dest_port;
std::string dest_ip_addr;
// 发送单帧udp的大小，小于65500
uint16_t per_udp_maxsize;
// 模拟数据发送频率，发送一帧数据之后程序sleep时间，单位微妙
float msg_sleep_time;
// 控制udp发送的速率，发送一个udp packet之后程序sleep的时间，单位微秒
float udp_sleep_time;
// 模拟一个完整的数据帧需要拆分的udp packet的数量
uint16_t udp_count;
// 验证数据 在一个udp packet中填充一个特殊的数字
uint8_t flag;

void read_config() {
  // read parameter from config.yaml
  YAML::Node node = YAML::LoadFile("../config.yaml");
  dest_port = node["dest_port"].as<int>();
  dest_ip_addr = node["dest_ip_addr"].as<std::string>();
  per_udp_maxsize = node["per_udp_maxsize"].as<int>();
  msg_sleep_time = node["msg_sleep_time"].as<float>();
  udp_sleep_time = node["udp_sleep_time"].as<float>();
  udp_count = node["udp_count"].as<int>();
  flag=node["flag"].as<int>();
}

int main() {
  read_config();

  /* socket文件描述符 */
  int sock_fd;

  /* 建立udp socket */
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    exit(1);
  }

  /* 设置address */
  struct sockaddr_in addr_serv;
  int len;
  memset(&addr_serv, 0, sizeof(addr_serv));
  addr_serv.sin_family = AF_INET;
  addr_serv.sin_addr.s_addr = inet_addr(dest_ip_addr.c_str());
  addr_serv.sin_port = htons(dest_port);
  len = sizeof(addr_serv);

  int send_num;
  char send_buf[per_udp_maxsize];
  uint64_t index = 1;

  while (1) {
    size_t count = udp_count;
    
    while (count--) {
      memcpy(send_buf, &index, sizeof(index));
      send_num = sendto(sock_fd, send_buf, per_udp_maxsize, 0,
                        (struct sockaddr *)&addr_serv, len);
      std::cout<<"send udp packet size id  is :"<<send_num<<std::endl;
      if (send_num < 0) {
        perror("sendto error:");
        exit(1);
      }
      usleep(udp_sleep_time);
    }

    std::cout << "send message index is : " << index++ << std::endl;

    usleep(msg_sleep_time);
  }

  close(sock_fd);

  return 0;
}
