# Rank 13 verification — 2026-09-07

Status **Reference**. Rank 12 software gate passed before work began here.
An original queued SPI/GPIO binding is in esp_idf_adapter/. Unlike a stack-local
timeout descriptor, its context preserves buffers for an unfinished transaction.

Software reference gate passed on 2026-09-07; proceed to rank 14.
No hardware result is claimed. Completed software gate:

1. Confirm DevKitC-1 pin/strap restrictions and document exact reference wiring.
2. Add official ESP-IDF build and board configuration, using the shared Core.
3. Test nullable/32-byte boundary transfers, queue/result timeout, descriptor
   lifetime, GPIO errors and fault-state recovery restrictions.
4. Build actual C6 bootloader/application, record versions and artifact sizes.
5. Hardware ID/test-frame evidence remains separate; catalog remains Reference.

Available local PlatformIO Espressif32 6.10.0 includes a C6 board definition;
its presence alone does not prove a successful build. Any new downloads/caches
must remain within this platform's tests/.tools rather than global locations.

## Reproduce locally

`powershell -File tests/build_reference.ps1` from this platform directory uses
tests/.tools/pio, not the global PlatformIO core directory. Installed SDK/tools
are referenced through junctions there; new Python/cache/build files stay local.
For another machine prepare a local PlatformIO core with Espressif32 6.10.0 first.
Do not package or recursively delete junction targets.

The Windows ldgen command exceeded cmd.exe's 8191-character limit. The original
tests/windows_ldgen.py hook bypasses the shell for that command only, preserving
vendor sources and the argument sequence. Other commands keep the normal spawn.

## Executed logic tests

Run tests/run_queue_tests.py --cc <arm-none-eabi-gcc> using Python with Unicorn
2.1.4 and pyelftools 0.33. The existing RA6M5 test Python was used locally.
PASS: 31/32/33/65-byte paths, nullable buffers, queue failure, completion timeout,
wrong returned descriptor, persistent storage after late completion, GPIO failure,
negative/out-of-range pins and invalid wait parameters. Mock API interfaces are
test-only; this is ARM-emulated C logic, not ESP32-C6/real IDF driver execution.
Physical SPI timing, ID/readback, test waveform and sustained loss counts remain open.

## Actual ESP32-C6 build result

PASS: PlatformIO Core 6.1.19, Espressif32 6.10.0, IDF 5.4.0,
RISC-V ESP GCC 14.2.0+20241119. Official bootloader and application .bin images
generated successfully. Final-source rebuild through tests/build_reference.ps1
also passed. RAM 10728 / 327680 bytes; application flash 198881 / 1048576 bytes.
Module flash configuration is 8 MB; 1 MB above is the application partition.
Repository main/Port/adapter compile with -Werror against actual IDF headers.
No test fixture is in that build. No board was flashed or EEG frame observed.

`esptool.py --chip esp32c6 image_info firmware.bin` passed both checksum and
embedded validation hash. This checks image integrity, not execution on hardware.
Observed final application file SHA256:
`7696bcf64cb83e652aedf38317d8714a9119b5cca93ab5e390386979fd7fb725`.
