import { Ads1299Protocol } from './Ads1299Protocol';
import { Ads1299SequenceTracker, Ads1299StreamParser } from './Ads1299StreamParser';

function assert(condition: boolean, message: string): void {
  if (!condition) throw new Error(`assertion failed: ${message}`);
}

function putU32le(out: Uint8Array, offset: number, value: number): void {
  value >>>= 0;
  out[offset] = value & 0xFF;
  out[offset + 1] = (value >>> 8) & 0xFF;
  out[offset + 2] = (value >>> 16) & 0xFF;
  out[offset + 3] = (value >>> 24) & 0xFF;
}

function putI32le(out: Uint8Array, offset: number, value: number): void {
  putU32le(out, offset, value >>> 0);
}

function makePacket(sequence: number, timestampUs: number): Uint8Array {
  const out: Uint8Array = new Uint8Array(Ads1299Protocol.PACKET_SIZE);
  out[0] = Ads1299Protocol.SYNC0;
  out[1] = Ads1299Protocol.SYNC1;
  out[2] = Ads1299Protocol.VERSION;
  out[3] = 0x21;
  putU32le(out, 4, sequence);
  putU32le(out, 8, timestampUs);
  out[12] = 0xC0;
  out[13] = 0x00;
  out[14] = 0x00;

  const values: number[] = [-8388608, -123456, -1, 0, 1, 123456, 4194304, 8388607];
  for (let i = 0; i < values.length; i += 1) {
    putI32le(out, 15 + i * 4, values[i]);
  }

  const crc: number = Ads1299Protocol.crc16CcittFalse(out, 2, 45);
  out[47] = crc & 0xFF;
  out[48] = (crc >>> 8) & 0xFF;
  return out;
}

function concat(...parts: Uint8Array[]): Uint8Array {
  let total: number = 0;
  parts.forEach((part: Uint8Array) => total += part.length);
  const out: Uint8Array = new Uint8Array(total);
  let offset: number = 0;
  for (const part of parts) {
    out.set(part, offset);
    offset += part.length;
  }
  return out;
}

function run(): void {
  const p0: Uint8Array = makePacket(100, 1000);
  const decoded = Ads1299Protocol.decode(p0);
  assert(decoded.sequence === 100, 'sequence decode');
  assert(decoded.timestampUs === 1000, 'timestamp decode');
  assert(decoded.channels[0] === -8388608, 'negative channel decode');
  assert(decoded.channels[7] === 8388607, 'positive channel decode');

  const parser = new Ads1299StreamParser();
  const stream: Uint8Array = concat(new Uint8Array([1, 2, 3, 4]), p0, makePacket(101, 2000));
  const chunks: number[] = [1, 2, 7, 13, 29, 5, 64, 128];
  let offset: number = 0;
  const sequences: number[] = [];
  for (const chunkSize of chunks) {
    const end: number = Math.min(stream.length, offset + chunkSize);
    if (end > offset) {
      parser.feed(stream.slice(offset, end)).forEach(packet => sequences.push(packet.sequence));
    }
    offset = end;
  }
  if (offset < stream.length) {
    parser.feed(stream.slice(offset)).forEach(packet => sequences.push(packet.sequence));
  }
  assert(sequences.length === 2 && sequences[0] === 100 && sequences[1] === 101, 'arbitrary chunk recovery');
  assert(parser.discardedBytes >= 4, 'junk byte accounting');

  const corrupted: Uint8Array = makePacket(102, 3000);
  corrupted[20] ^= 0x01;
  const recovered = parser.feed(concat(corrupted, makePacket(103, 4000)));
  assert(recovered.some(packet => packet.sequence === 103), 'resync after CRC error');
  assert(parser.formatErrors >= 1, 'format error accounting');

  const tracker = new Ads1299SequenceTracker();
  tracker.observe(10);
  tracker.observe(11);
  tracker.observe(14);
  assert(tracker.gapEvents === 1, 'gap event count');
  assert(tracker.missingPackets === 2, 'missing packet count');

  const wrap = new Ads1299SequenceTracker();
  wrap.observe(0xFFFFFFFE);
  wrap.observe(0xFFFFFFFF);
  wrap.observe(0);
  assert(wrap.missingPackets === 0, 'uint32 wrap-around');

  console.log('HarmonyOS/OpenHarmony ADS1299 protocol smoke test: PASS');
}

run();
