/**
 * Sketch for reading healt data and sending it to a REST API with an Arduino Yun.
 * 
 * Note, that you need a python script as counter part on the Linux part of the Arduino Yun, 
 * which takes the values from the bridge and executes the actual HTTP requests.
 * 
 * Created by: Tobias Wochinger
 * Created: 01 Feb 2017
 * 
 * https://github.com/wochinge/YunBitalino
 * 
 */

#include <LinkedList.h>
#include <Bridge.h>
#include <Process.h>
#include "Filter.h"

#define DEBUG false

#define PIN_OF_ECG 0
#define ECG_THRESHOLD (-100)
#define MIN_HEART_BEAT_INTERVAL 200

#define PIN_OF_RESPIRATION 1
#define RESPIRATION_THRESHOLD 400
#define MIN_BREATH_INTERVAL 400

#define PIN_OF_TEMPERATURE 2
#define CALCULATE_TEMPERATURE(analogInput) ((5.0 * analogInput * 100.0) / 1024);

#define MINUTE_IN_MS 60000.0
#define TIME_WINDOW_IN_MS 10000

#define ERROR_PIN 2
char errorBuffer[4];

#define PRINT_INTERVALL_IN_MS 3000

#define FIRST(list) (list->get(0))
#define LAST(list) (list->get(list->size() - 1))

#define INTERVAL_BIG_ENOUGH(list, minInterval) (timeOfMeasurement - LAST(list) >= minInterval)

LinkedList<unsigned long> ecgPeaks;

LinkedList<unsigned long> breathPeaks;
boolean stillInOldBreathPeak = true;

// Filters to clear input
ECGFilter ecgfilter;
RespirationFilter breathFilter;

Process mqtt;
unsigned long timeOfMeasurement;
unsigned long lastPrint;

int ecgValue;
int breathValue;
float temp;

void setup() {
  Serial.begin(57600);
  if (!DEBUG) {
    Bridge.begin();
    mqtt.runShellCommandAsynchronously("python /root/yun_client.py");
    pinMode(ERROR_PIN, OUTPUT); 
  }
}

void loop() {
  timeOfMeasurement = millis();
  
  ecgValue = analogRead(PIN_OF_ECG);
  ecgValue = ecgfilter.step(ecgValue);

  breathValue = analogRead(PIN_OF_RESPIRATION);
  breathValue = breathFilter.step(breathValue);
  
  if (DEBUG) {
    Serial.print(timeOfMeasurement);
    Serial.print(": ");
    Serial.println(ecgValue);   
  } else {
    checkECGPeak(ecgValue);
    checkBreathPeak(breathValue);
    
    // Print bpm in regular intervalls
    if (timeOfMeasurement - lastPrint > PRINT_INTERVALL_IN_MS) {
      checkError();
      lastPrint = timeOfMeasurement;
      int current = peaksPerMinute(&ecgPeaks);
      Serial.print("BPM: ");
      Serial.println(current);
      Bridge.put("bpm", String(current));

      current = peaksPerMinute(&breathPeaks);
      current = current >= 300 ? -1 : current;
      Serial.print("Breath: ");
      Serial.println(current);
      Bridge.put("respiration", String(current));

      calculateTemperature();
      Serial.print("Temp: ");
      Serial.println(temp);
      Bridge.put("temp", String(temp));
    }
  }
}

void checkECGPeak(int value) {
   // Check whether it is a peak?
    if (value <= ECG_THRESHOLD && INTERVAL_BIG_ENOUGH((&ecgPeaks), MIN_HEART_BEAT_INTERVAL)) {
      refreshList(&ecgPeaks, value);
    } 
}

void checkBreathPeak(int value) {
  if (value > RESPIRATION_THRESHOLD && INTERVAL_BIG_ENOUGH((&breathPeaks), MIN_BREATH_INTERVAL) && !stillInOldBreathPeak) {
    stillInOldBreathPeak = true;
    refreshList(&breathPeaks, value);
  } else if (value < 350) {
    stillInOldBreathPeak = false;
  }
}

/**
 * 
 */
void refreshList(LinkedList<unsigned long>* list, int value) {
  list->add(timeOfMeasurement);
  removeTooOldPeaks(list);
}

void removeTooOldPeaks(LinkedList<unsigned long>* list) {
  while (list->size() > 0 && (timeOfMeasurement - (list->get(0))) >= TIME_WINDOW_IN_MS) {
    list->shift();
  }
}

int peaksPerMinute(LinkedList<unsigned long>* list) {
  if (list->size() > 1) {
      return list->size() * (MINUTE_IN_MS / (timeOfMeasurement - FIRST(list)));
  } else {
    return 0;
  }
}

void calculateTemperature() {
  temp = 0;
  for (int i = 0; i < 5; ++i) {
    temp += CALCULATE_TEMPERATURE(analogRead(PIN_OF_TEMPERATURE));
  }
  temp /= 5;
}

void checkError() {
  memset(errorBuffer, 0, 4);
  Bridge.get("response_status", errorBuffer, 4);
  Bridge.put("response_status", "1");
  if (errorBuffer[0] != '2') {
    digitalWrite(ERROR_PIN, HIGH);
  } else {
    digitalWrite(ERROR_PIN, LOW);
  }
  Serial.println(errorBuffer);
}





