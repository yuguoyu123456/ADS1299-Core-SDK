# MIMXRT1042 ADS1299 ready-to-run path

Reuse this folder's existing NXP board/port integration with `../_shared/nxp_ads1299_ready_app.*` and the core-driver profile layer.

Configure LPSPI for mode 1 and DRDY falling-edge interrupt. Keep the ISR short, read exactly 27 bytes in the acquisition context, then queue frames for USB/Ethernet/storage/DSP.

Add DMA only after ID -> internal test -> input short -> EEG validation succeeds. Keep DMA buffer/cache handling and queue-overflow accounting explicit.

See `../DIRECT_USE.md` for the complete workflow.
