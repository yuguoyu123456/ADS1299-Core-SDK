#!/usr/bin/env python3
"""Cross-platform ADS1299-Core serial-to-WebSocket gateway.

The gateway preserves the ADS1299-Core host packet semantics while exposing a
simple JSON stream that can be consumed by browsers, mobile applications,
smart-glass prototypes, dashboards and edge services.
"""

from __future__ import annotations

import argparse
import asyncio
from dataclasses import asdict
import json
import math
from pathlib import Path
import sys
import time
from typing import Any

import websockets

ROOT = Path(__file__).resolve().parents[3]
PC_PYTHON = ROOT / "pc_tools" / "python"
if str(PC_PYTHON) not in sys.path:
    sys.path.insert(0, str(PC_PYTHON))

from serial_reader import packets_from_serial  # noqa: E402


class Hub:
    def __init__(self) -> None:
        self.clients: set[Any] = set()
        self.frames = 0
        self.gaps = 0
        self.last_sequence: int | None = None

    async def register(self, websocket: Any) -> None:
        self.clients.add(websocket)
        hello = {
            "type": "hello",
            "project": "ADS1299-Core-SDK",
            "transport": "websocket-json-v1",
            "channels": 8,
            "server_time_unix_ms": int(time.time() * 1000),
        }
        await websocket.send(json.dumps(hello, separators=(",", ":")))

    def unregister(self, websocket: Any) -> None:
        self.clients.discard(websocket)

    def observe_sequence(self, sequence: int) -> None:
        if self.last_sequence is not None:
            expected = (self.last_sequence + 1) & 0xFFFFFFFF
            if sequence != expected:
                self.gaps += 1
        self.last_sequence = sequence
        self.frames += 1

    async def broadcast(self, payload: dict[str, Any]) -> None:
        if not self.clients:
            return
        message = json.dumps(payload, separators=(",", ":"))
        dead: list[Any] = []
        for client in tuple(self.clients):
            try:
                await client.send(message)
            except Exception:
                dead.append(client)
        for client in dead:
            self.unregister(client)


async def websocket_handler(websocket: Any, hub: Hub) -> None:
    await hub.register(websocket)
    try:
        async for message in websocket:
            if message == "status":
                await websocket.send(
                    json.dumps(
                        {
                            "type": "status",
                            "frames": hub.frames,
                            "gaps": hub.gaps,
                            "clients": len(hub.clients),
                        },
                        separators=(",", ":"),
                    )
                )
    finally:
        hub.unregister(websocket)


def frame_to_json(frame: Any, hub: Hub) -> dict[str, Any]:
    hub.observe_sequence(frame.sequence)
    return {
        "type": "eeg",
        "protocol_version": frame.version,
        "flags": frame.flags,
        "sequence": frame.sequence,
        "timestamp_us": frame.timestamp_us,
        "status_hex": frame.status.hex().upper(),
        "channels": list(frame.channels),
        "gateway": {
            "received_frames": hub.frames,
            "sequence_gaps": hub.gaps,
        },
    }


async def serial_source(hub: Hub, port: str, baud: int) -> None:
    iterator = packets_from_serial(port, baud)
    while True:
        frame = await asyncio.to_thread(next, iterator)
        await hub.broadcast(frame_to_json(frame, hub))


async def simulated_source(hub: Hub, rate_hz: float) -> None:
    sequence = 0
    t0 = time.perf_counter()
    period = 1.0 / rate_hz
    while True:
        now = time.perf_counter() - t0
        channels = [
            int(50000 * math.sin(2.0 * math.pi * (8.0 + ch * 0.35) * now + ch * 0.2))
            for ch in range(8)
        ]
        hub.observe_sequence(sequence)
        payload = {
            "type": "eeg",
            "protocol_version": 0,
            "flags": 0,
            "sequence": sequence,
            "timestamp_us": int(now * 1_000_000) & 0xFFFFFFFF,
            "status_hex": "C00000",
            "channels": channels,
            "gateway": {
                "received_frames": hub.frames,
                "sequence_gaps": hub.gaps,
                "simulated": True,
            },
        }
        await hub.broadcast(payload)
        sequence = (sequence + 1) & 0xFFFFFFFF
        await asyncio.sleep(period)


async def run(args: argparse.Namespace) -> None:
    hub = Hub()

    async def handler(websocket: Any) -> None:
        await websocket_handler(websocket, hub)

    source = (
        asyncio.create_task(simulated_source(hub, args.rate))
        if args.simulate
        else asyncio.create_task(serial_source(hub, args.serial, args.baud))
    )

    print(f"ADS1299-Core gateway listening on ws://{args.host}:{args.ws_port}")
    if args.simulate:
        print(f"Source: simulated 8-channel stream at {args.rate:g} Hz")
    else:
        print(f"Source: {args.serial} @ {args.baud} baud")

    async with websockets.serve(handler, args.host, args.ws_port, max_queue=8):
        try:
            await source
        finally:
            source.cancel()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="ADS1299-Core serial-to-WebSocket integration gateway"
    )
    parser.add_argument("--serial", help="serial device, e.g. COM5 or /dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=921600)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--ws-port", type=int, default=8765)
    parser.add_argument("--simulate", action="store_true", help="run without hardware")
    parser.add_argument("--rate", type=float, default=250.0, help="simulation sample rate")
    args = parser.parse_args()
    if not args.simulate and not args.serial:
        parser.error("--serial is required unless --simulate is used")
    if args.rate <= 0:
        parser.error("--rate must be positive")
    return args


def main() -> None:
    asyncio.run(run(parse_args()))


if __name__ == "__main__":
    main()
