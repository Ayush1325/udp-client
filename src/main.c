/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>
#include <zephyr/timing/timing.h>

#define PORT 4242
#define REQS 50

const int num_packets = 100;

LOG_MODULE_REGISTER(udp_client, LOG_LEVEL_DBG);

int server_init() {
  int sock = zsock_socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

  if (sock < 0) {
    LOG_ERR("socket failed: %d", -errno);
    return -1;
  }

  return sock;
}

int main(void) {
  timing_t start_time, end_time;
  uint64_t total_cycles, total_ns;
  int sock, ret, buf = 0, i;
  struct sockaddr_in6 server_addr = {
      .sin6_family = AF_INET6,
      .sin6_port = htons(PORT),
  };
  inet_pton(AF_INET6, CONFIG_NET_CONFIG_PEER_IPV6_ADDR, &server_addr.sin6_addr);

  timing_init();

  sock = server_init();
  if (sock < 0) {
    return sock;
  }

  LOG_DBG("Socket Created");

  ret =
      zsock_connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (ret < 0) {
    LOG_ERR("connect failed: %d", -errno);
    return -1;
  }

  LOG_INF("TCP client started");

  k_sleep(K_MSEC(1000));

  timing_start();
  start_time = timing_counter_get();

  for (i = 0; i < REQS; ++i) {
    ret = zsock_send(sock, &i, sizeof(i), 0);
    if (ret < 0) {
      LOG_ERR("send failed: %d", -errno);
      return -1;
    }

    ret = zsock_recv(sock, &buf, sizeof(buf), 0);
    if (ret < sizeof(i) || buf != i) {
      LOG_ERR("recv failed: ret: %d, err: %d, buf: %d", ret, -errno, buf);
      return -1;
    }
  }

  end_time = timing_counter_get();

  total_cycles = timing_cycles_get(&start_time, &end_time);
  total_ns = timing_cycles_to_ns(total_cycles);

  LOG_INF("Total time: %llu ns", total_ns);
  LOG_INF("Time per request: %llu ns", total_ns / REQS);
  LOG_INF("Time per request: %llu ms", total_ns / (REQS * 1000000));

  timing_stop();

  return 0;
}
