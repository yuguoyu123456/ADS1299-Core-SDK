#!/usr/bin/env python3
"""Dependency-light JSON message builders for smart-home MQTT bridges."""

from __future__ import annotations

import json
import math
import time
from typing import Any

SCHEMA_VERSION = 1


def build_intent(
    intent_id: str,
    command: str,
    source_sequence: int,
    confidence: float,
    *,
    host_monotonic_ns: int | None = None,
    metadata: dict[str, Any] | None = None,
) -> dict[str, Any]:
    if not intent_id or not command:
        raise ValueError("intent_id and command must not be empty")
    if not 0 <= source_sequence <= 0xFFFFFFFF:
        raise ValueError("source_sequence must fit uint32")
    value = float(confidence)
    if not math.isfinite(value) or not 0.0 <= value <= 1.0:
        raise ValueError("confidence must be finite and in [0, 1]")
    now = time.monotonic_ns() if host_monotonic_ns is None else host_monotonic_ns
    if now < 0:
        raise ValueError("host_monotonic_ns must be non-negative")

    return {
        "schema_version": SCHEMA_VERSION,
        "type": "automation_intent",
        "intent_id": intent_id,
        "command": command,
        "source_sequence": source_sequence,
        "confidence": value,
        "host_monotonic_ns": now,
        "metadata": metadata or {},
    }


def build_ack(
    intent_id: str,
    state: str,
    adapter_id: str,
    *,
    detail: str = "",
    host_monotonic_ns: int | None = None,
) -> dict[str, Any]:
    if not intent_id or not state or not adapter_id:
        raise ValueError("intent_id, state and adapter_id must not be empty")
    now = time.monotonic_ns() if host_monotonic_ns is None else host_monotonic_ns
    if now < 0:
        raise ValueError("host_monotonic_ns must be non-negative")
    return {
        "schema_version": SCHEMA_VERSION,
        "type": "automation_ack",
        "intent_id": intent_id,
        "state": state,
        "adapter_id": adapter_id,
        "host_monotonic_ns": now,
        "detail": detail,
    }


def encode_json(message: dict[str, Any]) -> bytes:
    return json.dumps(message, ensure_ascii=False, separators=(",", ":")).encode("utf-8")


def decode_json(payload: bytes | str) -> dict[str, Any]:
    if isinstance(payload, bytes):
        payload = payload.decode("utf-8")
    value = json.loads(payload)
    if not isinstance(value, dict):
        raise ValueError("automation MQTT payload must be a JSON object")
    if value.get("schema_version") != SCHEMA_VERSION:
        raise ValueError("unsupported automation schema_version")
    if value.get("type") not in {"automation_intent", "automation_ack"}:
        raise ValueError("unsupported automation message type")
    return value
