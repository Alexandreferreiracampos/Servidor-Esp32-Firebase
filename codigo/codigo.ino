#include <WiFi.h>         
#include <IOXhop_FirebaseESP32.h>
#include <IOXhop_AutomacaoESP32.h>
#include <ArduinoJson.h>          


#define WIFI_SSID "Alexandre"                   
#define WIFI_PASSWORD "91906245"         
#define FIREBASE_HOST "***********"
#define AUTOMACAO_HOST "192.168.0.200"     
#define FIREBASE_AUTH "***********"   

hw_timer_t *timer = NULL;

int ledVermelho = 18;
int ledVerde = 2;

void IRAM_ATTR resetModule(){
    ets_printf("(watchdog) reiniciar\n"); //imprime no log
    ESP.restart(); //reinicia o chip
}

void setup() {
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledVerde, LOW);
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Conectando ao wifi");

  long tme = millis();
  
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Automacao.begin(AUTOMACAO_HOST);

   timer = timerBegin(0, 80, true); //timerID 0, div 80
    //timer, callback, interrupção de borda
    timerAttachInterrupt(timer, &resetModule, true);
    //timer, tempo (us), repetição
    timerAlarmWrite(timer, 15000000, true); //igual a 3 segundos
    timerAlarmEnable(timer); //habilita a interrupção 

    while (WiFi.status() != WL_CONNECTED)
  {
      digitalWrite(ledVerde, HIGH);
      delay(500);
      digitalWrite(ledVerde, LOW);
      delay(500);

  }

}

void loop() {

   timerWrite(timer, 0); //reseta o temporizador (alimenta o watchdog) 
   long tme = millis(); //tempo inicial do loop

  
  //reconectar caso o wifi desconect
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(ledVerde, HIGH);
    timerWrite(timer, 0);
    }else{
     resetModule();
    }

    delay(50);
  timerWrite(timer, 0);
  //salvar response da API entre {} para poder ser convertida em json
  String dateString = "{"+Firebase.getString("SmartHome")+"}";
  timerWrite(timer, 0);
  //convert dados em json
  StaticJsonBuffer<300> JSONBuffer;
  JsonObject& parsed = JSONBuffer.parseObject(dateString);
  delay(100);
  
  boolean luzgaragem = parsed["Luzgaragem"];
  boolean Andelas = parsed["arandelas"];
  boolean Leds = parsed["leds"];
  boolean Portao = parsed["portao"];
  timerWrite(timer, 0);

        if(Portao == true){
          digitalWrite(ledVerde,LOW);
          timerWrite(timer, 0);
          Firebase.setString("SmartHome/portao", "false");
          timerWrite(timer, 0);
          delay(100);
          Automacao.getString("relea");
          timerWrite(timer, 0);
          digitalWrite(ledVerde,HIGH);
        }
        delay(800);
        timerWrite(timer, 0);
        if(luzgaragem == true){
        digitalWrite(ledVerde,LOW);
          timerWrite(timer, 0);
          Firebase.setString("SmartHome/Luzgaragem", "false");
          timerWrite(timer, 0);
          delay(100);
          Automacao.getString("rele3");
          timerWrite(timer, 0);
          digitalWrite(ledVerde,HIGH);
        }
        delay(800);
        timerWrite(timer, 0);
        if(Andelas == true){
          digitalWrite(ledVerde,LOW);
          timerWrite(timer, 0);
          Firebase.setString("SmartHome/arandelas", "false");
          timerWrite(timer, 0);
          Automacao.getString("rele4");
          timerWrite(timer, 0);
          digitalWrite(ledVerde,HIGH);
        }
        delay(800);
        timerWrite(timer, 0);
        if(Leds == true){
         digitalWrite(ledVerde,LOW);
          timerWrite(timer, 0);
          Firebase.setString("SmartHome/leds", "false");
          timerWrite(timer, 0);
          delay(100);
          Automacao.getString("rele1");
          timerWrite(timer, 0);
          digitalWrite(ledVerde,HIGH);
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
