#pragma once

typedef struct sd_bus sd_bus;
struct epoll_event;

int run_sd_bus_process_threads(sd_bus *bus);
int run_epoll_wait_threads(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);