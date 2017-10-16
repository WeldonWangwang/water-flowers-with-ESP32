# Water flowers with ESP32  

## 概述  

通过将 ESP32 作为主控，来检测土壤的湿度情况，并通过对湿度情况的判断，来决定是否利用水泵对土壤进行浇水。 在非浇水状态下主控将处于睡眠模式，最大程度的节省电量。 

**优势**：  
> 1. 利用 ESP32 的低功耗模式，在睡眠状态下能耗很低，可以最大程度的节省外界电源电量消耗；  
> 2. 利用 ESP32 的 WiFi 功能，可以相互连接，可扩展性强；  

## 方案  

**方案一： TEST_MODE**  
![](https://i.imgur.com/gU8uH5z.png)

在该模式下，ESP32 在非浇花时处于睡眠模式，但是外部传感器和 LM393 比较器持续供电，当土壤中湿度发生变化值达到预先设定值时，LM393 输出高电平，对ESP32使用外部（EXT1）唤醒，ESP32 控制继电器使水泵工作，完成一次浇花，重新进入睡眠模式。  

**方案二：TIMER_MODE**  
![](https://i.imgur.com/IYjQFvB.png)

在该模式下，在非浇花时，ESP32 处于睡眠模式，外部传感器和 LM393 都处于断电模式，ESP32 由定时器定期唤醒，在唤醒后首先触发继电器，对传感器和 LM393 供电，然后读取土壤湿度值，对土壤熟读值进行判断，当达到预先设定值时，开启继电器2控制水泵工作，完成一次浇花，重新进入睡眠模式。  

**方案比较**  
在测试中，为了测试传感器的工作灵敏度与相关其他特性，可以选择测试模式。  
在正常使用时，可采用 TIMER 模式，在该模式下，在 ESP32 睡眠时，其余外围电路在继电器控制下均处于非上电状态，因此最大程度的做到最低功耗。

## 硬件设计  

|名称 |数量 |备注 |  
|:---:|:---:|:---:|  
|ESP32_Core_board|1|主控|  
|YL-69|1|土壤湿度传感器|
|LM-393|1|电压比较器|  
|JQC-3FF-S-Z|1|继电器|
|LED|1|测试中使用 LED 代替 motor|
|2N5401|2|三极管|

**ESP32 接线：**

|引脚|功能|
|:---:|:---:|
|5V|lm393 和 继电器供电|
|3.3V|LED 和 驱动控制连接 lm393 的三极管供电|
|GPIO 34|sensor 输入|
|GPIO 4|LED(motor) 控制|
|GPIO 5|继电器控制|
|GND|外围共地引脚|
**硬件连接图：**  

方案一：  
![](https://i.imgur.com/V0LZnR5.png)

方案二：  
![](https://i.imgur.com/Y0iY05M.png)

*注：以上方案连接图为大体示意图，省略芯片供电和部分电阻构成的外围电路，具体连接请参考下方图片。*

## 软件设计(TIMER MODE)
**ESP32 deep sleep mode:**  
ESP32 在程序中设置为 5s 唤醒一次，唤醒后开启传感器对土壤湿度进行检测。  

    const int wakeup_time_sec = 5;   //if set this as 43200,it is 12 hours
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);

**传感器控制与读数：**  
传感器的工作与否由继电器控制，对传感器读数采取  ADC 读数，电压阈值设定为 500*3.6 mv. 

电压阈值可以由测试环境自行设定，在 demo 中的测试环境为“有水”和“无水”模拟土壤的“湿”与“干”。  

由于传感器在初次上电时会有一个内部初始化，所以传感器的第一次读数往往不准确，因此在程序中，对传感器进行连续两次读数，上电后选择传感器的第二次读数作为检测的结果。  

**浇水功能**  
由于用 LED 代替 motor，LED 正极通过一个限流电阻连接到 3.3v 电源，负极连接到 GPIO 4.  GPIO_MOTOR
因此当 GPIO 4置低时，可以导通 LED， GPIO 4 置高时， LED熄灭。  
浇水时间由检测到的土壤湿度决定，在 demo 中设置为固定值 3s， 土壤湿度越低，输出电压越大，则浇水时间可以相应进行延长。  

## 测试  

唤醒后检测到土壤湿度较大：  

    I (261) gpio: GPIO[25]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    I (261) gpio: GPIO[4]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    I (271) gpio: GPIO[5]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    Wake up from timer. Time spent in deep sleep: 642217ms
    Enabling timer wakeup, 5s
    194.4 mV  
    **********************NO WATERING**********************
    deep_sleep_start

唤醒后检测到土壤湿度较小：   

    I (261) gpio: GPIO[25]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    I (261) gpio: GPIO[4]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    I (271) gpio: GPIO[5]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
    Wake up from timer. Time spent in deep sleep: 672792ms
    Enabling timer wakeup, 5s
    3664.8 mV 
    **********************START WATERING**********************
    **********************END WATERING**********************
    deep_sleep_start



