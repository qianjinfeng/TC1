#ifndef __MAIN_H_
#define __MAIN_H_

#include "mico.h"
#include "MiCOKit_EXT.h"

#define VERSION "v0.10.4"

#define TYPE 1
#define TYPE_NAME "tc1"

#define ZTC_NAME "tc1_%s"

#define USER_CONFIG_VERSION 2
#define SETTING_MQTT_STRING_LENGTH_MAX  32      //必须 4 字节对齐。

#define PLUG_NAME_LENGTH 32
#define PLUG_NUM 6              //插座数量
#define PLUG_TIME_TASK_NUM 5    //每个插座最多5组定时任务

#define Led         MICO_GPIO_5
#define Button      MICO_GPIO_23
#define POWER      MICO_GPIO_15

#define Relay_ON     1
#define Relay_OFF     0

#define Relay_0     MICO_GPIO_6
#define Relay_1     MICO_GPIO_8
#define Relay_2     MICO_GPIO_10
#define Relay_3     MICO_GPIO_7
#define Relay_4     MICO_GPIO_9
#define Relay_5     MICO_GPIO_18
#define Relay_NUM   PLUG_NUM

// #define CONFIG_SSID "Xiaomi_duoduo"                 //WiFi名称
// #define CONFIG_USER_KEY "xxx"         //WiFi密码
// #define CONFIG_MQTT_IP "192.168.1.3"                //MQTT服务器IP
// #define CONFIG_MQTT_PORT 1883                   //MQTT服务器端口     
// #define CONFIG_MQTT_USER "mqtt"            //MQTT用户名
// #define CONFIG_MQTT_PASSWORD "xxx"    //MQTT密码

//用户保存参数结构体
typedef struct
{
    char mqtt_ip[SETTING_MQTT_STRING_LENGTH_MAX];   //mqtt service ip
    int mqtt_port;        //mqtt service port
    char mqtt_user[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
    char mqtt_password[SETTING_MQTT_STRING_LENGTH_MAX];     //mqtt service user
//     char mqtt_device_id[SETTING_MQTT_STRING_LENGTH_MAX];        //mqtt service user  device name

    char version;
    char plug[PLUG_NUM];
    char user[maxNameLen];
} user_config_t;

extern char rtc_init;


extern uint32_t total_time;
extern char strMac[16];
extern uint32_t power;
extern system_config_t * sys_config;
extern user_config_t * user_config;

extern mico_gpio_t Relay[Relay_NUM];

#endif
