//add thu vien 
#include <mbed.h>
#include "esp8266.h"
#include <dht11.h>
#include <ccs811.h>
#include <TextLCD.h>
#include <stdio.h>

//Khoi tao chan va bien
//Serial pc(USBTX,USBRX);
AnalogIn KY37 (PA_6);
DigitalOut buzz (PA_4);
Dht11 dht(PB_4);
I2C i2c(PB_11,PB_10);
CCS811 ccs811(I2C_SDA, I2C_SCL);
TextLCD_I2C lcd(&i2c,0x4E,TextLCD::LCD20x4, TextLCD::HD44780);
ESP8266 wifi(USBTX,USBRX,115200);
//buffers for wifi library
char resp[1000];
char http_cmd[300], comm[300];

//khoi tao thong so wifi SSID and password
//#define SSID "VoNgocHoangTrinh" 
#define SSID "Ambition"
//#define PASS "0934648013"  
#define PASS "11111113"  
#define IP "184.106.153.149" //Ip Thingspeak

//bien chi so
float value = 0; 
int ECO2 = 0;
int TVOC = 0;
int T = 0;
int humid = 0;

//Update key for thingspeak
char* Update_Key = "AKJJAS0VI78REIAZ";
//ham khoi tao wifi
void kt_wifi(void)
{
    lcd.printf("Reset module");
    wifi.Reset();
    wait(1);
    lcd.cls();
 
    lcd.printf("Set mode with AP");
    wifi.SetMode(1); 
     wait(1); 
    lcd.cls();
    
    lcd.printf("Join network");
    wifi.Join(SSID, PASS);
     wait(1);     
     lcd.cls();
        
    lcd.printf("Get IP and MAC");
    wifi.GetIP(resp);
    wait(1);     
    lcd.cls();
    
    lcd.printf("UART passthrough");
    wifi.setTransparent();          
    lcd.cls();

    wait(1);      
    lcd.printf("single connection");
    wifi.SetSingle();             
    wait(1);
    lcd.cls();
}
//Ham phu send_Data
void wifi_send(void)
{
    //lcd.printf("TCP connection");
    wifi.startTCPConn(IP,80);    //cipstart
    wait_ms(100);
    //lcd.cls();
    //create link 
    sprintf(http_cmd,"/update?api_key=%s&field1=%f&field2=%d&field3=%d&field4=%d&field5=%d\r\n",Update_Key,value,T,humid,ECO2,TVOC);
    printf(http_cmd);
    //lcd.printf("Sending URL\r\n");
    wait(1);
    //lcd.printf("Sending URL");
    wifi.sendURL(http_cmd, comm);   //cipsend and get command
    //lcd.cls();
}
//Ham bao hieu
void buu(void)
{
    if(ECO2 > 500)
    {
        buzz = 1;
    }
    else buzz = 0;
}
//Ham thu thap du lieu
void sensordata(void)
{
   ccs811.readData(&ECO2, &TVOC);
    value = 100*KY37.read();
    dht.read();
    T = dht.getCelsius();
    humid = dht.getHumidity();
    lcd.locate(0,0);
    lcd.printf("CO2:%d ppm",ECO2);
    lcd.locate(0,1);
    lcd.printf("TVOC:%d ppb",TVOC);
    lcd.locate(0,2);
    lcd.printf("value:%f",value);
    lcd.locate(0,3);
    lcd.printf("T:%d*C,  H:%d/100", T, humid);
    //wait(5);
    //lcd.cls(); 
}
//ham khoi tao lcd
void kt_lcd(void)
{
lcd.setMode(TextLCD::DispOn); //DispOff, DispOn
lcd.setBacklight(TextLCD::LightOff);//LightOff, LightOn
lcd.setCursor(TextLCD::CurOff_BlkOff);//CurOff_BlkOff, CurOn_BlkOff, CurOff_BlkOn, CurOn_BlkOn
}

//MAIN_viet chuc nang chuong trinh chinh o day
int main() 
{
kt_lcd();
kt_wifi();
ccs811.init();  
lcd.printf("PLEASE WAITING");
wait(5);
lcd.cls(); 
    while(1) 
    {
    sensordata();
    wait(10);
    wifi_send();
    lcd.cls();
    }
}