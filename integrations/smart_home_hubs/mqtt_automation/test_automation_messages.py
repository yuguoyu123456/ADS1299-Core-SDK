#!/usr/bin/env python3
"""Smoke test for automation_messages.py."""

from automation_messages import build_ack, build_intent, decode_json, encode_json


def main() -> None:
    intent = build_intent(
        "intent-0001",
        "scene_next",
        42,
        0.91,
        host_monotonic_ns=1000,
        metadata={"source": "bci-demo"},
    )
    assert intent["type"] == "automation_intent"
    assert intent["source_sequence"] == 42

    decoded = decode_json(encode_json(intent))
    assert decoded["intent_id"] == "intent-0001"
    assert decoded["metadata"]["source"] == "bci-demo"

    ack = build_ack(
        "intent-0001",
        "dispatched",
        "mock-hub",
        host_monotonic_ns=1200,
    )
    decoded_ack = decode_json(encode_json(ack))
    assert decoded_ack["type"] == "automation_ack"
    assert decoded_ack["adapter_id"] == "mock-hub"

    try:
        build_intent("x", "y", 0, float("nan"))
        raise AssertionError("invalid confidence accepted")
    except ValueError:
        pass

    try:
        decode_json('{"schema_version":99,"type":"automation_intent"}')
        raise AssertionError("unsupported schema accepted")
    except ValueError:
        pass

    print("MQTT automation message smoke test: PASS")


if __name__ == "__main__":
    main()
