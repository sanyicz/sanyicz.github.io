/* Display: Android phone, VectorDisplay app, through Bluetooth connection
 * Time: RTC module
 * Temp in, Temp out, RH in measurement
 * Distance measurement, with buzzer
 */

//app state: environment (1), proximity (2) or plot (3)
int appState = 1;

//time
#include <Wire.h> //library for I2C communication, also included later
#include "RTClib.h" //library for RTC use
RTC_DS3231 rtc; //create an RTC_DS3231 object called 'rtc'
DateTime now; //create a variable for time values
int oldMinute = -1; int newMinute;
int oldHour = -1; int newHour;

//temperature
#include <OneWire.h>
#include <DallasTemperature.h>
const int tinPin = 3; //set a digital pin for the internal temperature sensor
OneWire temp_in(tinPin);
DallasTemperature sensor_temp_in(&temp_in);
float T_in = -273; //create a variable for internal temperature values
const int toutPin = 5; //set a digital pin for the outer temperature sensor
OneWire temp_out(toutPin);
DallasTemperature sensor_temp_out(&temp_out);
float T_out = -273; //create a variable for outer temperature values

//relative humidity
#include <DHT.h> //include the necessary library
const int dhtPin = 6; //set a digital pin for the humidity sensor
#define DHT_TYPE DHT11
DHT dht(dhtPin, DHT_TYPE);
float RH = -1; //create a variable for humidity values

//distance
const int trigPin = 8;
const int echoPin = 7;
long duration; int oldDistance; int newDistance;
int distAnimLimit = 100; //upper distance limit for drawing animation (in cm)
bool animState = false; bool prevAnimState = false;

//buzzer
const int buzzerPin = 4; //set a digital pin for the buzzer
int frequency; int buzzDuration = 200;

//bluetooth connection
#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(0, 1);// RX & TX pins for Arduino Nano

//android display
#include <VectorDisplay.h>
SerialDisplayClass Display(MyBlue);
VectorDisplayMessage msg;
int xsize = 480; int ysize = 240;
int16_t frame[4] = {20, 30, xsize - 40, ysize - 40}; //from upper left corner: x0, y0, xsize, ysize
int pixelToCm = frame[3] / distAnimLimit; //frame's xsize / distAnimLimit
int pixelToLine = 30;
int width = xsize - 40; //the width of the car (and the animation) in pixels
uint16_t color = 0xFFFF; //white

//plotting
bool plotState = false;
const int nMax = 12; //n = xmax
float tempInValues[nMax]; //T_values[xmax]
int saveDelta = 5; int lastSave;
int index = 1;
int xo = 40; int yo = 210; //origin of the coordinate system
int lx = 400; int ly = 200; //coordinate system x and y size
int arrowsize = 10; //size of the arrow (vertical and horizontal length in pixels)
float xmin = 1; float xmax = nMax; float ymin = 22; float ymax = 28;
float xPixelToValue = (xmax - xmin) / lx; float yPixelToValue = (ymax - ymin) / ly;

//function prototypes (why is it needed here???)
/*float measure_distance();
void print_time(int line, DateTime t);
void print_data(int line, String name, float value, String unit);*/


void setup() {
  Wire.begin(); //start the I2C interface
  MyBlue.begin(9600); //default communication rate of the HC-06 Bluetooth module
  sensor_temp_in.begin(); sensor_temp_out.begin();
  dht.begin();
  rtc.begin();
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT); //set UH pins
  pinMode(buzzerPin, OUTPUT); //set buzzer as output
  Display.begin(); Display.coordinates(xsize, ysize); Display.setTextSize(3);
  Display.text(0, 0, "Starting...");
  oldDistance = measure_distance();
  now = rtc.now(); oldHour = now.hour();
  sensor_temp_in.requestTemperatures(); T_in = sensor_temp_in.getTempCByIndex(0); //measure inside temperature
  sensor_temp_out.requestTemperatures(); T_out = sensor_temp_out.getTempCByIndex(0); //measure outside temperature
  RH = dht.readHumidity(); //measure inside relative humidity
  lastSave = now.minute()-1;
  tempInValues[0] = T_in; index++;
  Display.clear();
  Display.addButton('1', "Environment");
  Display.addButton('2', "Proximity");
  Display.addButton('3', "Plot");
}

void loop() {
  if (Display.readMessage(&msg) and msg.what == MESSAGE_BUTTON) {
    if (msg.data.button == '1') { appState = 1; }
    else if (msg.data.button == '2') { appState = 2; }
    else if (msg.data.button == '3') { appState = 3; }
  }
  
  if (appState == 1) { //environment
    plotState = false; Display.setTextSize(3);
    newDistance = measure_distance(); //always measure distance first
    now = rtc.now(); //read time from RTC
    newMinute = now.minute();
    newHour = now.hour();
    if (oldDistance > distAnimLimit && newDistance <= distAnimLimit) {animState = true;} //something got too close, trigger animation
    else if (oldDistance <= distAnimLimit && newDistance > distAnimLimit) {animState = false;} //something got far enough, no animation
    
    if (animState == true) {
      appState = 2; //set the app to Proximity mode
    }
    else if (newMinute != oldMinute && animState == false) {
      //nothing is too close and a minute passed
      //measure all environmental parameters and update everything on the display
      sensor_temp_in.requestTemperatures();
      T_in = sensor_temp_in.getTempCByIndex(0); //measure inside temperature
      sensor_temp_out.requestTemperatures();
      T_out = sensor_temp_out.getTempCByIndex(0); //measure outside temperature
      RH = dht.readHumidity(); //measure inside relative humidity
      Display.clear();
      print_time(0, now); //print time in line 0
      print_data(1, "Temp in", T_in, "°C"); //print inside temperature in line 1
      print_data(2, "Temp out", T_out, "°C"); //print outside temperature in line 2
      print_data(3, "Humidity in", RH, "%"); //print inside relative humidity in line 3
      print_data(4, "Distance", newDistance, "cm"); //print distance in line 4
      if ( (newMinute - lastSave) % saveDelta == 0) {
        lastSave = newMinute;
        if (index <= nMax) {
          tempInValues[index-1] = T_in;
          index++;
        }
        else { //index = nMax + 1, in this case 13
          for (int i=0; i<index-2; i++) {
            tempInValues[i] = tempInValues[i+1];
          }
          tempInValues[index-2] = T_in;
        }
      }
    }
    else if (prevAnimState == true && animState == false) {
      prevAnimState = false;
      Display.clear();
      print_time(0, now); //print time in line 0
      print_data(1, "Temp in", T_in, "°C"); //print inside temperature in line 1
      print_data(2, "Temp out", T_out, "°C"); //print outside temperature in line 2
      print_data(3, "Humidity in", RH, "%"); //print inside relative humidity in line 3
      print_data(4, "Distance", newDistance, "cm"); //print distance in line 4
      }
    else {
      //do nothing, if exiting proximity mode works properly
      //else find another way: if prox->env happened (some variable for this), show last data, else do nothing
      /*Display.clear();
      print_time(0, now); //print time in line 0
      print_data(1, "Temp in", T_in, "°C"); //print inside temperature in line 1
      print_data(2, "Temp out", T_out, "°C"); //print outside temperature in line 2
      print_data(3, "Humidity in", RH, "%"); //print inside relative humidity in line 3
      print_data(4, "Distance", newDistance, "cm"); //print distance in line 4*/
    }
    oldDistance = newDistance;
    oldMinute = newMinute;
    delay(250);
  }
  
  else if (appState == 2) { //proximity
    prevAnimState = true;
    plotState = false; Display.setTextSize(3);
    newDistance = measure_distance();
    if (oldDistance <= distAnimLimit && newDistance > distAnimLimit) {
      //if something got far enough, no animation
      //set the app to Environment mode
      Display.clear();
      print_time(0, now); //print time in line 0
      print_data(1, "Temp in", T_in, "°C"); //print inside temperature in line 1
      print_data(2, "Temp out", T_out, "°C"); //print outside temperature in line 2
      print_data(3, "Humidity in", RH, "%"); //print inside relative humidity in line 3
      print_data(4, "Distance", newDistance, "cm"); //print distance in line 4
      appState = 1;
      animState = false;
    }
    else {
      Display.clear();
      print_data(0, "Distance", newDistance, "cm"); //print distance in line 0
      if (oldDistance > distAnimLimit && newDistance <= distAnimLimit) {animState = true;}
      if (animState == true) {
        draw_animation(frame, width, color, newDistance);
        frequency = frequencyFunction(distAnimLimit, newDistance);
        tone(buzzerPin, frequency, buzzDuration);
      }
      delay(250);
    }
    oldDistance = newDistance;
  }

  else if (appState == 3) { //plot
    prevAnimState = true; animState = false;
    Display.setTextSize(1);
    if (plotState == false) {
      Display.clear();
      if (index <= nMax) { //index-1? because index is incremented after appending the array, so index is bigger by 1 than the array's length
        ymin = int(arrayMin(tempInValues, index-1)); ymax = int(arrayMax(tempInValues, index-1) + 1); yPixelToValue = (ymax - ymin) / ly;
        drawCoordinateSystem(xo, yo, lx, ly, arrowsize, xmin, xmax, ymin, ymax);
        plot(tempInValues, index-1);
       }
      else {
        ymin = int(arrayMin(tempInValues, index-1)); ymax = int(arrayMax(tempInValues, index-1) + 1); yPixelToValue = (ymax - ymin) / ly;
        drawCoordinateSystem(xo, yo, lx, ly, arrowsize, xmin, xmax, ymin, ymax);
        plot(tempInValues, index-1);
      }
      plotState = true;
    }
    else { delay(250); }
  }

}


//function for printing time to Android display
void print_time(int line, DateTime t) {
  String timeString; String monthString; String dayString; String hourString; String minuteString;
  if (t.month() < 10) { monthString = "0" + String(t.month()); } else { monthString = String(t.month());}
  if (t.day() < 10) { dayString = "0" + String(t.day()); } else { dayString = String(t.day());}
  if (t.hour() < 10) { hourString = "0" + String(t.hour()); } else { hourString = String(t.hour());}
  if (t.minute() < 10) { minuteString = "0" + String(t.minute()); } else { minuteString = String(t.minute());}
  timeString = String(t.year()) + "-" + monthString + "-" + dayString + " " + hourString + ":" + minuteString;
  Display.text(0, line*pixelToLine, timeString);
}

//function for printing a measured data to Android display
void print_data(int line, String name, float value, String unit) {
  String dataString;
  if (name == "Distance"){
    if (value >= 100) {
      value = value / 100;
      unit = "m";
      if (value >= 3) { dataString = name + ": >3" + unit; }
      else {dataString = name + ": " + String(value, 1) + " " + unit;}
    }
    else {dataString = name + ": " + String(value, 1) + " " + unit;}
  }
  else { dataString = name + ": " + String(value, 1) + " " + unit; }
  Display.text(0, line*pixelToLine, dataString);
}

//function for distance measurement
float measure_distance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2); //write LOW to trigger pin
  digitalWrite(trigPin, HIGH); delayMicroseconds(10); digitalWrite(trigPin, LOW); //send signal
  duration = pulseIn(echoPin, HIGH); //returns the sound wave travel time in microseconds
  float distance = (duration-10) * 0.0343 / 2; //calculates distance in cm
  return distance;
}

float frequencyFunction(float distAnimLimit, float distance) {
  int minFreq = 5000;
  int maxFreq = 17000;
  //step function
  int steps = 4;
  int freqStep = (maxFreq - minFreq) / steps; //= 12000/4 = 3000
  int i = (distAnimLimit - distance) / (distAnimLimit / steps ); //= (distAnimLimit - distance)/15
  int frequency = minFreq + i * freqStep;
  return frequency;
}

//function for drawing the car on the display
void draw_car(int16_t frame[], int16_t car_x0, int16_t car_y0, int16_t width, uint16_t color) {
  Display.drawLine(frame[0] + car_x0, frame[1], frame[0] + car_x0, frame[1] + car_y0, color); //left corner
  Display.drawLine(frame[0] + car_x0, frame[1] + car_y0, frame[0] + car_x0 + width, frame[1] + car_y0, color); //bottom line
  Display.drawLine(frame[0] + car_x0 + width, frame[1], frame[0] + car_x0 + width, frame[1] + car_y0, color); //right corner
  Display.drawLine(frame[0] + car_x0 + width/2, frame[1] + car_y0, frame[0] + car_x0 + width/2, frame[1] + car_y0 + 8*pixelToCm, color); //drawbar
}

void draw_wall(int16_t frame[], int16_t wall_x0, int16_t wall_y0, int16_t width, uint16_t color) {
  Display.drawLine(frame[0] + wall_x0, frame[1] + wall_y0, frame[0] + wall_x0 + width, frame[1] + wall_y0, color); //wall
}

void draw_animation(int16_t frame[], int16_t width, uint16_t color, int distance) {
  int wall_x0 = frame[2] / 2 - width / 2; //x coordinate in the frame
  int wall_y0 = frame[3]; //y coordinate in the frame
  draw_wall(frame, wall_x0, wall_y0, width, color); //draw the wall
  int car_x0 = wall_x0; //x coordinate in the frame
  int car_y0 = wall_y0 - distance * pixelToCm; //y coordinate in the frame, pixelToCm pixel = 1 cm
  draw_car(frame, car_x0, car_y0, width, color); //draw the car
}

float arrayMax(float values[], int n) {
  float maxx = values[0];
  for (byte i=1; i<n; i++){
    if (values[i] > maxx) { maxx = values[i]; }
  }
  return maxx;
}

float arrayMin(float values[], int n) {
  float minn = values[0];
  for (byte i=1; i<n; i++){
    if (values[i] < minn) { minn = values[i]; }
  }
  return minn;
}

void drawCoordinateSystem(int xo, int yo, int lx, int ly, int arrowsize, float xmin, float xmax, float ymin, float ymax) {
  //display size: xsize = 480, ysize = 240
  int xn = 10; float xdiv = (xmax - xmin) / (xn - 1); float xdivp = xdiv / xPixelToValue;
  int yn = 4; float ydiv = (ymax - ymin) / yn; float ydivp = ydiv / yPixelToValue;
  
  Display.drawFastHLine(xo, yo, lx, color); //horizontal axis
  for (int xi=1; xi<xn-1; xi++) {
    Display.drawLine(xo+xi*xdivp, yo-arrowsize, xo+xi*xdivp, yo+arrowsize, color);
  }
  Display.drawLine(xo+lx-arrowsize, yo-arrowsize, xo+lx, yo, color); //horizontal arrow, upper leg
  Display.drawLine(xo+lx-arrowsize, yo+arrowsize, xo+lx, yo, color); //horizontal arrow, lower leg
  Display.text(xo-14, yo+arrowsize+4, String(xmin));
  Display.text(xo+lx-14, yo+arrowsize+4, String(xmax));
  
  Display.drawFastVLine(xo, yo-ly, ly, color); //vertical axis
  for (int yi=1; yi<yn; yi++) {
    Display.drawLine(xo-arrowsize, yo-yi*ydivp, xo+arrowsize, yo-yi*ydivp, color);
  }
  Display.drawLine(xo-arrowsize, yo-ly+arrowsize, xo, yo-ly, color); //vertival arrow, left leg
  Display.drawLine(xo+arrowsize, yo-ly+arrowsize, xo, yo-ly, color); //vertical arrow, right leg
  Display.text(xo-28, yo-4, String(ymin));
  Display.text(xo-28, yo-ly-4, String(ymax));
}

void drawPoint(float x, float y) {
  float xp; float yp;
  xp = (x - xmin) / xPixelToValue; yp = (y - ymin) / yPixelToValue;
  Display.fillCircle(xo+xp, yo-yp, 2, color);
}

void plot(float values[], int n) {
  for (byte i=0; i<n; i++) { drawPoint(i+1, values[i]); }
}
