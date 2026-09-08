import assert from 'node:assert/strict';
import {
  SequenceTracker,
  normalizeEegMessage,
  parseGatewayText,
} from './gateway_message.mjs';

function validMessage(sequence = 1) {
  return {
    type: 'eeg',
    sequence,
    timestamp_us: 123456,
    status_hex: 'C00000',
    flags: 0x21,
    channels: [-8388608, -123, -1, 0, 1, 123, 4194304, 8388607],
  };
}

{
  const msg = normalizeEegMessage(validMessage(7));
  assert.equal(msg.sequence, 7);
  assert.equal(msg.channels.length, 8);
  assert.equal(msg.channels[0], -8388608);
  assert.equal(msg.status_hex, 'C00000');
}

{
  const msg = parseGatewayText(JSON.stringify(validMessage(9)));
  assert.equal(msg.sequence, 9);
}

{
  const bad = validMessage();
  bad.channels = [1, 2];
  assert.throws(() => normalizeEegMessage(bad));
}

{
  const bad = validMessage();
  bad.status_hex = 'xyz';
  assert.throws(() => normalizeEegMessage(bad));
}

{
  const bad = validMessage();
  bad.channels[0] = 2147483648;
  assert.throws(() => normalizeEegMessage(bad));
}

{
  const tracker = new SequenceTracker();
  assert.equal(tracker.observe(100).kind, 'first');
  assert.equal(tracker.observe(101).kind, 'ok');
  const gap = tracker.observe(104);
  assert.equal(gap.kind, 'gap');
  assert.equal(gap.missing, 2);
  assert.equal(tracker.gapEvents, 1);
  assert.equal(tracker.missingPackets, 2);
}

{
  const tracker = new SequenceTracker();
  tracker.observe(0xFFFFFFFE);
  assert.equal(tracker.observe(0xFFFFFFFF).kind, 'ok');
  assert.equal(tracker.observe(0).kind, 'ok');
}

console.log('web_pwa gateway message smoke tests: PASS');
