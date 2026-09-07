# BLE transport integration

Use a vendor SDK GATT service in the consuming project. Set `max_chunk` from the
negotiated ATT payload, fragment the common packet without changing its bytes,
and reassemble it before packet validation on the host. Acquisition must not run
inside a radio callback; measure notification loss and reconnect behaviour.

Status: **Reference**. No GATT profile or radio performance is claimed.
