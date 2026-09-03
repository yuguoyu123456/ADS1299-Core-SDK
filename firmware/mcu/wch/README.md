# WCH MCU reference projects

This directory contains complete ADS1299 reference projects for WCH microcontrollers.

## Current target

- [`ch32v307_evt/`](ch32v307_evt/) — CH32V30x / QingKe V4F RISC-V, WCH NoneOS SDK via PlatformIO

## Planned next targets

- CH32V203 — lower-cost RISC-V reference
- selected CH32F / CH32X targets where they add meaningful ecosystem coverage

A target is not considered supported merely because a port file exists. Each visible board project must contain a reproducible build, exact pin map, ADS1299 ID read, register access, internal test configuration, 27-byte DRDY-driven acquisition and common host packet output.
