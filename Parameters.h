const int SINE = 0x2000;                    // Define AD9833's waveform register value.
const int SQUARE = 0x2028;                  // When we update the frequency, we need to
const int TRIANGLE = 0x2002;                // define the waveform when we end writing.    

#define ADC0_PIN A0        // Define the ADC0 pin
#define ADC_CENTER 2048    // Center of the ADC range for 1.65V (12-bit ADC)
#define FM_RANGE 1.0       // Modulation range (1 octave up or down)

#define SUB_OUT1 10
#define SUB_OUT2 11

#define VELOCITY_PWM_PIN 12 // PWM output for velocity
#define NOTE_PWM_PIN 13     // PWM output for note value

#define SYNC_PIN 14


unsigned long lastToggle1 = 0; // Timestamp for GPIO 10
unsigned long lastToggle2 = 0; // Timestamp for GPIO 11

int detune = 0;
int noteVal = 0;
int interval = 0;
bool syncEnabled = false;
int type, channel, d1, d2;
byte noteon = 36;
double bend_factor = 0.0001;
float newbend = 0.01;
float frequency1 = 1.00;
float frequency2 = 1.00;
float squarewaveFrequency1 = 1.00;
float squarewaveFrequency2 = 1.00;
unsigned long interval1;
int adcValue;
float modulation = 1.00;
float fmModulation = 1.00;

// Global variables for portamento
unsigned long lastGlideTime = 0;
const int minGlideTime = 0; // Minimum glide time in milliseconds
const int maxGlideTime = 4000; // Maximum glide time in milliseconds
bool glideEnabled = false;  // Glide on/off
float currentFrequency1 = 0; // Current frequency for oscillator 1
float currentFrequency2 = 0; // Current frequency for oscillator 2
float glideStartFrequency1 = 0; // Starting frequency for glide (oscillator 1)
float glideStartFrequency2 = 0; // Starting frequency for glide (oscillator 2)
float glideTargetFrequency1 = 0; // Target frequency for glide (oscillator 1)
float glideTargetFrequency2 = 0; // Target frequency for glide (oscillator 2)
float currentGlideFrequency1 = 0; // Current interpolated frequency (oscillator 1)
float currentGlideFrequency2 = 0; // Current interpolated frequency (oscillator 2)
unsigned long lastGlideUpdate = 0; // Timestamp for glide interpolation
bool isGliding = false; // Indicates whether a glide is in progress
float exponent = 2.0; // Change this value as needed
int glidetime = 0;

// Octave switching
float oct_sw1 = 1.00;
float oct_sw2 = 1.00;
int oct_sel1, oct_sel2;

// Set up the MIDI codes to respond to by listing the lowest note
#define MIDI_NOTE_START 24   // B0

// Set the notes to be played by each key
float notes[] = {
  NOTE_C0, NOTE_CS0, NOTE_D0, NOTE_DS0, NOTE_E0, NOTE_F0, NOTE_FS0, NOTE_G0, NOTE_GS0, NOTE_A0, NOTE_AS0, NOTE_B0,
  NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1,
  NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2,
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
  NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
  NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7,
  NOTE_C8, NOTE_CS8, NOTE_D8, NOTE_DS8, NOTE_E8, NOTE_F8, NOTE_FS8, NOTE_G8, NOTE_GS8, NOTE_A8, NOTE_AS8, NOTE_B8,
  NOTE_C9
};
int numnotes;