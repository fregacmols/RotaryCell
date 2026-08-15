# Roadmap

## v0.9 — next development target

### Hardware

- Bench-test the passive transformerless audio board in `Development/Experimental_Hardware`.
- Retain the transformer board if the passive circuit does not equal or improve its stability.
- Wire the GPIO36 filtered tone-injection circuit.
- Add a board holder STL only if one becomes useful.

### Firmware

- Audible dial tone through GPIO36.
- Busy and reorder/error tones.
- Manual tone-test commands.
- Low-battery state with averaged readings and hysteresis.
- Two short low-battery beeps before dial tone.
- Critical-battery warning behavior that avoids nuisance beeps during ordinary calls.

## Later, only when justified

- Final PCB after the audio interface is bench validated.
- Persistent user configuration.
- Improved assembly documentation and photographs.
- A v1.0 release when the hardware and firmware are stable and reproducible.
