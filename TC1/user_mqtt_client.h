#ifndef __USER_MQTT_CLIENT_H_
#define __USER_MQTT_CLIENT_H_


#include "mico.h"

#define MQTT_CLIENT_KEEPALIVE   30
//#define MQTT_CLIENT_SUB_TOPIC1   "device/ztc1/set"
//#define MQTT_CLIENT_PUB_TOPIC   "device/ztc1/%s/state"
#define MQTT_CLIENT_SUB_TOPIC   "cmnd/%s"       //命令控制接收topic，%s代表名称，默认tc1_xxxxxxxxxxxx（xxx为mac地址）
#define MQTT_CLIENT_PUB_TOPIC   "stat/%s"       //状态信息topic，%s代表名称，默认tc1_xxxxxxxxxxxx（xxx为mac地址）
#define MQTT_CMD_TIMEOUT        5000  // 5s
#define MQTT_YIELD_TMIE         5000  // 5s


extern OSStatus user_mqtt_init(void);
extern OSStatus user_mqtt_send( char *arg );
extern bool user_mqtt_isconnect(void);
extern OSStatus user_mqtt_send_plug_state(unsigned char plug_id );
extern OSStatus user_mqtt_hass_auto( char plug_id );
//extern void user_mqtt_hass_auto_name(char plug_id);
extern OSStatus user_mqtt_hass_power( void );
extern OSStatus user_mqtt_hass_auto_power( void );
//extern OSStatus user_mqtt_hass_auto_power_name(void);
#endif
