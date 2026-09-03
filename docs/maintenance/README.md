# Maintenance self-audits

This directory is the durable engineering record for scheduled repository maintenance.

Each maintenance run must:

1. Scan the repository at three levels: L1 leaf, L2 module, and L3 system.
2. Select exactly one primary target, with linked changes only when required by that target.
3. Record authoritative references consulted. ADS1299-specific register, timing, electrical, conversion, reference, BIAS/SRB, clock, SPI, command, and multi-device behavior must be checked against the current official Texas Instruments ADS1299 datasheet before editing.
4. State validation honestly. `Compiles`, `Bench-tested`, and `24h-tested` are evidence labels, not aspirations.
5. Record destructive-change preflight when applicable.
6. Score the run A-K from 0-5 and give a concrete corrective action for every score below 4.

## A-K rubric

- **A Technical correctness** — implementation matches authoritative behavior and intended architecture.
- **B Project completeness** — the selected target is complete enough to be useful, without nominal scaffolding.
- **C Reproducibility/buildability** — a future maintainer can reproduce the stated validation.
- **D Validation honesty** — claims are limited to what was actually checked.
- **E Source/license integrity** — provenance and licensing are clear for imported or adapted work.
- **F Maintainability/API quality** — shared core/API boundaries remain coherent.
- **G Documentation/usability** — user-facing behavior and limitations are understandable.
- **H Scope/cross-repo consistency** — linked docs, CI, protocol, and support claims do not contradict the change.
- **I Destructive safety** — deletion/move/rewrite risk was reviewed proportionally.
- **J Priority judgment** — the selected target addresses a high-value weakness rather than cosmetic churn.
- **K TI datasheet compliance** — required when ADS1299 chip-specific semantics are touched; otherwise mark N/A with reason.

Audit files use UTC timestamps in filenames (`YYYY-MM-DDTHHMMZ.md`) so ordering is unambiguous across local environments.
