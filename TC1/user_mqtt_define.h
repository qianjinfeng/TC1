/**
 * @file user_mqtt_define.h
 * @brief MQTT specific configuration file
 */

#ifndef USER_MQTT_DEFINE_H_
#define USER_MQTT_DEFINE_H_

//#define MQTT_USE_SSL

#ifdef MQTT_USE_SSL
#define MQTT_HOST                   "a1d2zX0UdnY.iot-as-mqtt.cn-shanghai.aliyuncs.com" ///< Customer specific MQTT HOST. The same will be used for Thing Shadow
#define MQTT_PORT                   1883 ///< default port for MQTT/S
#define MQTT_ROOT_CA_FILENAME       NULL ///< Root CA file name
#define MQTT_CERTIFICATE_FILENAME   NULL ///< device signed certificate file name
#define MQTT_PRIVATE_KEY_FILENAME   NULL ///< Device private key filename
#define MQTT_USERNAME               "tc1_1&a1d2zX0UdnY"
#define MQTT_PASSWORD               "937957d62c8bf13506c73ef52933381e" // clientid is toshiba
#else
#define MQTT_HOST                   "a1d2zX0UdnY.iot-as-mqtt.cn-shanghai.aliyuncs.com" ///< Customer specific MQTT HOST. The same will be used for Thing Shadow
#define MQTT_PORT                   1883 ///< default port for MQTT/S
#define MQTT_USERNAME               "tc1_1&a1d2zX0UdnY"
#define MQTT_PASSWORD               "937957d62c8bf13506c73ef52933381e"
#endif

#define MQTT_CLIENT_ID "toshiba|securemode=3,signmethod=hmacmd5|"  // clientid is toshiba

#define MQTT_SUB_NAME "/sys/a1d2zX0UdnY/${deviceName}/thing/service/property/set"
//{"method":"thing.service.property.set","id":"80746702","params":{"S1":1},"version":"1.0.0"}
#define MQTT_PUB_NAME "/sys/{productKey}/{deviceName}/thing/event/property/post"
// {
//   "id": "123",
//   "version": "1.0",
//   "params": {
//     "Power": {
//       "value": 1,
//       "time": 1524448722000
//     }
//   },
//   "method": "thing.event.property.post"
// }

#endif /* USER_MQTT_DEFINE_H_ */
