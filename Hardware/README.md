### OX VOX Hardware
Schematic and layout created using [Eagle 7.7.0](https://www.autodesk.com/products/eagle/overview)

The design features eight momentary-push buttons, a basic smoothing and amplifier circuit, and a 800mW speaker. The buttons form a full octave keyboard from C (261.6 Hz) to C (523.3 Hz). The BMD-300 outputs a PWM signal that is smoothed and amplified by the LM4861 amplifier.

This layout is slightly different from the DC25 and BSidesPDX layouts. It features the LM4861 shutdown pin broken out, as suggested by [Micah Scott](https://github.com/scanlime). By breaking out the shutdown pin and connecting it to the remaining available BMD-300 GPIO, the amplifier can be to shutdown when not in use in order to save power. Please see the connection section for how to connect this, or connect it to ground to keep the amplifier always on.