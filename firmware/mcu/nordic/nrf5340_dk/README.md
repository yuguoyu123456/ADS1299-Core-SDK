# nRF5340 DK ADS1299 reference source

Status: **Reference source / build-pending**. Hardware validation pending.

Runs on the nRF5340 application core. Shared firmware owns ADS1299 reset/ID/configuration/internal-test/acquisition/frame/packet logic; this project owns only the nRF GPIO/timing binding. Future optimized revision should use SPIM + DPPI/GPIOTE/UARTE where useful.
