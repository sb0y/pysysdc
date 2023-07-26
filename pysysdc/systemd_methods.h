#pragma once

int unit_control(const char *unit_name, const char* action, char **return_data);
int unit_enable(const char *unit_name, char *return_data[]);
int unit_disable(const char *unit_name, char *return_data[]);
int unit_mask(const char *unit_name, char *return_data[]);
int unit_unmask(const char *unit_name, char *return_data[]);
int set_hostname(const char *method, const char *value);
int get_hostname(const char *attr, char **ret);
int unit_status(const char *unit_name, char *return_data[]);
int machine_reboot(const char *wall_message);
int machine_poweroff(const char *wall_message);
int set_wall_message(const char *wall_message);
int logind_method(const char *method, const char *wall_message);
int set_timezone(const char *timezone);
int daemon_reload(void);