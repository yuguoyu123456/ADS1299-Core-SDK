# Direct use — companion bridge

## Start with the adapter contract

Use `adapter_contract.py` as the boundary between experiment logic and any vendor/platform audio API.

Your vendor-specific adapter should implement:

```text
connect()
disconnect()
play(event_index, stimulus_id)
stop(event_index, stimulus_id)
```

Return only timing/capability information the SDK/API actually exposes.

## Smoke test

```bash
python test_adapter_contract.py
```

Expected:

```text
companion adapter contract smoke test: PASS
```

The mock adapter requires no external audio SDK and is only a contract test.

## Vendor-specific adapter pattern

Recommended structure:

```text
experiment_controller.py
        |
AudioEndpointAdapter
        |
+-------+---------------------+
|                             |
platform/vendor adapter       mock/test adapter
|                             |
documented public API         no hardware
```

Keep EEG acquisition protocol code outside the vendor adapter.

## Command lifecycle

For every audio command:

1. allocate an experiment event ID;
2. log host monotonic dispatch time;
3. call the adapter;
4. record whether the command was accepted;
5. record endpoint timing only if the API truly exposes it;
6. for timing-critical experiments, compare against an independently measured physical onset.

## Reconnect

A companion bridge should treat endpoint reconnect as a state transition:

```text
DISCONNECTED -> CONNECTING -> READY -> ERROR/RECOVERY
```

Do not automatically replay old stimulus commands after reconnect unless the experiment protocol explicitly calls for that behavior.

## ADS1299 path

The ADS1299 acquisition stream should continue using canonical packet sequence/timestamps regardless of the audio endpoint connection state.

If audio endpoint failure should abort a trial/session, make that an explicit experiment-controller rule and write an event marker.

## Unsupported consumer devices

If a device has no documented API for the required operation, do not create a fake adapter that guesses hidden capabilities.

Use one of these alternatives instead:

- system audio output selected through the OS;
- phone/PC playback;
- an open hardware audio endpoint;
- a documented vendor SDK;
- a separate experimental controller.

## Privacy/security

Do not embed participant identifiers in Bluetooth names, MQTT topics, OSC paths or endpoint IDs when avoidable. Store study metadata separately under the experiment's data-governance policy.
