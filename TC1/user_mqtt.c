#include "mico.h"
#include "user_mqtt_define.h"
#include "mqtt_client_interface.h"
#include "cJSON/cJSON.h"
#include "main.h"
#include "user_gpio.h"

#define mqtt_log(M, ...) custom_log("mqtt", M, ##__VA_ARGS__)

MQTT_Client mqttclient;
uint32_t uId = 0;

char *mqtt_client_id_get( char clientid[30] )
{
    uint8_t mac[6];
    char mac_str[13];

    mico_wlan_get_mac_address( mac );
    sprintf( mac_str, "%02X%02X%02X%02X%02X%02X",
             mac[0],
             mac[1], mac[2], mac[3], mac[4], mac[5] );
    sprintf( clientid, "MiCO_%s", mac_str );

    return clientid;
}

bool mqtt_isconnect( )
{
    return mqtt_is_client_connected(&mqttclient);
}

//更新开关状态
OSStatus mqtt_send_plug_state( unsigned char plug_id )
{
    OSStatus err = kUnknownErr;
    char *send_buf = NULL;
    send_buf = malloc( 128 ); //
    if ( send_buf != NULL )
    {
        sprintf( send_buf, "{"
                 "\"id\":\"%d\","
                 "\"version\":\"1.0\","
                 "\"params\":\"{\\\"S%d\\\":\\\"{\\\"value\\\":%d}\\\"}\","
                 "\"method\":\"thing.event.property.post\""
                 "}",uId++, plug_id, user_config->plug[plug_id] );
        
        IoT_Publish_Message_Params paramsQOS0;
        paramsQOS0.qos = QOS0;
        paramsQOS0.payload = (void *) send_buf;
        paramsQOS0.payloadLen = strlen( send_buf );
        paramsQOS0.isRetained = 0;
        mqtt_publish( &mqttclient, MQTT_PUB_NAME, strlen( MQTT_PUB_NAME ), &paramsQOS0 );
    }
    if ( send_buf ) free( send_buf );
    return err;
}
//更新PowerConsumption状态
OSStatus mqtt_send_power( void )
{
    OSStatus err = kUnknownErr;
    char *send_buf = NULL;
    send_buf = malloc( 128 ); //
    if ( send_buf != NULL )
    {

        sprintf( send_buf, "{"
                 "\"id\":\"%d\","
                 "\"version\":\"1.0\","
                 "\"params\":\"{\\\"TotalConsumption\\\":\\\"{\\\"value\\\":%ld.%ld}\\\"}\","
                 "\"method\":\"thing.event.property.post\""
                 "}",uId++, power / 10, power % 10  );

        IoT_Publish_Message_Params paramsQOS0;
        paramsQOS0.qos = QOS0;
        paramsQOS0.payload = (void *) send_buf;
        paramsQOS0.payloadLen = strlen( send_buf );
        paramsQOS0.isRetained = 0;
        mqtt_publish( &mqttclient, MQTT_PUB_NAME, strlen( MQTT_PUB_NAME ), &paramsQOS0 );
    }
    if ( send_buf ) free( send_buf );
    return err;
}

void iot_subscribe_callback_handler( MQTT_Client *pClient, char *topicName,
                                     uint16_t topicNameLen,
                                     IoT_Publish_Message_Params *params,
                                     void *pData )
{
    IOT_UNUSED( pData );
    IOT_UNUSED( pClient );
    mqtt_log("Subscribe callback");
    mqtt_log("%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, (char *)(params->payload));
    cJSON * pJsonRoot = cJSON_Parse( (char *)(params->payload) );
    if ( !pJsonRoot )
    {
        mqtt_log( "this is not a json data:\r\n%s\r\n", (char *)(params->payload)  );
        return;
    }
    cJSON *p_params = cJSON_GetObjectItem( pJsonRoot, "params" );
    if ( p_params )
    {
        //解析plug-----------------------------------------------------------------
        unsigned char i;
        for ( i = 0; i < PLUG_NUM; i++ )
        {
            char plug_str[2];
            sprintf( plug_str, "S%d", i);
            cJSON *p_plug = cJSON_GetObjectItem( p_params, plug_str );
            if ( p_plug )
            {
                if ( cJSON_IsNumber( p_plug ) )
                {
                    //switch on/off 
                    user_relay_set( i, p_plug->valueint );
                    //report to MQTT server
                    mqtt_send_plug_state(i);
                }
            }
        }
    }
    cJSON_Delete( pJsonRoot );
}

static void mqtt_sub_pub_main( mico_thread_arg_t arg )
{
    IoT_Error_t rc = MQTT_FAILURE;

    char clientid[40];
    // char cPayload[100];
    // int i = 0;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    // IoT_Publish_Message_Params paramsQOS0;
    // IoT_Publish_Message_Params paramsQOS1;

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     */
    mqttInitParams.enableAutoReconnect = false;
    mqttInitParams.pHostURL = MQTT_HOST;
    mqttInitParams.port = MQTT_PORT;
    mqttInitParams.mqttPacketTimeout_ms = 20000;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.disconnectHandler = NULL;
    mqttInitParams.disconnectHandlerData = NULL;
#ifdef MQTT_USE_SSL
    mqttInitParams.pRootCALocation = MQTT_ROOT_CA_FILENAME;
    mqttInitParams.pDeviceCertLocation = MQTT_CERTIFICATE_FILENAME;
    mqttInitParams.pDevicePrivateKeyLocation = MQTT_PRIVATE_KEY_FILENAME;
    mqttInitParams.isSSLHostnameVerify = false;
    mqttInitParams.isClientnameVerify = false;
    mqttInitParams.isUseSSL = true;
#else
    mqttInitParams.pRootCALocation = NULL;
    mqttInitParams.pDeviceCertLocation = NULL;
    mqttInitParams.pDevicePrivateKeyLocation = NULL;
    mqttInitParams.isSSLHostnameVerify = false;
    mqttInitParams.isClientnameVerify = false;
    mqttInitParams.isUseSSL = false;
#endif

    rc = mqtt_init( &mqttclient, &mqttInitParams );
    if ( MQTT_SUCCESS != rc )
    {
        mqtt_log("aws_iot_mqtt_init returned error : %d ", rc);
        goto exit;
    }

    connectParams.keepAliveIntervalInSec = 30;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = MQTT_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen( MQTT_CLIENT_ID );
    connectParams.isWillMsgPresent = false;
    connectParams.pUsername = MQTT_USERNAME;
    connectParams.usernameLen = strlen(MQTT_USERNAME);
    connectParams.pPassword = MQTT_PASSWORD;
    connectParams.passwordLen = strlen(MQTT_PASSWORD);

RECONN:
    mqtt_log("Connecting...");
    rc = mqtt_connect( &mqttclient, &connectParams );
    if ( MQTT_SUCCESS != rc )
    {
        mqtt_log("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
        sleep(2);
        goto RECONN;
    }

    mqtt_log("Subscribing...");  ///sys/a1d2zX0UdnY/${deviceName}/thing/service/property/set
    rc = mqtt_subscribe( &mqttclient, MQTT_SUB_NAME, strlen( MQTT_SUB_NAME ), QOS0,
                         iot_subscribe_callback_handler, NULL );
    if ( MQTT_SUCCESS != rc )
    {
        mqtt_log("Error subscribing : %d ", rc);
        goto RECONN;
    }

    // mqtt_log("publish...");
    // sprintf( cPayload, "%s : %d ", "hello from SDK", i );

    // paramsQOS0.qos = QOS0;
    // paramsQOS0.payload = (void *) cPayload;
    // paramsQOS0.isRetained = 0;

    // paramsQOS1.qos = QOS1;
    // paramsQOS1.payload = (void *) cPayload;
    // paramsQOS1.isRetained = 0;

    while ( 1 )
    {
        //Max time the yield function will wait for read messages
        rc = mqtt_yield( &mqttclient, 100 );
        if ( MQTT_SUCCESS != rc )
        {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            mico_rtos_thread_sleep( 1 );
            mqtt_log("mqtt disconnect");
            mqtt_disconnect( &mqttclient );
            goto RECONN;
        }

        mqtt_log("-->sleep, rc:%d", rc);
        // sprintf( cPayload, "%s : %d ", "hello from SDK QOS0", i++ );
        // paramsQOS0.payloadLen = strlen( cPayload );
        // mqtt_publish( &mqttclient, MQTT_SUB_NAME, strlen( MQTT_SUB_NAME ), &paramsQOS0 );

        // sprintf( cPayload, "%s : %d ", "hello from SDK QOS1", i++ );
        // paramsQOS1.payloadLen = strlen( cPayload );
        // rc = mqtt_publish( &mqttclient, MQTT_SUB_NAME, strlen( MQTT_SUB_NAME ), &paramsQOS1 );
        // if ( rc == MQTT_REQUEST_TIMEOUT_ERROR )
        // {
        //     mqtt_log("QOS1 publish ack not received");
        //     rc = MQTT_SUCCESS;
        // }
    }

    exit:
    mico_rtos_delete_thread( NULL );
}

OSStatus start_mqtt_sub_pub( void )
{
#ifdef MQTT_USE_SSL
    uint32_t stack_size = 0x3000;
#else
    uint32_t stack_size = 0x2000;
#endif
    return mico_rtos_create_thread( NULL, MICO_APPLICATION_PRIORITY, "mqtt", mqtt_sub_pub_main,
                                    stack_size,
                                    0 );
}
