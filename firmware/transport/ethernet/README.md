# Ethernet transport integration

Bind a vendor or RTOS socket API below the common transport layer. Select TCP or
UDP according to latency and loss requirements, document buffering, and never
block the acquisition task indefinitely. Sequence and CRC checks remain part of
the common packet contract.

Status: **Reference**. No MAC, PHY, TCP/IP stack or board test is bundled.
