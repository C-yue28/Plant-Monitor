/*

Firmware for Plant Monitor HackClub Stasis Project

*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <U8x8lib.h>

// mode: normal (0) or data collection (1) (sample more often to gather data for ML model)

const int MODE = 0;
const double K;
const double a;
const double b;
const double c;

//GPIO pins:

const int AIN1 = 10;
const int AIN2 = 11;
const int SLEEP = 23;

const int BME280_SDA = 20;
const int BME280_SCL = 21;

const int OLED_SDA = 24;
const int OLED_SCL = 25;

const int SOIL_SIG = 29;

const int BUTTON = 19;

// data readings

double soil_moisture = 0;
double SOIL_MOISTURE_THRESHOLD = 0.40; // arbitrary value found online (40%)
const double M_param = pow(SOIL_MOISTURE_THRESHOLD, 1-a);

Adafruit_BME280 bme280;
double temperature;
double humidity;
double pressure;

U8X8_SSD1315_128X64_NONAME_HW_I2C oled(U8X8_PIN_NONE, OLED_SCL, OLED_SDA);
bool OLED_ON = 0;
int OLED_ON_MILLIS = 0;

// time
int timeStep = 1800000; // ms; 30 minutes

struct days_hours {
  int days;
  int hours;
}

void readData() {

  // sample each sensor multiple times; take an average
  sum_moisture = 0;
  sum_temp = 0;
  sum_hum = 0;
  sum_pressure = 0;

  for (int i = 0; i < SAMPLE_COUNT; i++) {

    // for now, I cannot know what values this will get - it is a relative measurement; as moisture goes up analog voltage goes down 
    sum_moisture += analogRead(SOIL_SIG) / 3.3; // for now I will simply take it as a percentage of its max; I will definitely have to adjust this

    sum_temp += bme280.readTemperature();
    sum_hum += bme280.readHumidity();
    sum_pressure += bme280.readPressure() / 1000.0F;

  }

  soil_moisture = sum_moisture / SAMPLE_COUNT;
  temperature = sum_temp / SAMPLE_COUNT;
  humidity = sum_hum / SAMPLE_COUNT;
  pressure = sum_pressure / SAMPLE_COUNT;

}

days_hours predictTime() {
  // integrated the rate calculation function using the assumption that (1+b*T)(1-c*H) would remain relatively constant enough for me to treat it as such
  // this was done with the help of many online sources and ChatGPT
  time_until_next_water = (M_param-pow(soil_moisture, 1-a)) / (k*(1+b*temperature)*(1-c*humidity)*(1-a))
}

void toggleDisplay() {

  if (OLED_ON) {
    OLED_ON = false;
    oled.clearDisplay();
    return;
  }

  time_until_next_water = predictTime();

  oled.drawString(0, 0, "Moisture: %.2f", soil_moisture);
  oled.drawString(0, 1, "Temp(C): %.2f", temperature);
  oled.drawString(0, 2, "Humidity: %.2f%%", humidity);
  oled.drawString(0, 3, "Pressure: %.1f atm", pressure/760.0F);
  if (!MODE) {
    oled.drawString(0, 4, "Time until next")
    oled.drawString(0, 5, "water: %d:%d", time_until_next_water.days, time_until_next_water.hours);
  }

  OLED_ON = true;
  delay(50); // debounce button

}

void outputData(bool hasWatered) {
  Serial.println("SOIL: %f", soil_moisture);
  Serial.println("TEMPERATURE: %f", temperature);
  Serial.println("HUMIDITY: %f", humidity);
  Serial.println("WATERED: %d", hasWatered ? 1 : 0);
  Serial.println("MILLIS: %f", millis()); // for calculating evaporation rate
}

void waterPlant() {
  analogWrite(AIN1, MOTOR_SPEED);
  delay(5000);
  analogWrite(AIN1, 0);
}

void setup() {

  // initialize I2C buses
  Wire.setSDA(BME280_SDA);
  Wire.setSCL(BME280_SCL);

  // Wire1.setSDA(OLED_SDA);
  // Wire1.setSCL(OLED_SCL);
  
  oled.begin();
  bme280.begin(0x76, &Wire);
  Serial.begin(115200);

  // pin modes
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(SLEEP, OUTPUT);
  pinMode(SOIL_SIG, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON), toggleDisplay, LOW);
  digitalWrite(SLEEP, HIGH);

  oled.setFont(u8x8_font_8x13_1x2_f);
  if (MODE) {
    timeStep /= 6; // 30 minutes down to 5 minutes
  }

  Serial.println("Ready.");  

}

void loop() {

  readData();

  if (soil_moisture < SOIL_MOISTURE_THRESHOLD) {
    waterPlant();
    outputData(true);
  } else {
    outputData(false);
  }

  delay(timeStep);

}
