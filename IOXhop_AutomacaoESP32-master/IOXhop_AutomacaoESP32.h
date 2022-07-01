#ifndef _IOXhop_AutomacaoESP32_H_INCLUDED
#define _IOXhop_AutomacaoESP32_H_INCLUDED

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "IOXhop_AutomacaoStream.h"

#define Automacao_JSON_VALUE_BUFFER_SIZE 1024
#define Automacao_RETUEN_JSON_BUFFER_SIZE 512
#define Automacao_STREAM_TASK_BUFFER_SIZE 32768

typedef void(*AutomacaoStreamHandlerCallback)(AutomacaoStream);
static HTTPClient AutomacaoStreamHttp;
static WiFiClient *AutomacaoStreamSocket;
static AutomacaoStreamHandlerCallback AutomacaoStreamCallback;
static TaskHandle_t AutomacaoStreamTaskHandle;
static String AutomacaoStreamURL = "";
static String AutomacaoStreamEvent;
static String AutomacaoStreamDataJson;

class AutomacaoESP32 {
	public:
		AutomacaoESP32() ;
		void begin(String host) ;
		void begin(String host, String auth) ;
		
		// Get
		int getInt(String path) ;
		float getFloat(String path) ;
		String getString(String path) ;
		bool getBool(String path) ;
		void get(String path, int &value) ;
		void get(String path, float &value) ;
		void get(String path, String &value) ;
		void get(String path, bool &value) ;
		JsonVariant get(String path) ;

		// Set
		void setInt(String path, int value) ;
		void setFloat(String path, float value, int point = 2) ;
		void setString(String path, String value) ;
		void setBool(String path, bool value) ;
		void set(String path, int value) ;
		void set(String path, float value, int point = 2) ;
		void set(String path, String value) ;
		void set(String path, bool value) ;
		void set(String path, JsonVariant value) ;
		
		// Push
		String pushInt(String path, int value) ;
		String pushFloat(String path, float value, int point = 2) ;
		String pushBool(String path, bool value) ;
		String pushString(String path, String value) ;
		String push(String path, int value) ;
		String push(String path, float value, int point = 2) ;
		String push(String path, String value) ;
		String push(String path, bool value) ;
		String push(String path, JsonVariant value) ;
		
		// Stream Event
		void stream(String path, AutomacaoStreamHandlerCallback callback) ;
		void stopStream() ;
		
		// remove
		void remove(String path) ;
		
		// is fail
		int failed() ;
		String error() ;
		
	private:
		String _host = "", _auth = "";
		int _errCode = 0;
		String _errMsg = "";
		
		String _pushValue(String path, String data) ;
		String _http(String url, String type, String payload = "") ;
		String genURL(String path) ;
		
}
;

extern AutomacaoESP32 Automacao;

#endif