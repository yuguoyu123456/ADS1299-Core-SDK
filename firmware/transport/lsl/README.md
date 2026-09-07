# Lab Streaming Layer bridge

LSL belongs on a capable host or edge computer, not in the ADS1299 MCU Port.
The bridge should receive and validate common packets, convert device sequence
and timestamp metadata, then publish an LSL stream with channel labels, units,
sample rate and hardware identity. Clock synchronization must be measured on
the actual host/network path.

Status: **Planned**. This directory defines the boundary only; it does not claim
an implemented or validated LSL bridge.
