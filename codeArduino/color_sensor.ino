#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // thu vien Arduin Json v5
#include "DHT.h" // thư vien DHT sensors

/*---DHT11---*/
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define S0 D4
#define S1 D5
#define S2 D6
#define S3 D7
#define sensorOut D8

const char* ssid = "lophocvui.com 2.4GHz";
const char* password = "88889999";

// Thông tin về MQTT Broker
#define mqtt_server "tailor.cloudmqtt.com"
#define mqtt_port  15168
#define mqtt_user "lbmcmryr" // user mqtt 
#define mqtt_pwd "f_IDn5pEcB08" //password mqtt
#define mqtt_topic "color" // topic of stationx
#define mqtt_topic_sensor "sensor" // topic of stationx
#define DeviceID  "NodeMCU" // id of station x
WiFiClient espClient;
PubSubClient client(espClient);  

// Khởi tạo các biến lưu giá trị tần số đo được bởi photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

// Khởi tạo các biến lưu giá trị mức độ sáng của màu
int redColor = 0;
int greenColor = 0;
int blueColor = 0;


void setup_wifi() {
  delay(10);
  // Hàm kết nối wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  dht.begin();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

}

void reconnect() {
  // Lặp đến khi kết nối thành công
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client",mqtt_user,mqtt_pwd)) {
      Serial.println("connected");
 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Đặt tỉ lệ tần số tương ứng 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
 
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
 client.loop();
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  
  //Đọc giá tần số đầu ra tương ứng với ánh sáng đỏ
  redFrequency = pulseIn(sensorOut, LOW);
  
  // Xác định giá trị thang đo màu đỏ (R) từ 0 đến 255 dựa theo tần số đo được
  // Cần phải thay thế giá trị XX bằng giá trị đo được từ tần số đo được ở bước trước
  // ví dụ: redColor = map(redFrequency, XX, XX, 255,0);
  redColor = map(redFrequency, 20, 25, 255,0);
  int R = redColor;
  
  Serial.print("redFrequency= ");//printing name
  Serial.print(redFrequency);//printing RED color frequency
  Serial.print("  ");
  delay(50);
  
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  
  //Đọc giá tần số đầu ra tương ứng với ánh sáng xanh lục
  greenFrequency = pulseIn(sensorOut, LOW);
  // Xác định giá trị thang đo màu xanh (G) từ 0 đến 255 dựa theo tần số đo được
  // Cần phải thay thế giá trị XX bằng giá trị đo được từ tần số bước trước
  // Ví dụ: greenColor = map(greenFrequency, XX, XX, 255,0);
  greenColor = map(greenFrequency, 28, 38, 255, 0);
  int G = greenColor;

  Serial.print("greenFrequency= ");//printing name
  Serial.print(greenFrequency);//printing RED color frequency
  Serial.print("  ");
  delay(50);

  // 
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Đọc giá tần số đầu ra tương ứng với ánh sáng lam
  blueFrequency = pulseIn(sensorOut, LOW);
  
  // Xác định giá trị thang đo màu xanh lam (B) từ 0 đến 255 dựa theo tần số đo được
  // Cần phải thay thế giá trị XX bằng giá trị đo được từ tần số bước trước
  // Ví dụ: blueColor = map(blueFrequency, XX, XX, 255,0);
  blueColor = map(blueFrequency, 18, 25, 255, 0);
  int B = blueColor;
  
  // In giá trị ra Serial
  Serial.print("blueFrequency= ");//printing name
  Serial.print(blueFrequency);//printing RED color frequency
  Serial.println("  ");
  delay(50);

  // Đếm số lượng dựa theo màu sắc
  if( R > 0 &&  redFrequency < greenFrequency && redFrequency < blueFrequency){
    //
      Serial.println(" - RED detected!");
      client.publish("product","red");
  }
  if(G > 0 && greenFrequency < redFrequency && greenFrequency < blueFrequency){
    Serial.println(" - GREEN detected!");
    client.publish("product","green");
  }
  if(B > 0 && blueFrequency < redFrequency && blueFrequency < greenFrequency){
    Serial.println(" - BLUE detected!");
    client.publish("product","blue");
  }

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();//Allocates an empty JsonObject
  char mesage[256];// mang ky tu
  data["R"] = R;
  data["G"] = G ;
  data["B"] = B ;
  data.printTo(mesage, sizeof(mesage));// lưu thanh chuoi JSON;
  client.publish(mqtt_topic,mesage);
  Serial.println(mesage);
  delay(2000);  

  // Lấy dữ liệu cảm biến DHT và gửi lên web server
  JsonObject& data_sensor = jsonBuffer.createObject();//Allocates an empty JsonObject
  float h = dht.readHumidity();
  float t = dht.readTemperature();
    
  if (isnan(h)){ h = 0; } 
  if (isnan(t)){ t = 0; }
  // gan du lieu vao chuoi Json
  data_sensor["ID"] = DeviceID;
  data_sensor["Temperature"] = t ;
  data_sensor["Humidity"] = h;
    
  data_sensor.printTo(mesage, sizeof(mesage));// lưu thanh chuoi JSON
  // gui du lieu len ThingsBoard
  Serial.print("Format data: ");
  Serial.println(mesage);
  client.publish(mqtt_topic_sensor,mesage);
  
}
