
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <secrets.h>

#define MAX_WIFI_INIT_RETRY 10
#define WIFI_RETRY_DELAY 500

#define ESP_SUB_ROLE

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client, MQTT_SERVER, MQTT_BROKER_PORT);
bool mqtt_status;

#define LED_PIN 15
#define NUM_LEDS 8 //40 o 84
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

//Wifi Initialization function
int WiFi_init()
{
  int retries = 0;

  Serial.println("Connecting to WiFi AP..........");

  WiFi.mode(WIFI_STA); //set wifi station mode

  WiFi.begin(SSID, PASSWORD); //start connecting to WiFi AP

  //check the status of WiFi connection to be WL_CONNECTED
  while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY))
  {
    retries++;
    delay(WIFI_RETRY_DELAY);
    Serial.println("#");
  }
  Serial.println(String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]));

  return WiFi.status(); //return the WiFi connection status
}

//MQTT callback function invoked for every MQTT received message on a subscribed topic
void mqtt_callback(const MQTT::Publish &pub)
{
  Serial.println("MQTT receiving a message:");
  Serial.println(pub.payload_string());

  if (pub.payload_string() == "yellow")
  {
    Serial.println("Accendo!");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      // accende tutti i led di Rosso
      pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // imposta il colore per ogni led.
      pixels.show();                                      // This sends the updated pixel color to the hardware.
    }
  }

  if (pub.payload_string() == "off")
  {
    Serial.println("Accendo!");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      // accende tutti i led di Rosso
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // imposta il colore per ogni led.
      pixels.show();                                  // This sends the updated pixel color to the hardware.
    }
  }

  if (pub.payload_string() == "green")
  {
    Serial.println("Accendo!");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      // accende tutti i led di Rosso
      pixels.setPixelColor(i, pixels.Color(102, 204, 0)); // imposta il colore per ogni led.
      pixels.show();                                      // This sends the updated pixel color to the hardware.
    }
  }

  if (pub.payload_string() == "red")
  {
    Serial.println("Accendo!");
    for (int i = 0; i < NUM_LEDS; i++)
    {
      // accende tutti i led di Rosso
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // imposta il colore per ogni led.
      pixels.show();                                    // This sends the updated pixel color to the hardware.
    }
  }
}

int MQTT_init(boolean topic_subscribe)
{
  Serial.println("Initializing MQTT communication.........");

  mqtt_client.set_callback(mqtt_callback); //set callback on received messages
  mqtt_client.set_max_retries(255);

  //here we connect to MQTT broker and we increase the keepalive for more reliability
  if (mqtt_client.connect(MQTT::Connect(MQTT_CLIENT_ID).set_keepalive(90).set_auth(String(MQTT_UNAME), String(MQTT_PASSW))))
  {
    Serial.println("Connection to MQTT broker SUCCESS..........");

    //if role is SUB subscribe to topic
    if (topic_subscribe)
    {
      if (mqtt_client.subscribe(MQTT_TOPIC))
      {
        Serial.println("Subscription to MQTT topic [" + String(MQTT_TOPIC) + "] SUCCESS.........");
      }
      else
      {
        Serial.println("MQTT unable to subscribe to [" + String(MQTT_TOPIC) + "] ERROR.........");
        mqtt_client.disconnect();
        return false;
      }
    }
  }
  else
  {
    Serial.println("Connection to MQTT broker ERROR..........");
  }

  return mqtt_client.connected();
}

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(100);
  pixels.begin();
  pixels.clear();
  pixels.show();
  Serial.println();
  Serial.println("MQTT_basic starting....");
  if (WiFi_init() != WL_CONNECTED)
  {
    Serial.println("WiFi connection ERROR....");
  }
  else
  {
    Serial.println("WiFi connection OK....");
    mqtt_status = MQTT_init(true);
    if (!mqtt_status)
      Serial.println("MQTT connection ERROR....");
    else
      Serial.println("MQTT connection OK....");
  }
}

void loop()
{
  if (mqtt_status)
  {
    mqtt_client.loop();
    delay(100);
  }
}