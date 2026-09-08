# Ear-EEG acquisition integration

This directory covers research systems in which an ADS1299 is physically inside, attached to, or located close to an ear-worn electrode assembly.

It does **not** imply direct firmware access to proprietary consumer earbuds or hearing devices.

## System role

In this architecture the ADS1299 path is the authoritative EEG acquisition source:

```text
ear electrodes
    |
input protection / analog front end
    |
ADS1299
    |
MCU/FPGA
    |
BLE / USB / Wi-Fi / other transport
    |
host/mobile research application
```

Audio playback may exist on the same wearable or on a separate endpoint, but EEG sample timing must remain independent from audio API callback timing.

## Channel model

Use explicit metadata for every electrode/channel rather than assuming a universal ear-EEG montage.

For each channel record at minimum:

- logical channel number;
- electrode label;
- left/right/other side or anatomical note where appropriate;
- positive/negative input routing;
- reference strategy;
- BIAS strategy;
- PGA gain;
- sample rate;
- enabled/disabled state.

Generic labels such as `EAR_L_1`, `EAR_L_2`, `EAR_R_1` are acceptable during engineering bring-up, but experiment recordings should use the actual montage metadata.

## Reference and BIAS

Ear-EEG does not remove the need to define reference and common-mode control explicitly.

Do not assume:

- all ear electrodes share one universal reference;
- a consumer earbud ground is an EEG reference;
- multiple ADS1299 BIAS outputs can be tied together directly;
- left/right ear mechanics produce identical electrode impedance or motion artifact.

Follow the hardware guidance in `hardware/ads1299_core_module/` and `hardware/multi_ads1299/`.

## Mechanical/artifact considerations

Ear-worn systems can be especially sensitive to:

- cable/connector movement;
- jaw movement;
- facial muscle activity;
- electrode pressure changes;
- fit/reseating;
- audio transducer mechanical vibration;
- wireless/processor activity close to the analog front end.

Record fit changes and test artifacts deliberately during validation.

## Data transport

Use the same repository canonical 49-byte packet after ADS1299 acquisition. Do not invent a special ear-EEG binary format unless a documented outer transport layer requires it.

BLE may be attractive for wearables, but validate the exact sample rate/channel count/payload/phone combination. For higher channel count or development work, USB or other higher-throughput links may be more appropriate.

## Safety boundary

This folder is an engineering/research integration guide. It does not establish electrical isolation, biocompatibility, clinical safety, hearing safety, or medical-device compliance.
