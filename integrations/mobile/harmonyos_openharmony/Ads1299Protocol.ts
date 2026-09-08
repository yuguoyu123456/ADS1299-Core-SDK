export interface Ads1299Packet {
  version: number;
  flags: number;
  sequence: number;
  timestampUs: number;
  status: Uint8Array;
  channels: Int32Array;
  crc: number;
  raw: Uint8Array;
}

export class Ads1299Protocol {
  static readonly PACKET_SIZE: number = 49;
  static readonly CHANNELS: number = 8;
  static readonly VERSION: number = 0;
  static readonly SYNC0: number = 0xA5;
  static readonly SYNC1: number = 0x5A;
  private static readonly CRC_OFFSET: number = 47;

  static crc16CcittFalse(data: Uint8Array, offset: number = 0, length: number = data.length): number {
    if (offset < 0 || length < 0 || offset + length > data.length) {
      throw new Error('invalid CRC range');
    }

    let crc: number = 0xFFFF;
    for (let i = offset; i < offset + length; i += 1) {
      crc ^= (data[i] & 0xFF) << 8;
      for (let bit = 0; bit < 8; bit += 1) {
        crc = (crc & 0x8000) !== 0
          ? ((crc << 1) ^ 0x1021) & 0xFFFF
          : (crc << 1) & 0xFFFF;
      }
    }
    return crc >>> 0;
  }

  static decode(raw: Uint8Array): Ads1299Packet {
    if (raw.length !== Ads1299Protocol.PACKET_SIZE) {
      throw new Error(`packet size ${raw.length} != ${Ads1299Protocol.PACKET_SIZE}`);
    }
    if (raw[0] !== Ads1299Protocol.SYNC0 || raw[1] !== Ads1299Protocol.SYNC1) {
      throw new Error('bad sync');
    }
    if (raw[2] !== Ads1299Protocol.VERSION) {
      throw new Error(`unsupported protocol version ${raw[2]}`);
    }

    const expected: number = Ads1299Protocol.crc16CcittFalse(raw, 2, Ads1299Protocol.CRC_OFFSET - 2);
    const actual: number = Ads1299Protocol.u16le(raw, Ads1299Protocol.CRC_OFFSET);
    if (actual !== expected) {
      throw new Error(`CRC mismatch wire=0x${actual.toString(16)} expected=0x${expected.toString(16)}`);
    }

    const status: Uint8Array = raw.slice(12, 15);
    const channels: Int32Array = new Int32Array(Ads1299Protocol.CHANNELS);
    for (let ch = 0; ch < Ads1299Protocol.CHANNELS; ch += 1) {
      channels[ch] = Ads1299Protocol.i32le(raw, 15 + ch * 4);
    }

    return {
      version: raw[2],
      flags: raw[3],
      sequence: Ads1299Protocol.u32le(raw, 4),
      timestampUs: Ads1299Protocol.u32le(raw, 8),
      status,
      channels,
      crc: actual,
      raw: raw.slice(),
    };
  }

  static codeToMicrovolts(code: number, vrefVolts: number, gain: number): number {
    if (!Number.isInteger(code) || code < -2147483648 || code > 2147483647) {
      throw new Error('code must be int32');
    }
    if (!(vrefVolts > 0) || !(gain > 0)) {
      throw new Error('vrefVolts and gain must be positive');
    }
    return code * vrefVolts / (gain * 8388607.0) * 1_000_000.0;
  }

  private static u16le(data: Uint8Array, offset: number): number {
    return (data[offset] | (data[offset + 1] << 8)) >>> 0;
  }

  private static u32le(data: Uint8Array, offset: number): number {
    return (
      (data[offset]) |
      (data[offset + 1] << 8) |
      (data[offset + 2] << 16) |
      (data[offset + 3] << 24)
    ) >>> 0;
  }

  private static i32le(data: Uint8Array, offset: number): number {
    return (
      (data[offset]) |
      (data[offset + 1] << 8) |
      (data[offset + 2] << 16) |
      (data[offset + 3] << 24)
    );
  }
}
