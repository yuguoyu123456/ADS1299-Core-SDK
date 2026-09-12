# ADS1299 high-level control layer

This optional layer makes the portable ADS1299 core easier to use from MCU
shells, laboratory bring-up tools and examples. It deliberately **does not own
register semantics**. Every hardware operation is delegated to
[`../../core_driver/ads1299/`](../../core_driver/ads1299/), which remains the
single ADS1299 register/command authority in this repository.

## Why this exists

A datasheet-complete low-level driver and an easy interactive user interface
solve different problems. The core should stay small, strict and portable;
users still benefit from commands such as:

```text
rate 500
gain 1 24
input 1 normal
srb1 on
biasref internal
biasp 0xFF
biasn 0xFF
bias on
start
```

This directory provides that convenience without moving Arduino, UART, USB or
other platform policy into the core.

## Files

- `ads1299_control.[ch]` — ergonomic setters, safe diagnostic profiles and
  caller-buffer batch capture.
- `ads1299_cli.[ch]` — transport-neutral text command parser. The caller
  supplies a writer callback, so the same parser can sit behind UART, USB CDC,
  TCP, a debug console or a host test.

## Safe profiles

Three conservative profiles are provided:

- `ADS1299_CONTROL_PROFILE_EEG` — 250 SPS, gain 24, normal inputs, reference
  buffer enabled, with SRB1 and BIAS **off by default**. The application must
  explicitly opt into the reference/BIAS topology that matches its hardware.
- `ADS1299_CONTROL_PROFILE_INPUT_SHORT` — input-short noise/bring-up profile.
- `ADS1299_CONTROL_PROFILE_INTERNAL_TEST` — ADS1299 internal calibration source
  on all available channels.

The profile options permit another supported sample rate/gain and explicit EEG
SRB1/BIAS choices. No profile claims that a board is safe for human connection.
System isolation, leakage current, protection and clinical/electrical safety
remain hardware/system responsibilities.

## Batch capture

`ads1299_control_capture_frames()` stores frames in a **caller-owned buffer**.
The layer does not allocate a fixed `N x 16` global/static sample matrix. This
keeps RAM policy under the application and scales more naturally across MCU
families.

Example:

```c
ads1299_frame_t frames[32];
size_t captured = 0;

ads1299_control_t control;
ads1299_control_init(&control, &ads);

ads1299_status_t rc = ads1299_control_capture_frames(
    &control, frames, 32, 20000, 10, &captured);
```

The platform port must provide `drdy_read` for this helper.

## CLI integration example

```c
static void console_write(void *user, const char *text) {
    /* Send text through the platform's UART/USB/debug transport. */
}

ads1299_control_t control;
ads1299_cli_t cli;

ads1299_control_init(&control, &ads);
ads1299_cli_init(&cli, &control, console_write, NULL);

ads1299_cli_execute(&cli, "profile test");
ads1299_cli_execute(&cli, "start");
```

The complete command list is available through `help`.

## Licensing / provenance rule

This implementation is written against this repository's public ADS1299 core
API and the TI ADS1299-x datasheet. Public ADS1299 projects were useful as
workflow cross-checks, but **no third-party GPL source code is copied into this
directory**. This matters because some historical Arduino/OpenBCI-derived
ADS1299 manager libraries are GPL-licensed.

## Validation status

The source is intended to be compiled with `-Wall -Wextra -Werror` against the
current core headers in CI. That establishes source/API compatibility only.
Hardware status must remain unclaimed until a real ADS1299 target is tested.
