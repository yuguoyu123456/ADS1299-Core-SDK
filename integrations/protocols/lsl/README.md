# Lab Streaming Layer (LSL) interoperability

LSL is the preferred ADS1299-Core interoperability path for neuroscience and multimodal laboratory workflows that need a common research stream interface and a host-side timing model.

The repository reference bridge is under:

```text
firmware/transport/lsl/
```

## Architectural boundary

Use LSL on a host/edge computer after canonical packet validation:

```text
ADS1299 hardware
   |
MCU/FPGA acquisition
   |
USB / Ethernet / Wi-Fi / other transport
   |
49-byte canonical packet validation
   |
host-side ADS1299 -> LSL bridge
   |
LSL consumers / multimodal recording tools
```

Do not put liblsl work inside ADS1299 DRDY ISR or other timing-critical acquisition code.

## Channel data

One ADS1299 device provides eight signed channel samples. The bridge may expose:

- raw ADC counts; or
- input-referred physical units when Vref and PGA gain are explicitly known and recorded.

Do not convert counts to microvolts using guessed reference/gain settings.

## Stream metadata

A useful research stream should document at minimum:

- channel count;
- sample rate;
- channel labels/order;
- units;
- device/system identifier;
- ADS1299 device count;
- hardware/firmware revision where practical;
- reference/electrode montage metadata where appropriate to the experiment.

## Timing

Preserve the device timestamp from the canonical packet as source metadata. If mapping device time into a host/LSL time domain, keep the mapping/calibration strategy explicit so host receive jitter is not silently treated as acquisition timing.

## 64-channel systems

For 8×ADS1299, expose a stable 64-channel order such as:

```text
global_channel = device_index * 8 + local_channel
```

Only combine devices into one 64-channel sample when synchronization/epoch alignment has already been validated in hardware/firmware/host aggregation.
