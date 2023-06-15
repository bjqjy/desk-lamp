
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLECharacteristic.h>
#define SERVICE_UUID "b408e1a0-3d8a-11ed-b878-0242ac120002"         //服务UUID
#define CHARACTERISTIC_UUID "dfd84064-3d8a-11ed-b878-0242ac120002"  //特征UUID
#define CONTROL_UUID "de045162-3d97-11ed-b878-0242ac120002"         //控制特征UUID
#define LEDS_COUNT  8    //彩灯数目
#define LEDS_PIN	6    //ESP32控制ws2812的引脚
#define CHANNEL		0    //控制通道，最多8路
const int int1 = 2;  //控制WS2812B
const int int2 = 3;
const int ena = 10;

const int int3 = 5;  //控制马达
const int int4 = 4;
const int enb = 8; // 马达PWM

const int freq = 5000;//PWM频率
const int ledChannel1 = 4;//信号生成GPIO
const int resolution = 8;//8位分辨率，by Author. C3可设置1~14bits
const int PIR = 1; //红外控制引脚 
char state = 48;

BLECharacteristic controlCharacteristic(CONTROL_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);//申请一个彩灯控制对象
//设置服务器所需要的配置
void setup() {
  strip.begin();      //初始化彩灯控制引脚
	strip.setBrightness(30);//设置彩灯亮度
  Serial.begin(115200);
  BLEDevice::init("esp32");                                   //创建设备
  BLEServer *pServer = BLEDevice::createServer();               //设置为服务器
  BLEService *pService = pServer->createService(SERVICE_UUID);  //使用上面的服务UUID创建服务
  // 添加一个带有对象名（官方UUID）的特征，不带对象，这个特征不会改变
  BLECharacteristic *nameCharacteristic = pService->createCharacteristic(BLEUUID((uint16_t)0x2A00), BLECharacteristic::PROPERTY_READ);
  nameCharacteristic->setValue("TT");                  //显示特征名
  pService->addCharacteristic(&controlCharacteristic);  //增加一个控制LED的特性
  controlCharacteristic.setValue(&state);

  //设置特征， 使用上面的特征UUID，需要将特征的属性作为参数传递。此情况下是读或写
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setValue("Hello HWD!!!");  //创建完特征后，可以使用setValue()方法为其在此赋值
  //此值可以是其他传感器的值
  pService->start();
  //下面是启动服务和广播，以便其他BLE设备找到此 BLE 设备
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();  //BLE广播
  pAdvertising->addServiceUUID(SERVICE_UUID);                  //广播添加服务 UUID
  pAdvertising->setScanResponse(true);                         //广播扫描响应
  pAdvertising->setMinPreferred(0x06);                         //广播设置最小首选
  pAdvertising->setMinPreferred(0x12);                         //广播设置最小首选
  BLEDevice::startAdvertising();                               //BLE 设备启动广播，特征已经定义，可以在手机中读取它
  pinMode(int1, OUTPUT);
  pinMode(int2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(int3, OUTPUT);
  pinMode(int4, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(PIR,INPUT);
  ledcSetup(ledChannel1, freq, resolution);
  ledcAttachPin(enb,ledChannel1);
  digitalWrite(int1,HIGH);
  digitalWrite(int2,LOW);
  digitalWrite(int3,HIGH);
  digitalWrite(int4,LOW);
  digitalWrite(ena,HIGH);
  strip.setAllLedsColor(242,233,219);
  ledcWrite(ledChannel1,255);
  delay(100);
  ledcWrite(ledChannel1,140);
  tt();
}

void loop() {
  std::string controlValue = controlCharacteristic.getValue();  //字符串控件获取特征值
  if (controlValue[0] != state) {
    state = controlValue[0];
    if (state == 48) {
      digitalWrite(int1, LOW);
      digitalWrite(int3, LOW);
    } else if (state == 49) {
      ledcWrite(ledChannel1,255);
      delay(1000);
      ledcWrite(ledChannel1,140);
      tt();
    } else if(state == 50){
      ledcWrite(ledChannel1,255);
      delay(1000);
      ledcWrite(ledChannel1,100);
      tt();              
    } else if(state==51)
      digitalWrite(int1, HIGH);
      strip.setBrightness(30);
      strip.setAllLedsColor(242,233,219);
    } else if(state==52){
      digitalWrite(int1, HIGH);
      strip.setBrightness(60);
      for (int j = 0; j < 255; j += 2) {
      for (int i = 0; i < LEDS_COUNT; i++) {
      strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));//设置彩灯颜色数据
      }
      }
          strip.show();//显示颜色
          delay(5);
    } else if(state==53){
      if(digitalRead(PIR)){
      digitalWrite(int1, HIGH);
      strip.setAllLedsColor(0,255,0);
      delay(5000);
      digitalWrite(int1, LOW);      
      }
    }

}


void tt(){
  while(1){
  digitalWrite(int3,LOW);
  digitalWrite(int4,HIGH);
  delay(1000);
  digitalWrite(int4,LOW);
  digitalWrite(int3,HIGH);
  delay(1000);
  }
  digitalWrite(int3,LOW);
}