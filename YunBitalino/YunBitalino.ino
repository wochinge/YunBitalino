#include <LinkedList.h>
#include <Bridge.h>
#include <Process.h>
#include "Bandpass.h"

#define DEBUG false

#define PIN_OF_ECG 0
#define ECG_THRESHOLD 130

#define PIN_OF_RESPIRATION 1
#define RESPIRATION_THRESHOLD 200

#define MINUTE_IN_MS 60000.0
#define TIME_WINDOW_IN_MS 10000

#define PRINT_INTERVALL_IN_MS 5000

#define FIRST(list) (list->get(0))
#define LAST(list) (list->get(list->size() - 1))

LinkedList<unsigned long> ecgPeaks;
LinkedList<unsigned long> breathPeaks;

BandpassFilter filter;

RespirationFilter respFilter;

Process mqtt;
unsigned long now;
unsigned long lastPrint;
int ecgValue;
int breathValue;

void setup() {
  Serial.begin(57600);
  //Bridge.begin();
 // mqtt.runShellCommandAsynchronously("python /root/yun_client.py");
}

void loop() {
  
  now = millis(); // Here or one line down?
  
  ecgValue = analogRead(PIN_OF_ECG);
  ecgValue = filter.step(ecgValue);

  breathValue = analogRead(PIN_OF_RESPIRATION);
  
  //ecgValue = (ecgValue / 1024.0 - 0.5) * 100;
  
  // Apply filter
  //ecgValue = respFilter.step(ecgValue);

  if (DEBUG) {
    Serial.print(now);
    Serial.print(": ");
    Serial.println(ecgValue);
    
  } else {
    checkPeak(ecgValue, &ecgPeaks, ECG_THRESHOLD);
    //checkPeak(breathValue, &breathPeaks, RESPIRATION_THRESHOLD);
    
    // Print bpm in regular intervalls
    if (now - lastPrint > PRINT_INTERVALL_IN_MS) {
      lastPrint = now;
      int current = peaksPerMinute(&ecgPeaks);
      Serial.print("BPM: ");
      Serial.println(current);
      Bridge.put("bpm", String(current));

//      current = peaksPerMinute(&breathPeaks);
//      Serial.print("Breath: ");
//      Serial.println(current);
//      Bridge.put("respiration", String(current));
    }
  }
}

void checkPeak(int value, LinkedList<unsigned long>* list, int threshold) {
   // Check whether it is a peak?
    if (value >= threshold && now - LAST(list) >= 200) {
      list->add(now);
      // Remove peaks which are not in the time windows anymore
      removeTooOldPeaks(list);
    }
}

void removeTooOldPeaks(LinkedList<unsigned long>* list) {
  while (list->size() > 0 && now - list->get(0) > TIME_WINDOW_IN_MS) {
    list->shift();
  }
}

int peaksPerMinute(LinkedList<unsigned long>* list) {
  if (list->size() > 1) {
      return list->size() * (MINUTE_IN_MS / (LAST(list) - FIRST(list)));
  } else {
    return -1;
  }
}

