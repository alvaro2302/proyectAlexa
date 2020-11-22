#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define LED_BUILTIN 2


int Sensor = 23;
unsigned int temperature;
DHT dht (Sensor, DHT11);

const char* SSID = "Puente COMTECO-N3317583";
const char* PASSWORD = "6fa6c9c13c09";
const char* AWS_HOST = "adhy50rq4z8k7-ats.iot.us-east-2.amazonaws.com";
const int AWS_PORT = 8883;

const char* CLIENT_ID = "sis234_mcu";
const char* IN_TOPIC = "ucbcba/sis234_in";   // subscribe
const char* OUT_TOPIC = "ucbcba/sis234_out"; // publish

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVANrUGGgUsBQE0hOV33tUy7TBU4OXMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDEwMjIwMDQ5
NDFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDczirvSYBFadZWtjqz
loLvqztng3MC775UMb/+okqbFTTkpEZXNX+getsarosOQUZSKoVGGzjBwMRAnKMl
08P48VarBcTpM3iw/Mm7butjuoNdkBwRaeeHhD5Fd4f8WjNwFeJ14wh2IDf7Rlzr
nJK5+wBGp040sXnBHVm4D3fDpTSYCRQSsqpONZQFsVb9Er2pWmYaV7XbhOpuuFEE
DfWaqh62KiA7oq6BXh1uzEC3t7QUs6vNkMlyBvWd5hSvHWjUJ/nUcECgatJes9ad
jWH1JH8fYe1DMJP9NIv+QcFF/CTpbXsYv5QRCwju55fCQjt5TefvT9V1CG13/uKl
zuPDAgMBAAGjYDBeMB8GA1UdIwQYMBaAFFQQW8nRfU70ov1CETg3wsFc0SpjMB0G
A1UdDgQWBBTUKI48Lx0nzCcUr+jyZnWosWdNpjAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAXJFmDVYNIVww8NAptJ4+cSOP
XbWyuc2pgjiHQ8hDq8Sxgscxywt06Qj8uPEUKme4eTR+9sG2V4m2ZWfV64zGgfsT
RhTaJb6CbbrPhSp+N19DbZ4p5JtLpdQDErjK4u5ve5Qscm0iQpByF9mmhW70g+gj
eycNfaN/qOtZpkGDSNO3ZWPVEUjh7w3ikwpHWFrmNomYFd5idL7ALJhh03gaCCe5
7kulhx81RDcixmX5zzrOF1SP+EoSMpVLp6YxJ7dtDDzbhBI6ul/kYmAw/f15+YX/
Ua2XvBrrQooifoDPZC50zsXzCIKSe9apATHh7jsgGRQgsI8vYQjM3xBymlipEA==
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEoQIBAAKCAQEA3M4q70mARWnWVrY6s5aC76s7Z4NzAu++VDG//qJKmxU05KRG
VzV/oHrbGq6LDkFGUiqFRhs4wcDEQJyjJdPD+PFWqwXE6TN4sPzJu27rY7qDXZAc
EWnnh4Q+RXeH/FozcBXideMIdiA3+0Zc65ySufsARqdONLF5wR1ZuA93w6U0mAkU
ErKqTjWUBbFW/RK9qVpmGle124TqbrhRBA31mqoetiogO6KugV4dbsxAt7e0FLOr
zZDJcgb1neYUrx1o1Cf51HBAoGrSXrPWnY1h9SR/H2HtQzCT/TSL/kHBRfwk6W17
GL+UEQsI7ueXwkI7eU3n70/VdQhtd/7ipc7jwwIDAQABAoIBAQDIkfJIOEDvTKtp
StDZzw+TO7yTU8FGWpZODn7zg2I3qzTHuD04/Jy5HsAmZYRk54BEJOG5BAMzZLIg
auh79PegC0GJyozKtHmec5Gf06q4hXTMqZoFa2joHYUfPuvkm977orV6FkfaXXvc
oVbIowpZLBpdE56jmM8bT1jvLAXDTX5JtivaqcHHtrSv6h6j1/spctytYXQ/e7hj
63czdxuV2lkB88yFTVlGsPeTQ4Lnd8EsFMN26ijkm70NqnKMP68oTUSOcSj/d1Pi
rgTgS+AePZt11PGRfqPCNny1cDyO8OMsd1RHTN08LANMaFtVFz1kbepnZbL+Mbnc
C2Q//GmZAoGBAP0LFY9gTH9sdROyNk1oormJ8t2L58MBI1V+vX019b0QTU1CdJ8b
PP3PyGnC9s/Pfx62TnCGbbrSwyplHPYQUyS6PVbJJeWnv36sLUHRdEvv8nt2cGlZ
pj8PcmZsV3/Nu/iIk7eFTxHJOtmAkhekAeVAzeOqwBJQS3j4x/eIoAYfAoGBAN9i
ptZtwEcpqvJQtTAPFDJNMZcU6kR1sc3XHKTtkSlnVkp7anrLzXvM0UFaANsfxx9D
IcZm5Wi8Sfu/rdDzLYTgXTOGs3yZ5fSYvMIRuq0GLmqPOMGdd64rNm9ukkAhryIM
Ktexucq/qk+I+uG7jH86xyPFNDqoSO7qiRmNFQXdAoGAMQ8PFQcUjbfXUwRAJu3c
bqBiJ7XSLypvUUoLLtVhWVBpzErQFXDQeFzqfCbCdzPBb5Dk0EgfyhLGp9eGr+Ym
WH6LGeJRqJ7HL6Xt+E7Ff1t75Zfm32X0J1G1WkozBKI8iUjhark7xTUAZd7ZdB/m
Bi50PGmEZ4iNuVxyWHlynP0Cf0hss6rR6oUn/USS5iuut92XKXzSSyHnBr61/5N5
cpb56gA8A0nMydJZlX6ar6xboTOBkHHC6p4KbfIxMymLrjZmh9Es2wSK4QNsgcZp
+Rwf5Oyngr9DToopeHyoXMJuKVngmANWm+YgPheLUcveAq89X1M5nLVn2j2bzNd7
5lUCgYBZyUR4eeMwJzNGXeogfek/CjzRCyduKpb4Mr2cMeCTVQmtRGcIwD+X0j6H
GZxb2f0wOQmD8mOjMOHDhAo9uRTYHmmcyu+/2vxXyxx5+vmnr7LJFzTQI/0+OCKQ
+UvtaKiK1A86X7VITcqlRplSj6MSSpea7uPxv9yf4t8oZpQiYw==
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
}

StaticJsonDocument<JSON_OBJECT_SIZE(1)> inputDoc;

// PubSubClient callback function
void callback(const char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += String((char) payload[i]);
  }
  if (String(topic) == IN_TOPIC) {
    Serial.println("Message from topic " + String(IN_TOPIC) + ":" + message);

    DeserializationError err = deserializeJson(inputDoc, payload);
    if (!err) {
      String action = String(inputDoc["action"].as<char*>());
      if (action == "LedOn") ledOn();
      else if (action == "LedOff") ledOff();
      else Serial.println("Unsupported action received: " + action);
    }
  }
}

boolean mqttClientConnect() {
  Serial.println("Connecting to AWS IoT...");
  if (mqttClient.connect(CLIENT_ID)) {
    Serial.println("Connected to " + String(AWS_HOST));

    mqttClient.subscribe(IN_TOPIC);

    Serial.println("Subscribed to " + String(IN_TOPIC));
  } else {
    Serial.println("Couldn't connect to AWS IoT.");
  }
  return mqttClient.connected();
}

void setup() {
  
  dht.begin();
   
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Connecting to WiFi...");

  WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Couldn't connect to WiFi.");
    while(1) delay(100);
  }
  Serial.println("Connected to " + String(SSID));

  wifiClient.setCACert(AMAZON_ROOT_CA1);
  wifiClient.setCertificate(CERTIFICATE);
  wifiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(AWS_HOST, AWS_PORT);
  mqttClient.setCallback(callback);
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;

unsigned char intToChar = (unsigned char)temperature;

StaticJsonDocument<JSON_OBJECT_SIZE(1)> outputDoc;
char outputBuffer[128];


void publishMessage(unsigned char value) {
  outputDoc["Temperature"] = value;
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(OUT_TOPIC, outputBuffer);
}


void loop() {
   temperature = dht.readTemperature();
   delay(2000);
                          
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    
    // Connect to MQTT broker
    
    if (now - previousConnectMillis >= 5000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) {
        previousConnectMillis = 0;
      } else delay(1000);
    }
  } else {
    // This should be called regularly to allow the client to process incoming messages and maintain its connection to the server
    mqttClient.loop();
    delay(100);

    if (now - previousPublishMillis >= 1000) {
    previousPublishMillis = now;
    // Publish message
    publishMessage(temperature);
    }
  }
}
