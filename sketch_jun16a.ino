#include <DHT.h>
#include <LiquidCrystal.h>  

#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

 
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const int PIN_LDR = A0;
void setup() {
  // put your setup code here, to run once:

const int PIN_WATER = A1;
const int PIN_SON = A2;
const int PIN_BUZZER = 4;

 
const int SEUIL_EAU = 300;
const float SEUIL_HUMIDITE_AIR = 80.0;
const float SEUIL_TEMP = 34.0;
const int SEUIL_OBSCURITE = 200;
const int SEUIL_BRUIT = 50;

 
const int CONFIRMATIONS_REQUISES = 3;
int compteurEau = 0;
int compteurClimat = 0;
int compteurNuisible = 0;

 
void afficherEtatLCD(int typeAlerte, float tempAir, float humiditeAir);
void gererBuzzer(int typeAlerte);
void envoyerRapportSerie(float tempAir, float humiditeAir, int humiditeSol, int bruitStock, int typeAlerte);

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  
  lcd.begin(16, 2); 
  
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  
 
  lcd.setCursor(0, 0);
  lcd.print("SYSTEME KEBA");
  lcd.setCursor(0, 1);
  lcd.print("INITIALISATION..");
  delay(2000);
  lcd.clear();
}

void loop() {
  int humiditeSol = analogRead(PIN_WATER);
  int bruitStock = analogRead(PIN_SON);
  int expositionSoleil = analogRead(PIN_LDR);
  float humiditeAir = dht.readHumidity();
  float tempAir = dht.readTemperature();
  
  
  if (isnan(humiditeAir) || isnan(tempAir)) {
    lcd.setCursor(0, 0);
    lcd.print("Erreur capteur ");
    lcd.setCursor(0, 1);
    lcd.print("DHT11 - Reset...");
    Serial.println("Erreur de lecture DHT11, cycle ignore.");
    delay(1500);
    return;
  }
  
   
  bool eauDetectee = (humiditeSol > SEUIL_EAU);
  bool climatMauvais = (humiditeAir > SEUIL_HUMIDITE_AIR && tempAir > SEUIL_TEMP);
  bool nuisibleSuspect = (expositionSoleil < SEUIL_OBSCURITE && bruitStock > SEUIL_BRUIT);
  
 
  compteurEau = eauDetectee ? compteurEau + 1 : 0;
  compteurClimat = climatMauvais ? compteurClimat + 1 : 0;
  compteurNuisible = nuisibleSuspect ? compteurNuisible + 1 : 0;
  
   
  int typeAlerte = 0;
  if (compteurEau >= CONFIRMATIONS_REQUISES) {
    typeAlerte = 3;
  } else if (compteurClimat >= CONFIRMATIONS_REQUISES) {
    typeAlerte = 2;
  } else if (compteurNuisible >= CONFIRMATIONS_REQUISES) {
    typeAlerte = 1;
  }
  
 
  afficherEtatLCD(typeAlerte, tempAir, humiditeAir);
  envoyerRapportSerie(tempAir, humiditeAir, humiditeSol, bruitStock, typeAlerte);
  gererBuzzer(typeAlerte);
}

void afficherEtatLCD(int typeAlerte, float tempAir, float humiditeAir) {
  lcd.clear();
  switch (typeAlerte) {
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("EAU DETECTEE ! ");
      lcd.setCursor(0, 1);
      lcd.print("BOUGER LE STOCK");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("ALERTE CLIMAT ! ");
      lcd.setCursor(0, 1);
      lcd.print("OUVRIR LE STOCK");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("NUISIBLE DETECTE");
      lcd.setCursor(0, 1);
      lcd.print("DANS LE STOCK");
      break;
    default:
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print((int)tempAir);
      lcd.print("C H:");
      lcd.print((int)humiditeAir);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("STOCK: SEC / RAS");
      break;
  }
}

void gererBuzzer(int typeAlerte) {
  switch (typeAlerte) {
    case 3:  
      tone(PIN_BUZZER, 1000);
      delay(1500);
      break;
    case 2:  
      tone(PIN_BUZZER, 2000, 200); delay(400);
      tone(PIN_BUZZER, 2000, 200); delay(400);
      tone(PIN_BUZZER, 2000, 200); delay(500);
      break;
    case 1:  
      tone(PIN_BUZZER, 500, 100);
      delay(1400);
      break;
    default:  
      noTone(PIN_BUZZER);
      delay(1500);  
      break;
  } 
}

void envoyerRapportSerie(float tempAir, float humiditeAir, int humiditeSol, int bruitStock, int typeAlerte) {
  Serial.println("--- RAPPORT KEBA ---");
  Serial.print("Temp: "); Serial.print(tempAir); Serial.print("C | Hum: ");
  Serial.print(humiditeAir); Serial.println("%");
  Serial.print("Sol/Eau: "); Serial.println(humiditeSol);
  Serial.print("Bruit: "); Serial.println(bruitStock);
  Serial.print("Type alerte: "); Serial.println(typeAlerte);
  Serial.println("--------------------");
}
