# USB transport integration

Use the official MCU SDK's USB CDC or vendor-specific bulk endpoint and bind its
non-blocking transmit function to `ads1299_transport_t`. USB configuration,
descriptors and middleware remain in the consuming vendor project. Buffer data
across host polling delays and expose disconnect or queue-overflow events.

Status: **Reference**. No USB stack or device test is bundled.
