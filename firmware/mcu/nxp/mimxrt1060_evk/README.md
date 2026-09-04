# NXP MIMXRT1060-EVK ADS1299 reference source

Status: **Reference source / build-pending**. Hardware validation pending.

Target: MIMXRT1062 with MCUXpresso SDK. First-stage firmware deliberately uses the shared GPIO/timing reference transport; the optimized target should later use LPSPI, GPIO interrupt and LPUART/USB without changing ADS1299 core APIs.
