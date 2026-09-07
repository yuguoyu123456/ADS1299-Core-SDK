# Wi-Fi transport integration

Bind a vendor socket send function below the common transport layer. TCP gives
ordered delivery but needs explicit reconnect and back-pressure handling; UDP
needs application-visible loss and ordering checks. Keep radio and network work
outside the DRDY path and preserve the common packet sequence counter and CRC.

Status: **Reference**. No network stack or throughput is bundled.
