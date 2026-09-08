#!/usr/bin/env python3
"""Smoke test for the vendor-neutral audio companion adapter contract."""

from adapter_contract import MockAudioEndpoint


def main() -> None:
    endpoint = MockAudioEndpoint()

    before_connect = endpoint.play(0, "tone-a")
    assert not before_connect.accepted

    info = endpoint.connect()
    assert info.endpoint_id == "mock-0"
    assert "play" in info.capabilities

    play = endpoint.play(1, "tone-a")
    assert play.accepted
    assert play.event_index == 1
    assert play.endpoint_time_ns is None
    assert "not a physical-onset" in play.detail

    stop = endpoint.stop(2, "tone-a")
    assert stop.accepted

    endpoint.disconnect()
    after_disconnect = endpoint.stop(3, "tone-a")
    assert not after_disconnect.accepted

    try:
        endpoint.connect()
        endpoint.play(-1, "bad")
        raise AssertionError("negative event_index accepted")
    except ValueError:
        pass

    print("companion adapter contract smoke test: PASS")


if __name__ == "__main__":
    main()
