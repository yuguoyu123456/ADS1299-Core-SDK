# Clock, START and DRDY Synchronization

A 64-channel system must define what "same sample time" means across eight ADS1299 devices. Do not assume simultaneous sampling simply because every device uses the same nominal data rate.

## 1. Clock ownership

Choose one explicit architecture:

- all devices use their own internal oscillators and software accepts/measures resulting relative timing; or
- all devices receive one common external clock; or
- one designated clock source fans out to all modules through a validated buffer/distribution network.

For tightly aligned 64-channel EEG, a common clock is normally the architecture to evaluate first.

Never allow more than one output driver to own the same clock net.

## 2. START ownership

The conversion start mechanism must be deterministic across modules.

Options include:

- one shared hardware START signal;
- one synchronized command sequence whose skew is measured and acceptable;
- FPGA-controlled start/fan-out.

A shared hardware line simplifies the timing definition but still requires signal-integrity/fan-out validation.

## 3. RESET ownership

A shared RESET can put all devices into a known common state, but only if:

- every module receives valid logic levels;
- fan-out/edge quality is acceptable;
- no local pull/driver fights the shared line;
- the firmware reconfigures every device deterministically after reset.

## 4. DRDY strategies

### Per-device DRDY

Preferred when enough host/FPGA inputs are available.

Advantages:

- direct visibility into each converter timing;
- easy fault isolation;
- easier skew/jitter measurement.

### One designated DRDY

Possible only when the clock/start architecture guarantees the other devices remain aligned and this has been measured. Periodically verify the other DRDY lines during engineering validation even if production firmware does not consume them all.

### Logic aggregation

AND/OR/FPGA aggregation can reduce host pins but must preserve a well-defined event. Document polarity, pulse behavior and fault handling.

## 5. Define timestamps

Recommended model:

- one `sample_epoch` or group sequence identifies one 64-channel sampling instant;
- each device retains its own canonical packet sequence;
- host/firmware records device index and timestamp;
- any measured inter-device offset/skew is preserved as validation evidence.

Do not stamp each device only after its sequential SPI read and then mistake readout latency for conversion-time difference.

## 6. Measure skew

Use a logic analyzer or oscilloscope to measure:

- shared clock at the source and farthest modules;
- START arrival at representative modules;
- DRDY edges from at least the first and last modules;
- CS/read start time per device.

Repeat under:

- 2 devices;
- 4 devices;
- 8 devices;
- intended sample rate;
- intended USB/network/radio load;
- long-duration operation.

## 7. Readout skew vs sampling skew

These are different:

- **sampling skew**: difference in actual conversion timing between devices;
- **readout skew**: difference in when firmware clocks each already-converted frame out over SPI.

A single shared SPI bus naturally creates readout skew, even if the ADCs sampled synchronously. Keep these concepts separate in documentation and analysis.

## 8. Fault handling

Define behavior if one module:

- stops asserting DRDY;
- resets unexpectedly;
- misses a START event;
- returns invalid status/data;
- has a clock fault.

The 64-channel aggregator should report the group as incomplete rather than silently substituting data from another time point.

## Acceptance gate

Multi-device synchronization is validated only when the chosen clock/START/DRDY architecture has measured timing/skew results that meet the actual system requirement and remains stable at the target device count/sample rate.
