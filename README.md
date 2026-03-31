# OneWireUART_Driver_STM32

Firmware and hardware documentation for a 1-Wire UART interface using the STM32F103C8T6 MCU.

## Overview

This repository contains an STM32CubeIDE project that implements single-line UART communication for Feetech STC/SCS serial servo motors.

The target MCU is the `STM32F103C8T6` (Blue Pill / STM32F1 series), and the firmware is built for use with an ST-Link programmer/debugger.

## Repository Structure

- `Firmware/` - STM32CubeIDE project for the `STM32F103C8T6`.
  - `Firmware/Debug/` - compiled output, including the `.elf` file for flashing.
  - `Firmware/u2u_URT1.ioc` - CubeMX configuration file.
  - `Firmware/STM32F103C8TX_FLASH.ld` - linker script for the STM32F103C8T6 flash layout.
- `Electronics/` - schematics and PCB documentation for the 1-Wire UART interface.
  - `Electronics/Schematic_FE_URT-1.pdf`
  - `Electronics/PCB_PCB_FE_URT-1_Local.pdf`

## Target Board

This project is intended for the `STM32F103C8T6` microcontroller.

> If you are using a Blue Pill or equivalent STM32F103 board, connect it via ST-Link for programming and debugging.

## Build and Flash

1. Open `Firmware/u2u_URT1.ioc` in STM32CubeIDE.
2. Generate the project code.
3. Build the project in STM32CubeIDE.
4. Flash the firmware using ST-Link.

Alternatively, flash the built ELF directly:

- File: `Firmware/Debug/OneWireUART_Driver_STM32.elf`
- Programmer: `ST-Link`
- Target MCU: `STM32F103C8T6`

## Hardware

The electronics documentation includes a 1-Wire UART circuit using common tri-state buffering and 3.3V logic components.

The schematic is designed to interface the STM32 UART with serial servo motors over a single bidirectional data line.

## Notes

- Ensure the board is powered with `3.3V` only.
- Do not connect `5V` signals directly to the STM32 UART pins.
- Use the ST-Link tool to verify the target is set to `STM32F103C8T6` before programming.

