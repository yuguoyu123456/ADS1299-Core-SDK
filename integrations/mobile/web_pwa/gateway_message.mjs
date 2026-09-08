export const EEG_CHANNELS = 8;

function requireUint32(value, name) {
  if (!Number.isInteger(value) || value < 0 || value > 0xFFFFFFFF) {
    throw new TypeError(`${name} must be uint32`);
  }
  return value >>> 0;
}

function requireInt32(value, name) {
  if (!Number.isInteger(value) || value < -2147483648 || value > 2147483647) {
    throw new TypeError(`${name} must be int32`);
  }
  return value;
}

function normalizeStatusHex(value) {
  if (typeof value !== 'string' || !/^[0-9A-Fa-f]{6}$/.test(value)) {
    throw new TypeError('status_hex must be exactly 6 hexadecimal characters');
  }
  return value.toUpperCase();
}

/**
 * Validate/normalize one gateway EEG JSON message used by the PWA.
 *
 * Required fields:
 *   type: "eeg"
 *   sequence: uint32
 *   timestamp_us: uint32
 *   channels: exactly 8 signed int32 values
 *   status_hex: exactly three status bytes encoded as six hex characters
 *
 * Optional fields are preserved in `meta` only when the caller chooses to use
 * them; this function intentionally keeps the rendering contract small.
 */
export function normalizeEegMessage(input) {
  if (!input || typeof input !== 'object' || Array.isArray(input)) {
    throw new TypeError('message must be an object');
  }
  if (input.type !== 'eeg') {
    throw new TypeError('message.type must be "eeg"');
  }
  if (!Array.isArray(input.channels) || input.channels.length !== EEG_CHANNELS) {
    throw new TypeError(`channels must contain exactly ${EEG_CHANNELS} samples`);
  }

  const channels = input.channels.map((value, index) =>
    requireInt32(value, `channels[${index}]`)
  );

  return Object.freeze({
    type: 'eeg',
    sequence: requireUint32(input.sequence, 'sequence'),
    timestamp_us: requireUint32(input.timestamp_us, 'timestamp_us'),
    channels: Object.freeze(channels),
    status_hex: normalizeStatusHex(input.status_hex),
    flags: input.flags === undefined ? undefined : requireUint32(input.flags, 'flags') & 0xFF,
  });
}

export class SequenceTracker {
  constructor() {
    this.last = null;
    this.gapEvents = 0;
    this.missingPackets = 0;
    this.duplicatesOrReordered = 0;
  }

  reset() {
    this.last = null;
    this.gapEvents = 0;
    this.missingPackets = 0;
    this.duplicatesOrReordered = 0;
  }

  observe(sequence) {
    sequence = requireUint32(sequence, 'sequence');
    if (this.last === null) {
      this.last = sequence;
      return { kind: 'first', missing: 0 };
    }

    const expected = (this.last + 1) >>> 0;
    if (sequence === expected) {
      this.last = sequence;
      return { kind: 'ok', missing: 0 };
    }

    if (sequence === this.last) {
      this.duplicatesOrReordered += 1;
      return { kind: 'duplicate-or-reordered', missing: 0 };
    }

    const forward = (sequence - expected) >>> 0;
    if (forward < 0x80000000) {
      this.gapEvents += 1;
      this.missingPackets += forward;
      this.last = sequence;
      return { kind: 'gap', missing: forward };
    }

    this.duplicatesOrReordered += 1;
    return { kind: 'duplicate-or-reordered', missing: 0 };
  }
}

export function parseGatewayText(text) {
  if (typeof text !== 'string') throw new TypeError('WebSocket text payload required');
  return normalizeEegMessage(JSON.parse(text));
}
