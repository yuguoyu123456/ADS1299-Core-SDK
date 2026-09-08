import { Ads1299Packet, Ads1299Protocol } from './Ads1299Protocol';

export class Ads1299StreamParser {
  private buffer: Uint8Array = new Uint8Array(0);

  validPackets: number = 0;
  formatErrors: number = 0;
  discardedBytes: number = 0;

  reset(): void {
    this.buffer = new Uint8Array(0);
    this.validPackets = 0;
    this.formatErrors = 0;
    this.discardedBytes = 0;
  }

  feed(chunk: Uint8Array): Ads1299Packet[] {
    if (chunk.length > 0) {
      const merged: Uint8Array = new Uint8Array(this.buffer.length + chunk.length);
      merged.set(this.buffer, 0);
      merged.set(chunk, this.buffer.length);
      this.buffer = merged;
    }

    const output: Ads1299Packet[] = [];
    while (true) {
      const syncIndex: number = this.findSync(this.buffer);
      if (syncIndex < 0) {
        if (this.buffer.length > 0 && this.buffer[this.buffer.length - 1] === Ads1299Protocol.SYNC0) {
          this.discardedBytes += Math.max(0, this.buffer.length - 1);
          this.buffer = new Uint8Array([Ads1299Protocol.SYNC0]);
        } else {
          this.discardedBytes += this.buffer.length;
          this.buffer = new Uint8Array(0);
        }
        break;
      }

      if (syncIndex > 0) {
        this.discardedBytes += syncIndex;
        this.buffer = this.buffer.slice(syncIndex);
      }

      if (this.buffer.length < Ads1299Protocol.PACKET_SIZE) break;

      const candidate: Uint8Array = this.buffer.slice(0, Ads1299Protocol.PACKET_SIZE);
      try {
        const packet: Ads1299Packet = Ads1299Protocol.decode(candidate);
        output.push(packet);
        this.validPackets += 1;
        this.buffer = this.buffer.slice(Ads1299Protocol.PACKET_SIZE);
      } catch (_) {
        this.formatErrors += 1;
        this.discardedBytes += 1;
        this.buffer = this.buffer.slice(1);
      }
    }

    return output;
  }

  private findSync(data: Uint8Array): number {
    if (data.length < 2) return -1;
    for (let i = 0; i < data.length - 1; i += 1) {
      if (data[i] === Ads1299Protocol.SYNC0 && data[i + 1] === Ads1299Protocol.SYNC1) return i;
    }
    return -1;
  }
}

export class Ads1299SequenceTracker {
  last: number | null = null;
  gapEvents: number = 0;
  missingPackets: number = 0;
  duplicatesOrReordered: number = 0;

  reset(): void {
    this.last = null;
    this.gapEvents = 0;
    this.missingPackets = 0;
    this.duplicatesOrReordered = 0;
  }

  observe(sequence: number): void {
    if (!Number.isInteger(sequence) || sequence < 0 || sequence > 0xFFFFFFFF) {
      throw new Error('sequence must be uint32');
    }

    if (this.last === null) {
      this.last = sequence >>> 0;
      return;
    }

    const expected: number = (this.last + 1) >>> 0;
    sequence >>>= 0;
    if (sequence === expected) {
      this.last = sequence;
      return;
    }
    if (sequence === this.last) {
      this.duplicatesOrReordered += 1;
      return;
    }

    const forward: number = (sequence - expected) >>> 0;
    if (forward < 0x80000000) {
      this.gapEvents += 1;
      this.missingPackets += forward;
      this.last = sequence;
    } else {
      this.duplicatesOrReordered += 1;
    }
  }
}
