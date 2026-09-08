# Smart-home BCI failsafe matrix

Document the intended system response before running an experiment.

| Failure/condition | Required bridge behavior | Logging |
|---|---|---|
| EEG packet stream stops | generate no new command intents | disconnect/stale event |
| EEG signal marked invalid | suppress new intents | validity transition |
| classifier error | suppress new intents | error + algorithm state |
| command gate disabled | suppress new intents | user/system override |
| unknown command | reject | rejected command name/reason |
| MQTT/network unavailable | do not grow memory without bound | disconnect + queue/drop counters |
| hub unavailable | do not report success | dispatch failure/timeout |
| stale retained MQTT message | do not execute as a fresh intent | stale/rejected message |
| application restart | start in disabled/neutral state unless explicitly configured otherwise | restart/recovery event |
| manual user override | user/manual path takes precedence | override event |
| duplicate intent ID | reject or handle idempotently according to adapter policy | duplicate event |
| acknowledgement missing | mark outcome unknown/failed, not successful | timeout result |

## Recovery principle

Recovery should be explicit:

```text
FAULT/DISCONNECTED
      |
health checks / reconnect
      v
READY-BUT-NO-COMMAND
      |
new valid EEG + classifier state
      v
COMMAND-ELIGIBLE
```

Do not jump directly from a fault into replaying the last BCI action.

## Queue limits

Every queue between classifier, gate, MQTT client and automation adapter should have a documented finite capacity and overflow policy.

Do not use an unbounded queue to hide network outages.

## Unknown target state

If a command outcome is unknown because the hub/device did not acknowledge it, record that uncertainty. A later BCI decision should not assume the previous physical state changed unless the adapter can verify it.

## Test method

Inject each failure deliberately during validation rather than waiting for it to occur accidentally in a participant session.
