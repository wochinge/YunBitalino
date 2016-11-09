#include <LinkedList.h>
#include <Bridge.h>
#include "Bandpass.h"
#include <Process.h>

#define PIN_OF_ECG 0
#define MAX_BPM 200
#define THRESHOLD_FOR_PEAK 190

#define MINUTE_IN_MS 60000.0
#define TIME_WINDOW_IN_MS 10000

#define PRINT_INTERVALL_IN_MS 5000

#define FIRST(list) (list.get(0))
#define LAST(list) (list.get(list.size() - 1))

LinkedList<unsigned long> peaks;
BandpassFilter filter;

Process mqtt;
unsigned long now;
unsigned long lastPrint;
int ecgValue;

void setup() {
  Serial.begin(57600);
  Bridge.begin();
  //mqtt.runShellCommandAsynchronously("python /root/yun_mqtt.py");
}

void loop() {
  
  now = millis(); // Here or one line down?
  
  // Read voltage
  ecgValue = analogRead(PIN_OF_ECG);
  // Apply filter
  ecgValue = filter.step(ecgValue);

  // Check whether it is a peak?
  if (ecgValue >= THRESHOLD_FOR_PEAK && now - LAST(peaks) >= 300) {
    peaks.add(now);
    // Remove peaks which are not in the time windows anymore
    removeTooOldPeaks();
  }

  // Print bpm in regular intervalls
  if (now - lastPrint > PRINT_INTERVALL_IN_MS) {
    lastPrint = now;
    int current = bpm();
    Serial.print("BPM: ");
    Serial.println(current);
    Bridge.put("bpm", String(current));
  }
}

void removeTooOldPeaks() {
  while (peaks.size() > 0 && now - peaks.get(0) > TIME_WINDOW_IN_MS) {
    peaks.shift();
  }
}

int bpm() {
  if (peaks.size() > 1) {
      return peaks.size() * (MINUTE_IN_MS / (LAST(peaks) - FIRST(peaks)));
  } else {
    return 0;
  }
}

