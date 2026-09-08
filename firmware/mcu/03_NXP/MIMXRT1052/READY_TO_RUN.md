# MIMXRT1052 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure LPSPI for mode 1 and DRDY falling-edge GPIO interrupt. The ISR should only signal acquisition; read one exact 27-byte frame in the acquisition context and queue it for transport/DSP.

Validate ID -> internal test -> input short -> EEG profile before enabling DMA. When DMA/cache is used, keep buffer placement/coherency explicit and track queue overflows.

See `../DIRECT_USE.md` for the complete workflow.
