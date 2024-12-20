/*
AD9833 Waveform Module 
*/

#include <SPI.h>
#include "pitches.h"
#include "Parameters.h"
#include <MIDI.h>
#include "AD9833.h"
#include <hardware/pwm.h>

#define MIDI_CHANNEL 1
#define SLOT_ONE 1

AD9833 AD(5, 3, 2);   //  SW SPI over the HW SPI pins (UNO);
AD9833 AD1(27, 3, 2);  //  SW SPI over the HW SPI pins (UNO);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() {

  analogReadResolution(12);
  analogWriteFreq(10000);
  analogWriteRange(256);

  pinMode(GATE_PIN, OUTPUT);  // gate output
  pinMode(TRIG_PIN, OUTPUT);  // trig output
  pinMode(LED_PIN, OUTPUT);   // led output

  // Initialize pins to LOW
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  pinMode(SUB_OUT1, OUTPUT);  // Square wave output 1

  // Initialize pins to LOW
  digitalWrite(SUB_OUT1, LOW);

  // Configure PWM pins as outputs
  pinMode(VELOCITY_PWM_PIN, OUTPUT);
  pinMode(NOTE_PWM_PIN, OUTPUT);

  pinMode(PW1_PIN, OUTPUT);
  pinMode(PW2_PIN, OUTPUT);
  pinMode(PWM1_PIN, OUTPUT);
  pinMode(PWM2_PIN, OUTPUT);

  // Set initial PWM duty cycle to 0
  analogWrite(VELOCITY_PWM_PIN, 0);
  analogWrite(NOTE_PWM_PIN, 0);

  analogWrite(PW1_PIN, 0);
  analogWrite(PWM1_PIN, 0);
  analogWrite(PW2_PIN, 0);
  analogWrite(PWM2_PIN, 0);

  pinMode(SYNC_PIN, OUTPUT);
  digitalWrite(SYNC_PIN, LOW);

  MIDI.begin(0);
  MIDI.setHandleControlChange(myControlChange);
  MIDI.setHandleNoteOn(DinHandleNoteOn);
  MIDI.setHandleNoteOff(DinHandleNoteOff);
  MIDI.setHandlePitchBend(myPitchBend);

  SPI.begin();

  AD.begin();
  AD1.begin();

  AD.setWave(AD9833_TRIANGLE);
  AD.setFrequency(880.0, 0);
  AD.setFrequencyChannel(0);
  AD1.setWave(AD9833_TRIANGLE);
  AD1.setFrequency(880.0, 0);
  AD1.setFrequencyChannel(0);

  numnotes = sizeof(notes) / sizeof(notes[0]);

  bend_factor = 1;

  delay(2000);
}

void myControlChange(byte channel, byte control, byte value) {
  switch (control) {
    case 5:
      {  // Glide time
        // Use braces to scope variables properly
        float linearValue = map(value, 0, 127, 0, 4000);  // Convert CC to 0-4000 range
        glidetime = mapToExponential(linearValue, 1.5);   // Apply exponential mapping
        break;
      }
    case 65:                         // Glide on/off
      glideEnabled = (value >= 64);  // Turn on if value >= 64
      break;

    case 16:
      detune = map(value, 0, 127, 0, 15);
      break; 

    case 17:
      oct_sel1 = map(value, 0, 127, 0, 2);
      switch (oct_sel1) {
        case 0:
          oct_sw1 = 0.5;
          break;
        case 1:
          oct_sw1 = 1;
          break;
        case 2:
          oct_sw1 = 2;
          break;
      }
      break;

    case 18:
      oct_sel2 = map(value, 0, 127, 0, 2);
      switch (oct_sel2) {
        case 0:
          oct_sw2 = 0.5;
          break;
        case 1:
          oct_sw2 = 1;
          break;
        case 2:
          oct_sw2 = 2;
          break;
      }
      break;

    case 20:
      interval = value;
      if (interval > 12) {
        interval = 12;
      }
      break;

    case 21:
      // Keytrack 0-127
      keytrack = value;
      update_keytrack(keytrack_pitch);
      break;

    case 22:
      // Keytrack on/off
      keytrack_on = (value >= 64);
      update_keytrack(keytrack_pitch);
      break;

    case 23:                        // Sync on/off
      syncEnabled = (value >= 64);  // Turn on if value >= 64
      if (syncEnabled) {
        digitalWrite(SYNC_PIN, HIGH);
      } else {
        digitalWrite(SYNC_PIN, LOW);
      }
      break;

      case 24:
          if (SLOT_ONE == 1) {
          PW1_duty = map(value, 0, 127, 0, 256);
          analogWrite(PW1_PIN, PW1_duty);
          }
      break;

      case 25:
          if (SLOT_ONE == 1) {
          PW2_duty = map(value, 0, 127, 0, 256);
          analogWrite(PW2_PIN, PW2_duty);
          }
      break;

      case 26:
          if (SLOT_ONE == 1) {
          PWM1_depth = map(value, 0, 127, 0, 156);
          analogWrite(PWM1_PIN, PWM1_depth);
          }
      break;

      case 27:
          if (SLOT_ONE == 1) {
          PWM2_depth = map(value, 0, 127, 0, 156);
          analogWrite(PWM2_PIN, PWM2_depth);
          }
      break;
  }
}

void update_keytrack(byte pitch) {

    if (keytrack_on) {
        // Scale note value based on keytrack
        int scaledValue = map(keytrack, 0, 127, 0, 100);  // Scale keytrack to a percentage (0-100%)
        noteDutyCycle = map(pitch * scaledValue / 100, 0, 127, 0, 255);  // Apply scaling to pitch and map to PWM range
    } else {
        // Turn off the keytrack
        noteDutyCycle = 0;  
    }

    analogWrite(NOTE_PWM_PIN, noteDutyCycle);  // Set the PWM output
}

void DinHandleNoteOn(byte channel, byte pitch, byte velocity) {
    if (channel == MIDI_CHANNEL) {
        if (velocity == 0) {
            // Treat velocity 0 as Note Off
            DinHandleNoteOff(channel, pitch, velocity);
            return;
        }

        keytrack_pitch = pitch;
        update_keytrack(keytrack_pitch);

        // Set PWM duty cycle for velocity
        velocityDutyCycle = map(velocity, 0, 127, 0, 256);  // Map velocity to 8-bit range
        analogWrite(VELOCITY_PWM_PIN, velocityDutyCycle);

        // Setup glide parameters
        if (glideEnabled) {
            glideStartFrequency1 = currentGlideFrequency1;
            glideStartFrequency2 = currentGlideFrequency2;
            glideTargetFrequency1 = notes[(pitch - MIDI_NOTE_START)];
            glideTargetFrequency2 = notes[(pitch - MIDI_NOTE_START)];  // Base note without interval
            isGliding = true;
        } else {
            // No glide, set directly
            glideTargetFrequency1 = notes[(pitch - MIDI_NOTE_START)];
            glideTargetFrequency2 = notes[(pitch - MIDI_NOTE_START)];  // Base note without interval
            currentGlideFrequency1 = glideTargetFrequency1;
            currentGlideFrequency2 = glideTargetFrequency2;
            isGliding = false;
        }

        noteon = pitch;  // Store the active note

        digitalWrite(TRIG_PIN, HIGH);
        TRIG_START = millis();  // Record the timestamp
        digitalWrite(GATE_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
    }
}

void DinHandleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == MIDI_CHANNEL) {
    digitalWrite(GATE_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}

void myPitchBend(byte channel, int bend) {

  if (bend >= 0) {
    // Increase frequency until it's twice the original at maximum positive bend
    bend_factor = 1.0 + bend / 8192.0;
  } else {
    // Decrease frequency until it's half the original at maximum negative bend
    bend_factor = 1.0 / (1.0 - bend / 8192.0);
  }
}

// Function to map linear input to exponential curve
float mapToExponential(float x, float exponent) {
  // Ensure x is within the range [0, 4000]
  x = constrain(x, 0, 4000);

  // Map the linear input to exponential curve
  float y = pow(x / 4000.0, exponent) * 4000.0;

  return y;
}

void loop() {
  MIDI.read();  // Handle MIDI events

  if (millis() - TRIG_START >= TRIG_LENGTH) {
    digitalWrite(TRIG_PIN, LOW);
  }

  // Read FM input
  adcValue = analogRead(ADC0_PIN);
  modulation = (adcValue - ADC_CENTER) / float(ADC_CENTER);
  fmModulation = pow(2.0, modulation * FM_RANGE);

  // Read PolyMod input
  polymodValue = analogRead(ADC2_PIN);
  polymodulation = (polymodValue - ADC_CENTER) / float(ADC_CENTER);
  polyModulation = pow(2.0, polymodulation * PM_RANGE);

  // Handle glide if enabled
  unsigned long currentMillis = millis();
  if (isGliding && currentMillis - lastGlideUpdate >= 10) {   // Update every 10ms
    float step = (glidetime > 0) ? (10.0 / glidetime) : 1.0;  // Step size based on glidetime
    currentGlideFrequency1 += (glideTargetFrequency1 - currentGlideFrequency1) * step;
    currentGlideFrequency2 += (glideTargetFrequency2 - currentGlideFrequency2) * step;

    if (abs(currentGlideFrequency1 - glideTargetFrequency1) < 0.1 && abs(currentGlideFrequency2 - glideTargetFrequency2) < 0.1) {
      // Glide finished
      currentGlideFrequency1 = glideTargetFrequency1;
      currentGlideFrequency2 = glideTargetFrequency2;
      isGliding = false;
    }

    lastGlideUpdate = currentMillis;
  }

  // Dynamically calculate the interval-adjusted frequency for oscillator 2
  float intervalFrequency2 = currentGlideFrequency2 * pow(2.0, interval / 12.0);  // Apply interval as a semitone offset

  // Apply modulation and settings
  frequency1 = currentGlideFrequency1 * oct_sw1 * bend_factor * fmModulation * polyModulation;
  frequency2 = intervalFrequency2 * oct_sw2 * bend_factor * fmModulation - detune;

  squarewaveFrequency1 = frequency1 / 2.0;  // One octave below

  // Toggle GPIO 10 for square wave 1
  interval1 = 1000000.0 / squarewaveFrequency1;  // Microseconds per half-period
  if (micros() - lastToggle1 >= interval1) {
    digitalWrite(SUB_OUT1, !digitalRead(SUB_OUT1));  // Toggle pin
    lastToggle1 = micros();
  }

  // Update the AD9833 frequencies
  AD.setFrequency(frequency1, 0);
  AD1.setFrequency(frequency2, 0);
}
