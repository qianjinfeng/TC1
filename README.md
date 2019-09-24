# TC1

## 参考
 感谢大神们的辛苦劳作
- [a2633063](https://github.com/a2633063/zTC1)
- [ljr.im](https://ljr.im/articles/streamline-the-fibonacci-tc1-firmware/)

## 主要变化
- 去掉定时功能
- Easylink配网

## 开发环境

-  [Python, Git, MiCoder Tool](http://developer.mxchip.com/handbooks/102#%E5%AE%89%E8%A3%85)

-  MiCO Cube  
```
    pip install mico-cube
    mico config --global MICODER D:\Workspace\MiCoder_v1.1.Win32\MiCoder
```

## 编译代码
    git clone https://github.com/qianjinfeng/TC1.git
    cd TC1
    mico deploy
    mico make TC1@MK3031@moc

## [烧录固件](https://github.com/a2633063/zTC1/wiki/%E5%9B%BA%E4%BB%B6%E7%83%A7%E5%BD%95) 
 GND/3v3/clk/dio <----> Jlink's GND/3v3/clk/dio  
 手持也可以。只是有点累！

## 使用
#### 插座wifi设置
- 下载[easylink](http://developer.mxchip.com/downloads/8)  
- easylink App 启动发送
- 按住TC1按钮，插上电源，进入配网模式，松开按钮
- 灯短闪几下后，长亮，配网成功

#### 配置MQTT到插座
使用[UDP工具](http://developer.mxchip.com/handbooks/106#socket-%E8%B0%83%E8%AF%95%E5%B7%A5%E5%85%B7)与插座通信  

- udp receive 插座ip 10181  
  监听插座的消息，配网成功有消息发出来

- udp send 插座ip 10182  
  发送下面消息，配置mqtt broker, etc.  
```
    {"mac":"macaddress","setting":{"mqtt_uri":"mqtt broker","mqtt_port":1883,"mqtt_user":"username","mqtt_password":"password"}}
```

\*\* 长按10s, 可清空mqtt, wifi连接信息 \*\*  

#### HomeAssistant配置
- 安装[Configurator](https://www.home-assistant.io/docs/ecosystem/hass-configurator/#configuration-ui-for-home-assistant)  
- 修改configuration.yaml，增加如下信息，自动发现插座实体
```
    mqtt:
        #MQTT Broker的IP地址或者域名
        broker: xx
        #MQTT Broker的端口号，缺省为1883
        port: xx
        #用户名
        username: xx
        #密码
        password: xx
        #自动发现
        discovery: true
        #自动发现topic前缀，与固件user_mqtt_client的一致
        discovery_prefix: homeassistant
```

- 手动修改configuration.yaml配置插座实体 (不推荐，好处name不是自动生成)
```
    sensor:
    - platform: mqtt
        name: "tc1_1_power"
        state_topic: "homeassistant/sensor/macxxxx/power/state"
        icon: 'mdi:gauge'
        unit_of_measurement: 'W'
        value_template: '{{ value_json.power }}'

    switch:
    - platform: mqtt
        name: 'tc1_1_plug_0'
        state_topic: "homeassistant/switch/macxxxx/plug_0/state"
        command_topic: "cmnd/tc1_macxxxx"
        payload_on: "{\"mac\":\"macxxxx\",\"plug_0\":1}"
        payload_off: "{\"mac\":\"macxxxx\",\"plug_0\":0}"
        qos: 0
        retain: true
```

#### OTA
搭建http-server    
```
npm install http-server -g
cd .\build\TC1@MK3031@moc\binary
copy TC1@MK3031@moc.ota.bin ota.bin
htt-server 
```  

udp send 插座ip 10182  
```
    {"mac":"macaddress","setting":{"ota":"http://httpserveripaddress:8080/ota.bin"}}
```
