# Web/PWA Validation Checklist

Use this checklist before calling a browser/mobile configuration supported.

## A. Static app

- [ ] `index.html` loads without console errors.
- [ ] `app.js` initializes the canvas/viewer.
- [ ] manifest is reachable.
- [ ] service worker registers where the browser supports it.
- [ ] installable PWA behavior is documented for the tested browser/OS.

## B. Message contract

- [ ] `node test_gateway_message.mjs` passes.
- [ ] valid 8-channel EEG JSON is accepted.
- [ ] malformed JSON is rejected/ignored.
- [ ] invalid channel count is rejected.
- [ ] out-of-range channel value is rejected by the strict validator.
- [ ] invalid status string is rejected.
- [ ] uint32 sequence wrap-around is handled.
- [ ] sequence gaps are counted explicitly.

## C. Gateway connection

- [ ] connect works to the intended WebSocket endpoint.
- [ ] disconnect updates UI state.
- [ ] gateway shutdown does not freeze the browser permanently.
- [ ] reconnect behavior is understood/documented.
- [ ] incorrect gateway URL produces a visible connection error.

## D. Signal display

- [ ] all 8 channels update.
- [ ] channel order matches the gateway/canonical mapping.
- [ ] sequence and timestamp fields update.
- [ ] ADS1299 status is displayed.
- [ ] bounded display history prevents unbounded memory growth.
- [ ] rendering remains responsive at the target sample/message rate.

## E. Packet-integrity boundary

The browser JSON layer has no canonical CRC of its own. Therefore verify upstream:

- [ ] gateway transport framing is validated;
- [ ] canonical 49-byte packet CRC is validated;
- [ ] sequence/timestamp/status/channels are decoded before JSON emission;
- [ ] gateway does not hide device packet gaps by renumbering messages.

## F. Mobile behavior

For each tested Android/iOS device:

- [ ] device model recorded;
- [ ] OS/browser version recorded;
- [ ] browser-tab mode tested;
- [ ] installed-PWA mode tested when supported;
- [ ] foreground session duration recorded;
- [ ] screen-lock behavior recorded;
- [ ] background/app-switch behavior recorded;
- [ ] Wi-Fi roam/reconnect behavior recorded where relevant;
- [ ] rendering lag/thermal/battery behavior observed for long sessions.

## G. 64-channel boundary

The current PWA is an 8-channel viewer. Before claiming 64-channel support:

- [ ] a versioned multi-device message schema exists;
- [ ] device index/global channel mapping is explicit;
- [ ] browser rendering strategy is benchmarked;
- [ ] JSON/network bandwidth is measured;
- [ ] authoritative raw recording remains lossless outside the visualization-only downsampling path.

## H. Security/deployment

For use beyond a trusted development LAN:

- [ ] HTTPS/WSS deployment evaluated;
- [ ] authentication/authorization strategy implemented when required;
- [ ] origin policy evaluated;
- [ ] malformed/oversized/rate-abusive messages tested;
- [ ] service-worker cache update behavior tested;
- [ ] data/privacy policy defined for the actual study/product.

## I. Release record

Record:

- PWA commit;
- gateway commit;
- firmware commit;
- browser/OS/device;
- sample rate;
- channel count;
- test duration;
- messages received;
- sequence gaps;
- reconnects;
- console/runtime errors;
- known limitations.

A passing Node smoke test proves only the message-validation code; it does not prove browser compatibility or real ADS1299 hardware operation.
