#include <DHT.h>
#include <SPI.h>
#include <WiFiNINA.h>

#define brocheBranchementDHT 6   // Si la ligne de données du DHT22 est branchée sur la pin D6 de votre Arduino, par exemple
#define typeDeDHT DHT22          // Si votre DHT utilisé est un DHT22 (mais cela pourrait aussi bien être un DHT11 ou DHT21, par exemple)

DHT dht(brocheBranchementDHT, typeDeDHT);
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "YOUR_NETWORK_SSID";        // your network SSID (name)
char pass[] = "YOUR_NETWORK_PASSWORD";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;  
char buffer[20];

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
  dht.begin();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

  float floatHumidity = dht.readHumidity();              // Lecture du taux d'humidité (en %)
  float floatTemperature = dht.readTemperature();   // Lecture de la température, exprimée en degrés Celsius

  // Vérification si données bien reçues
  if (isnan(floatHumidity) || isnan(floatTemperature)) {
    Serial.println("Aucune valeur retournée par le DHT22. Est-il bien branché ?");
    delay(2000);
    return;         // Si aucune valeur n'a été reçue par l'Arduino, on attend 2 secondes, puis on redémarre la fonction loop()
  }

  sprintf(buffer, "%0.2f", floatHumidity);
  String humidity = buffer;
  sprintf(buffer, "%0.2f", floatTemperature);
  String temperature = buffer;

    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.print("{");
          client.print("\"humidity\":");
          client.print(humidity);
          client.print(",");
          client.print("\"temperature\":");
          client.print(temperature);
          client.print("}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}