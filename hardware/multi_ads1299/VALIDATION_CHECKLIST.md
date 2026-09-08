# Multi-ADS1299 16 / 32 / 64-Channel Validation Checklist

Run this checklist at 2, 4 and 8 devices. Do not call an 8-device system validated if only the single-device module was tested.

## A. Before populating multiple modules

- [ ] One core module has passed `hardware/ads1299_core_module/VALIDATION_CHECKLIST.md`.
- [ ] Host board SPI/clock/control pin mapping is documented.
- [ ] Backplane slot numbering is fixed.
- [ ] Device index to global channel mapping follows `CHANNEL_MAP_64CH.md` or another explicitly documented mapping.
- [ ] Power budget is calculated from measured module current.
- [ ] Transport budget is calculated from `THROUGHPUT_BUDGET.md`.

## B. Two devices / 16 channels

- [ ] Both device IDs read correctly and independently.
- [ ] CS0 and CS1 select only the intended device.
- [ ] No MISO contention is observed.
- [ ] Both DRDY signals are observed.
- [ ] START/RESET/clock strategy is verified.
- [ ] Internal test passes on all 16 channels.
- [ ] Input-short noise is recorded for all 16 channels.
- [ ] Device/channel order is verified on the host.
- [ ] Long-duration capture has no unexplained frame/packet loss.
- [ ] Inter-device timing/skew is measured.

Do not proceed to four devices until this stage is repeatable.

## C. Four devices / 32 channels

Repeat all 16-channel tests and add:

- [ ] CS0..CS3 address four independent devices.
- [ ] DRDY0..DRDY3 or the selected aggregation strategy is verified.
- [ ] Shared clock/START edge quality is checked at near and far modules.
- [ ] SPI service window is measured for all four frames.
- [ ] Power rail at the farthest module is measured under full acquisition/transport load.
- [ ] All 32 internal-test channels map correctly.
- [ ] All 32 input-short channels are compared for outliers.
- [ ] Transport queue high-water mark is recorded.
- [ ] Sequence/timestamp grouping reconstructs complete 32-channel sample groups.

## D. Eight devices / 64 channels

Repeat all prior tests and add:

- [ ] CS0..CS7 are unique and deterministic.
- [ ] All eight IDs can be read repeatedly without cross-talk.
- [ ] All DRDY lines/aggregation behavior are verified.
- [ ] Clock reaches all eight devices with acceptable logic/timing quality.
- [ ] START/RESET reaches all eight devices deterministically.
- [ ] No active clock or BIAS output contention exists.
- [ ] All eight 27-byte raw frames fit the acquisition service budget with margin.
- [ ] All 64 internal-test channels are confirmed in the host mapping.
- [ ] All 64 input-short channels are recorded and compared.
- [ ] The 64-channel sample group always contains the intended 8 device packets.
- [ ] Inter-device sampling skew is measured and documented.
- [ ] Readout latency/skew is measured separately from sampling skew.
- [ ] Full-rate USB/network/radio load does not corrupt acquisition.
- [ ] Queue/ring high-water marks and overflows are recorded.
- [ ] Long-duration raw capture shows no unexplained CRC/sequence failures.

## E. Failure injection

Where practical, deliberately test:

- [ ] one module absent;
- [ ] one module held in reset;
- [ ] one DRDY missing;
- [ ] one packet CRC corrupted in a host test;
- [ ] transport temporarily stalled;
- [ ] queue filled to its overflow policy;
- [ ] host disconnect/reconnect;
- [ ] module ID/order mismatch.

The system should report incomplete/lost data explicitly rather than silently reusing an old sample.

## F. 64-channel data integrity

For every complete sample group verify:

- [ ] exactly 8 device packets are present;
- [ ] each packet is 49 bytes at the canonical layer;
- [ ] CRC passes for every device packet;
- [ ] per-device sequence is correct;
- [ ] group/device timestamp relation is consistent;
- [ ] global channel order is 1..64 as documented;
- [ ] no channel is duplicated or omitted.

## G. Analog interaction

Compare one-module and eight-module conditions:

- [ ] input-short noise;
- [ ] reference stability;
- [ ] power-rail noise;
- [ ] clock coupling;
- [ ] USB/network/radio activity;
- [ ] BIAS/reference behavior.

If noise increases only when all modules are populated, treat it as a system-level hardware problem until measured otherwise.

## H. Release evidence

Archive with the 64-channel validation result:

- board/module/backplane revisions;
- device count;
- firmware commit;
- host/MCU/FPGA target;
- sample rate and PGA gain;
- clock/START/DRDY architecture;
- SPI lane topology;
- transport;
- measured throughput;
- queue/overflow statistics;
- synchronization/skew measurements;
- input-short noise summary;
- long-duration test length;
- known limitations.

This checklist is engineering validation. It does not establish medical-device certification, human electrical-safety compliance or EMC compliance.
