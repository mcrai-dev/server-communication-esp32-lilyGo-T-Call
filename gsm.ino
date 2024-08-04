/*
    * Ce code Arduino configure un module GSM pour effectuer des requêtes HTTP GET vers un serveur. 
    * Auteur: M'Crai Laydam
    * Date: 04/08/2024

*/

// Sélection de type de modem :
#define TINY_GSM_MODEM_SIM800

// Définir le port série pour la console de débogage (vers le moniteur série, vitesse par défaut 115200)
#define SerialMon Serial
// Définir le port série pour les commandes AT
#define SerialAT Serial1

// Définir la console série pour les impressions de débogage, si nécessaire
#define TINY_GSM_DEBUG SerialMon

// définir le PIN GSM, le cas échéant
#define GSM_PIN ""

// Vos identifiants GPRS, le cas échéant
const char apn[] = "internet"; // APN (ORANGE: orangenet , TELMA: internet, AIRTEL: internet)
const char gprsUser[] = "";
const char gprsPass[] = "";

// Définir la console série pour les impressions de débogage, si nécessaire
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);

// Pins TTGO T-Call
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

#define OUTPUT_1             13
#define OUTPUT_2             15

// I2C pour SIM800 (pour le maintenir en marche lorsqu'il est alimenté par batterie)
TwoWire I2CPower = TwoWire(0);

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

long lastMsg = 0;

bool setPowerBoostKeepOn(int en) {
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  if (en) {
    I2CPower.write(0x37); // Définir bit1 : 1 enable 0 disable boost keep on
  } else {
    I2CPower.write(0x35); // 0x37 est la valeur par défaut du registre
  }
  return I2CPower.endTransmission() == 0;
}

void setup() {
  // Définir le débit en bauds de la console
  SerialMon.begin(115200);
  delay(10);
  
  // Démarrer la communication I2C
  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
  
  // Maintenir l'alimentation en cas de fonctionnement sur batterie
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Définir les broches de réinitialisation, d'activation et d'alimentation du modem
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  
  pinMode(OUTPUT_1, OUTPUT);
  pinMode(OUTPUT_2, OUTPUT);
  
  SerialMon.println("Wait...");

  // Définir le débit en bauds du module GSM et les broches UART
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Le redémarrage prend pas mal de temps
  // Pour l'éviter, appelez init() au lieu de restart()
  SerialMon.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem Info: ");
  SerialMon.println(modemInfo);

  // Débloquez votre carte SIM avec un code PIN si nécessaire
  if (strlen(GSM_PIN) && modem.getSimStatus() != 3) {
    modem.simUnlock(GSM_PIN);
  }

  SerialMon.print("Connecting to APN: ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    ESP.restart();
  } else {
    SerialMon.println(" OK");
  }
  
  if (modem.isGprsConnected()) {
    SerialMon.println("GPRS connected");
  }

  // Envoyer une requête HTTP GET à une URL de test simple
  if (client.connect("example.com", 80)) {
    SerialMon.println("Connected to server");
    client.print(String("GET / HTTP/1.1\r\n") +
                 "Host: example.com\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 10000) {
      while (client.available()) {
        String line = client.readStringUntil('\n');
        SerialMon.println(line);
        timeout = millis();
      }
    }
    client.stop();
    SerialMon.println("HTTP GET successful");
  } else {
    SerialMon.println("HTTP GET failed, error: connection refused");
  }
}

void loop() {
  // reste vide pour un teste
}