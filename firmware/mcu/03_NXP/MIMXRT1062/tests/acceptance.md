# Rank 5 software-reference acceptance

Passed 2026-09-05: official SDK 2.16.000 compilation of 11 repository C files,
CMake library build, and complete RAM-debug ELF link. Zero undefined symbols.
Text/data/bss = 47108/280/2192 bytes. Reproduction and versions: ../build.md.

The adapter validates GPIO ranges/collisions and keeps ADS1299 chip behavior
in Core. The board binding provides SDK clock setup and actual pad selection;
the application exposes a latest-frame debugger snapshot. Startup/linker and
vendor sources are external SDK dependencies, not vendored files.

This passes the sequential software-reference gate; proceed to rank 6.
Catalog status remains Reference pending board execution. Runtime unit tests,
physical wiring, ID/readback, acquisition and 24-hour tests were not performed.
