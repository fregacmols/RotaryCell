# Assembly Guide — v0.9 Prototype

This guide describes the working transformer-based prototype. It is not a production assembly specification.

## 1. Prepare the telephone

- Confirm the Model 500 dial, hookswitch, handset, network block, and mechanical bells are functional.
- Keep the telephone isolated from all external telephone wiring.

## 2. Mount the main electronics

- Mount the LilyGO T-A7670G-S3 Standard and AG1171 securely inside the telephone.
- Keep the cellular antenna and its feed away from low-level audio wiring.
- Provide access to USB-C for programming, charging, and battery-path wake-up after battery replacement.

## 3. Build the transformer audio board

Follow `Transformer_Audio_Interface.pdf`:

- T1: 600:600-ohm receive transformer
- T2: 600:600-ohm transmit transformer
- VR1: 10-kohm linear receive control
- VR2: 10-kohm linear transmit control
- Receive coupling: 10 nF
- Transmit coupling: 100 nF

Keep the LilyGO windings floating. Do not connect their audio conductors to ground.

## 4. Connect the AG1171

Use `ESP-CellCore_Service_Schematic_RevB.pdf` for power, control, TIP/RING, and audio pins. Place the power bypass capacitors close to the AG1171.

## 5. Connect the Model 500

Connect AG1171 TIP and RING to the internal Model 500 line terminals shown in the schematic. No public or building telephone line may be connected.

## 6. Load firmware

Open the sketch under `../Firmware/ESP-RotaryCell_v0.9/`, select the appropriate ESP32-S3 board and port, compile, and upload. Open the serial monitor at 115200 baud.

## 7. Commission the phone

- Verify modem registration.
- Place an incoming call and confirm physical ringing.
- Lift the handset and confirm answer.
- Verify two-way audio with the cellular phone in another room.
- Replace the handset and confirm call termination.
- Lift the handset, dial a normal test number, wait six seconds, and confirm outgoing dialing.
- Verify blocked-number behavior only through the serial output; ensure no `ATD911;` command is sent.
