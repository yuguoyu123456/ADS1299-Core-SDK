# Web/PWA Security Boundary

The PWA is a visualization client. Security decisions belong to the complete gateway/network deployment, not only to browser JavaScript.

## Development mode

The README's plain `ws://` development gateway is appropriate only on a trusted local development network.

Do not expose an unauthenticated development WebSocket directly to the public internet.

## Production/deployed mode

When the viewer is used outside a closed lab LAN, evaluate:

- HTTPS origin;
- `wss://` WebSocket transport;
- gateway authentication/authorization;
- origin checking;
- session expiration;
- device/gateway identity;
- rate limits/resource limits;
- network segmentation;
- audit logging appropriate to the application.

## Data minimization

The viewer only needs the fields required for display/diagnostics. Do not add participant identifiers, clinical notes or unrelated personal data to every high-rate EEG message.

Keep acquisition metadata separate and protect it according to the actual research/product data policy.

## Message validation

Treat gateway messages as untrusted input:

- require the expected message type;
- require exactly eight channel values for the current schema;
- enforce integer ranges;
- validate status formatting;
- bound retained display history;
- ignore/reject malformed messages;
- never use message text as unsanitized HTML.

`gateway_message.mjs` implements the numeric/schema checks for EEG messages.

## Denial-of-service considerations

A malicious or broken gateway can send messages faster than the browser can render.

Production clients should:

- bound queue/history length;
- decouple rendering rate from sample arrival rate;
- drop visualization-only updates if necessary while preserving loss diagnostics;
- close/reconnect on unreasonable message sizes/rates according to policy;
- keep the authoritative recorder outside the visualization path when reliability matters.

## Service worker

Only cache known application-shell assets. Do not cache live EEG WebSocket messages as if they were static web resources.

When application code changes, update the service-worker cache/version strategy so stale protocol/UI files do not persist indefinitely.

## Privacy boundary

Raw EEG and associated metadata may be sensitive research/health-related data depending on use. The repository does not define a universal privacy/compliance policy. Deployments must define:

- what is collected;
- where it is stored;
- who can access it;
- retention/deletion rules;
- whether data leaves the local network/device;
- required consent/governance for the actual study/product.

## Validation

Before exposing a gateway beyond a trusted lab network, test at least:

- malformed JSON;
- oversized messages;
- invalid channel counts/types;
- rapid reconnect loops;
- unexpected origins;
- authentication failure;
- TLS certificate behavior;
- gateway shutdown/restart.

A working live plot is not evidence of a secure deployment.
