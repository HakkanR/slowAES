/**
 * @file infinityFree.ino
 * @author Hakan ÖZMEN (hakkanr@gmail.com)
 * @brief
 * the website given this example is hosted on InfinityFree Servers
 * You can use this example any of website hosted on InfinityFree.
 * This example get praytimes for Turkey/Istanbul city,
 * 
 * You should get an apikey from web site by registering apikey form:
 * http://www.namazvakitleri.site/api/apireg.php
 * 
 * when you've got an e-mail which contains your api key, put your
 * apiKey below. For more details about using api please visit :
 * http://www.namazvakitleri.site/api/index.php
 * 
 * @version 1.0.0
 * @date 2023-02-05
 *
 * @section for more information please visit :
 * https://www.devrelerim.com/2023/01/nodemcu-namaz-vakitlerini-cekme.html
 * https://www.youtube.com/HakkanR
 * https://github.com/HakkanR/slowAES
 *
 * @copyright BDS License, all above text must be included any redistribution.
 *
 */
#include <Arduino.h>
#include "slowAES.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char *ssid = "";		  // Wifi SSID
const char *pwd = ""; // Wifi password
String apiKey = ""; //apiKey
String web = "http://www.namazvakitleri.site/api/api.php?api=vakitler&api-key=" + apiKey + "&il=istanbul";
HTTPClient http;
WiFiClient wfc;

void setup()
{
	Serial.begin(115200);
	Serial.print(ssid);
	Serial.println(" connecting.");
	WiFi.begin(ssid, pwd);
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(500);
	}
	Serial.println(" connected.");
	http.begin(wfc, web);

	int httpCode = http.GET();
	Serial.println(httpCode);
	String payload = http.getString();

	/* getting abc vars */
	int loc = payload.indexOf("var a=toNumbers(");
	String temp = payload.substring(loc, temp.length());
	temp.replace("var a=toNumbers(\"", "");
	String a = temp.substring(0, 32);

	temp.replace(a + "\"),b=toNumbers(\"", "");
	String b = temp.substring(0, 32);

	temp.replace(b + "\"),c=toNumbers(\"", "");
	String c = temp.substring(0, 32);

	uint8_t aNums[16];
	uint8_t bNums[16];
	uint8_t cNums[16];

	toNumbers(a.c_str(), aNums);
	toNumbers(b.c_str(), bNums);
	toNumbers(c.c_str(), cNums);

	uint8_t finalCookie[33];
	slowAES _slowAES;
	uint8_t resNums[16];
	_slowAES.decrypt(cNums, aNums, bNums, resNums);
	toHex(resNums, finalCookie);
	String str = (char *)finalCookie;

	web = web + "&i=1";
	Serial.println(web);
	http.begin(wfc, web);
	String fullCookieStr = "__test=" + str;
	http.addHeader("Cookie", fullCookieStr, true);
	http.addHeader("Host", "www.namazvakitleri.site", true);
	http.addHeader("Connection", "Keep-Alive", true);

	httpCode = http.GET();
	Serial.println(httpCode);
	payload = http.getString();
	Serial.println(payload);
	// and parse the JSON
}

void loop()
{
	// nothing goes here!
}
