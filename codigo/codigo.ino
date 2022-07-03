
#include <WiFi.h>         
#include <IOXhop_FirebaseESP32.h>
#include <IOXhop_AutomacaoESP32.h>
#include <ArduinoJson.h>                   

#define WIFI_SSID "Alexandre"                   
#define WIFI_PASSWORD "91906245"         
#define FIREBASE_HOST "automacao-smarthome-default-rtdb.firebaseio.com"
#define AUTOMACAO_HOST "192.168.0.200"     
#define FIREBASE_AUTH "4TSGAJ30yNAYgGmDX6e0ownR88zF4PogqlV4LEyO"   

void setup() {
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Conectando ao wifi");
  
  while (WiFi.status() != WL_CONNECTED)
  {
      digitalWrite(2, HIGH);
      delay(500);
      digitalWrite(2, LOW);
      delay(500);
  }
  
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Automacao.begin(AUTOMACAO_HOST);


}


void loop() {
  
  //reconectar caso o wifi desconect
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(2, HIGH);
    }else{
      digitalWrite(2, HIGH);
      delay(500);
      digitalWrite(2, LOW);
      delay(500);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }

  //salvar response da API entre {} para poder ser convertida em json
  String dateString = "{"+Firebase.getString("SmartHome")+"}";

  //convert dados em json
  StaticJsonBuffer<300> JSONBuffer;
  JsonObject& parsed = JSONBuffer.parseObject(dateString);
  delay(300);

  //verificar se os dados foram convertidos
  if(!parsed.success()){
    Serial.println("Erro na leitura do Json");
    delay(5000);
    return;
  }

  boolean luzgaragem = parsed["Luzgaragem"];
  boolean Andelas = parsed["arandelas"];
  boolean Leds = parsed["leds"];
  boolean Portao = parsed["portao"];

  
    if(luzgaragem == true){
        digitalWrite(5,HIGH);
         delay(100);
          digitalWrite(2,LOW);
          delay(1000);
          Firebase.setString("SmartHome/Luzgaragem", "false");
          Automacao.getString("rele3");
          delay(100);
          digitalWrite(5,LOW);
          delay(100);
          digitalWrite(2,HIGH);
        delay(100);
        }
        delay(1000);
        if(Andelas == true){
          digitalWrite(5,HIGH);
          delay(100);
          digitalWrite(2,LOW);
          delay(1000);
          Firebase.setString("SmartHome/arandelas", "false");
          Automacao.getString("rele4");
          delay(100);
          digitalWrite(5,LOW);
          delay(100);
          digitalWrite(2,HIGH);
          delay(100);
        }
        delay(1000);
        if(Leds == true){
          digitalWrite(5,HIGH);
          delay(100);
          digitalWrite(2,LOW);
          delay(1000);
          Firebase.setString("SmartHome/leds", "false");
          Automacao.getString("rele1");
          delay(100);
          digitalWrite(5,LOW);
          delay(100);
          digitalWrite(2,HIGH);
          delay(100);
        }
        delay(1000);
        if(Portao == true){
          digitalWrite(5,HIGH);
          delay(100);
          digitalWrite(2,LOW);
          delay(1000);
          Firebase.setString("SmartHome/portao", "false");
          Automacao.getString("relea");
          delay(100);
          digitalWrite(5,LOW);
          delay(100);
          digitalWrite(2,HIGH);
          delay(100);
        }

        delay(2000);
      
//Exemplo da função Get

/*
  Serial.println();
  Serial.print("Dono do quarto: ");
  Serial.print(Firebase.getString("/quarto/dono"));
  Serial.println();
  Serial.print("Luminosidade do quarto: ");
  Serial.print(Firebase.getInt("/quarto/luminosidade"));
  Serial.println();
  Serial.print("Temperatura do quarto: ");
  Serial.print(Firebase.getFloat("/quarto/temperatura"));
  Serial.println();
  Serial.print("O quarto está ocupado: ");
  Serial.print(Firebase.getBool("/quarto/ocupado"));
  Serial.println();
  Serial.print("Luminosidade da sala: ");
  Serial.print(Firebase.getInt("/sala/luminosidade"));
  Serial.println();
  delay(3000);
*/

//Exemplo da função Set

/*
  Firebase.setString("/quarto/dono", "Rebeca");
  Firebase.setInt("/quarto/luminosidade", 300);
  Firebase.setBool("/quarto/ocupado", false);
  Firebase.setFloat("/quarto/temperatura", 24.7);
  Firebase.setInt("/sala/luminosidade", 200);
  delay(3000);
  Firebase.setString("/quarto/dono", "Matteo");
  Firebase.setInt("/quarto/luminosidade", 500);
  Firebase.setBool("/quarto/ocupado", true);
  Firebase.setFloat("/quarto/temperatura", 35.3);
  Firebase.setInt("/sala/luminosidade", 500);
  delay(3000);
*/

//Exemplo da função Push

/*
  Firebase.pushString("/quarto/registro", "Matteo");
  delay(3000);
  Firebase.pushString("/quarto/registro", "Rebeca");
  delay(3000);
  Firebase.pushString("/quarto/registro", "Vanderson");
  delay(3000);
  Firebase.pushString("/quarto/registro", "Raquel");
  delay(3000);
*/

}
