/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#define PORT 4242

const int num_packets = 100;

LOG_MODULE_REGISTER(udp_client, LOG_LEVEL_DBG);

int server_init() {
  int sock = zsock_socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

  if (sock < 0) {
    LOG_ERR("socket failed: %d", -errno);
    return -1;
  }

  return sock;
}

int main(void) {
  uint8_t buf[100] = {1};
  int i, sock;
  struct sockaddr_in6 server_addr = {
      .sin6_family = AF_INET6,
      .sin6_port = htons(PORT),
  };
  inet_pton(AF_INET6, CONFIG_NET_CONFIG_PEER_IPV6_ADDR, &server_addr.sin6_addr);

  sock = server_init();
  if (sock < 0) {
    return sock;
  }

  LOG_INF("UDP server started");

  for (i = 0; i < num_packets; i++) {
    zsock_sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr,
                 sizeof(server_addr));
		LOG_DBG("Sent pkg: %d", i);
    k_sleep(K_MSEC(50));
  }

  return 0;
}
