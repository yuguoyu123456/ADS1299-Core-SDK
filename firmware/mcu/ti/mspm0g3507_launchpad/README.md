# TI LP-MSPM0G3507 ADS1299 reference source

Status: **Reference source / build-pending**. Hardware validation pending.

This is the low-cost TI MCU track. The first profile uses shared software SPI Mode 1 and GPIO DRDY polling so ADS1299 correctness is separated from DriverLib SPI configuration. Later graduation should use SPI/DMA and timer capture where useful.
