#ifndef _IOXhop_AutomacaoESP32_CPP_INCLUDED
#define _IOXhop_AutomacaoESP32_CPP_INCLUDED

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "IOXhop_AutomacaoESP32.h"

AutomacaoESP32::AutomacaoESP32() {
	// not use
}

void AutomacaoESP32::begin(String host) {
	begin(host, "");
}

void AutomacaoESP32::begin(String host, String auth) {
	host.replace("http:", "");
	host.replace("/", "");
	_host = host;
	_auth = auth;
}

// Start of get
int AutomacaoESP32::getInt(String path) {
	return _http(path, "GET").toInt();
}

float AutomacaoESP32::getFloat(String path) {
	return _http(path, "GET").toFloat();
}

String AutomacaoESP32::getString(String path) {
	String value = _http(path, "GET");
	return value.substring(1, value.length() - 1);
}

bool AutomacaoESP32::getBool(String path) {
	return _http(path, "GET").indexOf("true") >= 0;
}

void AutomacaoESP32::get(String path, int &value) {
	value = getInt(path);
}

void AutomacaoESP32::get(String path, float &value) {
	value = getFloat(path);
}

void AutomacaoESP32::get(String path, String &value) {
	value = getString(path);
}

void AutomacaoESP32::get(String path, bool &value) {
	value = getBool(path);
}

JsonVariant AutomacaoESP32::get(String path) {
	return StaticJsonBuffer<Automacao_JSON_VALUE_BUFFER_SIZE>().parseObject(_http(path, "GET"));
}
// END of get

// Start of set
void AutomacaoESP32::setInt(String path, int value) {
	set(path, (int)value);
}

void AutomacaoESP32::setFloat(String path, float value, int point) {
	set(path, (float)value);
}

void AutomacaoESP32::setString(String path, String value) {
	set(path, (String)value);
}

void AutomacaoESP32::setBool(String path, bool value) {
	set(path, (bool)value);
}

void AutomacaoESP32::set(String path, int value) {
	_http(path, "PUT", String(value));
}

void AutomacaoESP32::set(String path, float value, int point) {
	_http(path, "PUT", String(value, point));
}

void AutomacaoESP32::set(String path, String value) {
	String buf = "";
	JsonVariant(value.c_str()).printTo(buf);
	_http(path, "PUT", buf);
}

void AutomacaoESP32::set(String path, bool value) {
	_http(path, "PUT", value ? "true" : "false");
}

void AutomacaoESP32::set(String path, JsonVariant value) {
	String bufferJson = "";
	JsonObject& data = value;
	data.printTo(bufferJson);
	_http(path, "PUT", bufferJson);
}
// END of set

// Start of push
String AutomacaoESP32::pushInt(String path, int value) {
	return push(path, value);
}

String AutomacaoESP32::pushFloat(String path, float value, int point) {
	return push(path, value);
}

String AutomacaoESP32::pushBool(String path, bool value) {
	return push(path, value);
}

String AutomacaoESP32::pushString(String path, String value) {
	return push(path, value);
}

String AutomacaoESP32::push(String path, int value) {
	return _pushValue(path, String(value));
}

String AutomacaoESP32::push(String path, float value, int point) {
	return _pushValue(path, String(value, point));
}

String AutomacaoESP32::push(String path, String value) {
	String buf;
	JsonVariant(value.c_str()).printTo(buf);
	return _pushValue(path, buf);
}

String AutomacaoESP32::push(String path, bool value) {
	return _pushValue(path, value ? "true" : "false");
}

String AutomacaoESP32::push(String path, JsonVariant value) {
	String bufferJson = "";
	JsonObject& data = value;
	data.printTo(bufferJson);
	return _pushValue(path, bufferJson);
}

String AutomacaoESP32::_pushValue(String path, String data) {
	String rosJson = _http(path, "POST", data);
	if (failed()) return String();
	
	StaticJsonBuffer<Automacao_RETUEN_JSON_BUFFER_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(rosJson);
	if (!root.success()) {
		_errCode = 1;
		_errMsg = "Automacao not respond json formant, parseObject() failed";
		return String();
	}
	
	if (!root.containsKey("name")) {
		_errCode = 2;
		_errMsg = "Automacao not respond 'name' for object";
		return String();
	}
	
	return root["name"].as<String>();
}
// END of push

// Start of stream
void AutomacaoESP32::stream(String path, AutomacaoStreamHandlerCallback callback) {
	AutomacaoStreamCallback = callback;
	AutomacaoStreamURL = genURL(path);
	
	xTaskCreatePinnedToCore([](void* param) {
		for (;;) {
			delay(5); // Disable WDT
				
			if (!AutomacaoStreamHttp.connected()) {
				AutomacaoStreamHttp.end();
				AutomacaoStreamHttp.begin(AutomacaoStreamURL);
				AutomacaoStreamHttp.setTimeout(5000);
				AutomacaoStreamHttp.addHeader("Accept", "text/event-stream");
				int httpCode = AutomacaoStreamHttp.GET();
				if (httpCode != HTTP_CODE_OK) {
					Serial.println("Error !, Automacao stream fail: " + String(httpCode));
					continue;
				}
				AutomacaoStreamSocket = AutomacaoStreamHttp.getStreamPtr();
			}
			
			if (!AutomacaoStreamSocket) continue;
			
			if (AutomacaoStreamSocket->available()) {
				String line = AutomacaoStreamSocket->readStringUntil(0x0A);
				if (line.startsWith("event:")) {
					AutomacaoStreamEvent = line.substring(7, line.length());
					AutomacaoStreamEvent.trim();
				} else if (line.startsWith("data:")) {
					AutomacaoStreamDataJson = line.substring(6, line.length());
					AutomacaoStreamDataJson.trim();
				} else if (line.length() == 0) {
					if (AutomacaoStreamCallback) AutomacaoStreamCallback(AutomacaoStream(AutomacaoStreamEvent, AutomacaoStreamDataJson));
				}
			}
		}
		vTaskDelete(NULL);
		AutomacaoStreamTaskHandle = NULL;
	}, "AutomacaoStream_Task", Automacao_STREAM_TASK_BUFFER_SIZE, NULL, 3, &AutomacaoStreamTaskHandle, ARDUINO_RUNNING_CORE);
	return;
}

void AutomacaoESP32::stopStream() {
	if (AutomacaoStreamTaskHandle) vTaskDelete(AutomacaoStreamTaskHandle);
	if (AutomacaoStreamHttp.connected()) {
		AutomacaoStreamHttp.end();
	}
	AutomacaoStreamCallback = NULL;
	AutomacaoStreamEvent = "";
	AutomacaoStreamDataJson = "";
}
// END of stream

// Start of remove
void AutomacaoESP32::remove(String path) {
	_http(path, "DELETE");
}
// END of remove

int AutomacaoESP32::failed() {
	return _errCode;
}

String AutomacaoESP32::error() {
	return _errMsg;
}

String AutomacaoESP32::_http(String path, String type, String payload) {
	HTTPClient http;
	http.begin(genURL(path));
	int httpCode = http.sendRequest(type.c_str(), payload);
	if (httpCode == HTTP_CODE_OK) {
		_errCode = 0;
		_errMsg = "";
	} else {
		_errCode = httpCode;
		_errMsg = http.errorToString(httpCode);
	}
	String ros = http.getString();
	return ros == "null" ? "" : ros;
}

String AutomacaoESP32::genURL(String path) {
	String url = "http://" + _host + "/" + path;
	if (_auth.length() > 0) url ;
	return url;
}

AutomacaoESP32 Automacao = AutomacaoESP32();

#endif