/* YourDuino Electronic Brick Test
Temperature Sensor DS18B20
- Connect cable to Arduino Digital I/O Pin 2
terry@yourduino.com */

/*-----( Import needed libraries )-----*/
// Get 1-wire Library here: http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>
//Get DallasTemperature Library here:  http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>

/*-----( Declare Constants )-----*/
#define Freezer_OW_Sensor 6 /*-(Connect to Pin 3 )-*/
#define Ferm_OW_Sensor 5
#define RELAY_ON 1
#define RELAY_OFF 0

/*-----( Declare objects )-----*/
/* Set up a oneWire instance to communicate with any OneWire device*/
OneWire FreezerWire(Freezer_OW_Sensor);
OneWire FermWire(Ferm_OW_Sensor);

/* Tell Dallas Temperature Library to use oneWire Library */
DallasTemperature FreezerSensors(&FreezerWire);
DallasTemperature FermSensors(&FermWire);


/*-----( Declare Variables )-----*/
#define FreezerRelay  2  // Arduino Digital I/O pin number
#define FermRelay  4
#define FermOffSwitch 7
#define FermTempSwitch 8

int currentTempFreezer;
int targetFreezerTemp = 40;
int currentTempFerm;
int TargetFermTemp = 60;

int avgTemp[5];
bool validFreezerTemp = false;
int invalidFermCount = 0;
int invalidFreezerCount = 0;
bool validFermTemp = false;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  //-------( Initialize Pins so relays are inactive at reset)----

  digitalWrite(FreezerRelay, RELAY_OFF);
  digitalWrite(FermRelay, RELAY_OFF);
  pinMode(FermOffSwitch, INPUT);
  digitalWrite(FermOffSwitch, HIGH);
  pinMode(FermTempSwitch, INPUT);
  digitalWrite(FermTempSwitch, HIGH);

  //---( THEN set pins as outputs )----
  pinMode(FreezerRelay, OUTPUT);
  pinMode(FermRelay, OUTPUT);

  /*-(start serial port to see results )-*/
  delay(1000);
  Serial.begin(9600);

  /*-( Start up the DallasTemperature library )-*/
  FreezerSensors.begin();
  FermSensors.begin();

}//--(end setup )---

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

  delay(1000);
  getFreezerTemp();

  Serial.println("The target Freezer temperature is ");
  Serial.println(targetFreezerTemp);
  Serial.println();

  if (currentTempFreezer > targetFreezerTemp)
  {
    Serial.println("Turning freezer relay on.");
    digitalWrite(FreezerRelay, RELAY_ON);
  }
  if (currentTempFreezer < targetFreezerTemp)
  {
    Serial.println("Turning freezer relay off.");
    digitalWrite(FreezerRelay, RELAY_OFF);
  }



  if (digitalRead(FermOffSwitch))
  {
    getFermTemp();

    TargetFermTemp = digitalRead(FermTempSwitch) ? 60 : 40;

    Serial.println("The target Ferm temperature is ");
    Serial.println(TargetFermTemp);
    Serial.println();
    Serial.println("The Ferm switch is on");
    if (currentTempFerm > TargetFermTemp)
    {
      Serial.println("Turning Ferm relay on.");
      digitalWrite(FermRelay, RELAY_ON);
    }
    if (currentTempFerm < TargetFermTemp)
    {
      Serial.println("Turning Ferm relay off.");
      digitalWrite(FermRelay, RELAY_OFF);
    }
  }
  else
  {
    Serial.println("Fermentor is off");
    digitalWrite(FermRelay, RELAY_OFF);
  }

  delay(3000);
}/* --(end main loop )-- */

/* ( THE END ) */

//Read temperature sensor values and write value to currentTempFreezer
void getFreezerTemp(void) {
  //Counter for numver of invalid temperature reading
  validFreezerTemp = false;

  invalidFreezerCount = 0;
  while (!validFreezerTemp) {
    if (invalidFreezerCount >= 5)
    {
      failSafe();
    }
    Serial.println();
    Serial.print("Requesting Freezer temperature...");
    FreezerSensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    currentTempFreezer = FreezerSensors.getTempFByIndex(0);
    Serial.print(currentTempFreezer);
    Serial.println(" Degrees F");
    Serial.println();

    if (currentTempFreezer > 0 & currentTempFreezer < 110) {
      validFreezerTemp = true;
    }
    else {
      invalidFreezerCount++;
    }

    delay(1000);
  }

}
void getFermTemp(void) {
  //Counter for numver of invalid temperature reading
  validFermTemp = false;
  invalidFermCount = 0;

  while (!validFermTemp) {
    if (invalidFermCount >= 5)
    {
      failSafe();
    }
    Serial.println();
    Serial.print("Requesting Ferm temperature...");
    FermSensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    currentTempFerm = FermSensors.getTempFByIndex(0);
    Serial.print(currentTempFerm);
    Serial.println(" Degrees F");
    Serial.println();

    if (currentTempFerm > 0 & currentTempFerm < 110) {
      validFermTemp = true;
    }
    else {
      invalidFermCount++;
    }
    delay(1000);
  }
}

void failSafe(void) {
  Serial.println("Temperature sensor failure detected, turning off compressor...");
  digitalWrite(FreezerRelay, RELAY_OFF);
  while (true);
}
