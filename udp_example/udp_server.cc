#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <sys/time.h>

#include <yaml-cpp/yaml.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

uint16_t server_port;
uint16_t per_udp_maxsize;
uint16_t udp_count;

void read_config() {
  // read parameter from config.yaml
  YAML::Node node = YAML::LoadFile("../config.yaml");
  server_port = node["server_port"].as<int>();
  per_udp_maxsize = node["per_udp_maxsize"].as<int>();
  udp_count = node["udp_count"].as<int>();
}

int main() {
  read_config();

  /* sock_fd --- socket文件描述符 创建udp套接字*/
  int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("socket");
    exit(1);
  }

  /* 将套接字和IP、端口绑定 */
  struct sockaddr_in addr_serv;
  int len;
  memset(&addr_serv, 0, sizeof(struct sockaddr_in)); //每个字节都用0填充
  addr_serv.sin_family = AF_INET;                    //使用IPV4地址
  addr_serv.sin_port = htons(server_port);           //端口
  addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);     //自动获取IP地址
  len = sizeof(addr_serv);

  /* 绑定socket */
  if (bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0) {
    perror("bind error:");
    exit(1);
  }

  int recv_num;
  char recv_buf[per_udp_maxsize];
  struct sockaddr_in addr_client;
  uint16_t last_index = 0;
  uint16_t current_index = 0;
  uint16_t receive_counts = 0;
  uint64_t start_time;
  uint64_t end_time;

  while (1) {
    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                        (struct sockaddr *)&addr_client, (socklen_t *)&len);

    std::cout << "reveive udp packet size is : " << recv_num << std::endl;
    // std::cout << "flag is : " << recv_buf[per_udp_maxsize - 2] << std::endl;

    if (recv_num < 0) {
      perror("recvfrom error:");
      exit(1);
    }

    // read index from buf
    current_index = (uint16_t)recv_buf[0];
    if (current_index != last_index) {
      // receive a new msg
      // get currnt time;
      if (receive_counts == udp_count) {
        std::cout << "reveive a complete msg , msg index is : " << last_index
                  << std::endl;
      } else {
        std::cout << "msg index is : " << last_index << ", receive "
                  << receive_counts << " udp packets, "
                  << ", lose " << udp_count - receive_counts << " udp packet"
                  << std::endl;
      }
      receive_counts = 1;
      last_index = current_index;
    } else if (current_index == last_index) {
      // index_cout+1;
      // get currrnt time;
      receive_counts++;
    }
  }

  close(sock_fd);

  return 0;
}
