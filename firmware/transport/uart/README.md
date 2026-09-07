# UART transport integration

Bind the common transport `write` callback to the official SDK's buffered UART
or DMA transmit API. Use binary mode, document baud rate and framing, and size
the acquisition queue for worst-case transmit latency. A 49-byte packet at
250 samples/s requires 98 kbit/s before serial framing; choose a rate with
margin and verify sequence gaps on real hardware.

Status: **Reference**. No UART target or throughput is claimed here.
