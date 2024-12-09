# Dual-AD9833-AS3340-DCO

* Pico controlled AD9833 dual DCO with AS3340 waveshaping

Searching for a stable DCO with sawtooth, PWM, triangle and SUB waveforms.
I was watching a video on the CEM3340 and it's waveshaping capabilities.
It seems that it is a triangle core with waveshaping for it's ouputs.

The AD9833 is capable of triangle, square and sine wave outputs, all of which are difficult to convert to a sawtooth.
I experimented with injecting the triangle (amplified) of the AD9833 into the AS3340 triangle input buffer and eventually found a signal that triggered the wave conversion.

Based on this and an RP2040 controlling the AD8933 frequencies it was possible to construct a MIDI controlled dual DCO with upto 8 waveform outputs. 
2 saw, 2 PWM, 2 triangle and 2 sub outputs.

* The specifications are as follows

MIDI controlled
Dual oscillators 
Osc2 Detune
Osc2 Interval 0-12 semitones
FM input 0-3.3v (center 0 modulation)
Sawtooth 0-10v X2
PWM 0-10V X2
Triangle 0-10v X2
Sub oscillator 0-10v X2
Velocity output 0-5V
CV output 0-5V
Oscillator Sync (not tested yet)



