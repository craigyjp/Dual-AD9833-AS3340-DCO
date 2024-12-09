# Dual-AD9833-AS3340-DCO

Pico controlled AD9833 dual DCO with AS3340 waveshaping

Searching for a stable DCO with sawtooth, PWM, triangle and SUB waveforms.
I was watching a video on the CEM3340 and it's waveshaping capabilities.
It seems that it is a triangle core with waveshaping for it's ouputs.

The AD9833 is capable of triangle, square and sine wave outputs, all of which are difficult to convert to a sawtooth.
I experimented with injecting the triangle (amplified) of the AD9833 into the AS3340 triangle input buffer and eventually found a signal that triggered the wave conversion.

Based on this and an RP2040 controlling the AD8933 frequencies it was possible to construct a MIDI controlled dual DCO with upto 8 waveform outputs. 
2 saw, 2 PWM, 2 triangle and 2 sub outputs.

# The specifications are as follows

* MIDI controlled
* Dual oscillators 
* Osc2 Detune (MIDI controlled)
* Osc2 Interval 0-12 semitones (MIDI controlled)
* FM input 0-3.3v (center 0 modulation)
* Sawtooth 0-10v x2
* PWM 0-10V x2
* Triangle 0-10v x2
* Sub oscillator 0-10v x2
* Velocity output 0-5V
* Keytrack CV output 0-5V
* Keytrack scaling and on/off
* Oscillator Sync based on P600 softsync
* Pitchbend +/- 1 octave (MIDI controlled)
* Octave select -/0/+ 1 octave per DCO (MIDI controlled)
* Glide (MIDI controlled)
* PWM CV input 0-10V x2
* Gate output
* Trig output (10mS)

# MIDI controller values
  
* CC 5 - glide time 0-127 (0-4000mS)
* CC 65 - glide off/on >63 = on
* CC 17 - DCO1 octave select (0-127) -1 / 0 / +1
* CC 18 - DCO2 detune (0-127)
* CC 19 - DCO2 octave select (0-127) -1 / 0 / +1
* CC 20 - DCO2 interval (0-12) 0 - 12 semitones
* CC 21 - Keytrack scale (0-127) 0-100%
* CC 22 - Keytrack off/on >63 = on
* CC 23 - Sync off/on >63 = on



