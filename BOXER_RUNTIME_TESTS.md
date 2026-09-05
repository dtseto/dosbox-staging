# Boxer Runtime Regression Tests

## Purpose

This suite protects Boxer/MaddTheSane behavior while DOSBox Staging is upgraded. It complements `BOXER_PATCHES.md`: that manifest explains the production changes, while these tests invoke the changed production entry points and record Boxer-visible outcomes. `boxer-compat-local:BOXER_TEST_COVERAGE.md` remains historical source-contract coverage; it is not evidence that a runtime path was executed.

The current integration under test is DOSBox v0.79.1 at `92281b3ee732508334b6d96df81d37e2780428ec`.

## Architecture

```text
Shared behavioral expectation
        |
        +-- DOSBox079Adapter -> v0.79.1 production sources/entry points
        +-- DOSBox080Adapter -> deliberately unavailable pending API comparison
                +-- DOSBox0800Adapter, if a maintenance difference requires it
                +-- DOSBox0801Adapter, if a maintenance difference requires it
```

Expectations live in `Boxer Integration Tests/SharedBehavior`. Process execution, ordered events, clean-state checks, and adapter contracts live in `RuntimeHarnessSupport`. An adapter maps source locations and entry points; it must not alter expected behavior.

Each runtime harness compiles an unmodified production source into a child executable, supplies controlled linker fakes at host boundaries, calls a real production entry point, and checks recorded Boxer-visible effects. Mutations are made only in temporary source copies and must make the same expectation fail.

## Shared-expectation rules

1. Write an invariant once in the shared suite.
2. Put source paths, signatures, and property spelling in an adapter.
3. Do not accept both correct and incorrect outcomes.
4. Do not turn a failed assertion into a skip.
5. A compile failure is an adapter failure, not a successful negative integration test.
6. Change a shared expectation only for a documented intentional Boxer behavior change.

## Version matrix

| Tree | Adapter | Current result | Required result |
|---|---|---|---|
| Boxer-integrated v0.79.1 (`92281b3ee`) | `DOSBox079Adapter` | All production-linked v0.79.1 shared tests pass | All shared tests pass |
| Official v0.80.0 | Unverified | Not run | Boxer expectations fail because integration is absent; mouse regression detected |
| Boxer-integrated v0.80.0 | Future | Not available | Pass except documented upstream mouse defect where applicable |
| Official v0.80.1 | Unverified | Not run | Boxer expectations fail because integration is absent; restored acceleration detected |
| Boxer-integrated v0.80.1 | Future | Not available | All shared tests pass |

`DOSBox080Adapter` intentionally advertises no supported versions until v0.80.0 and v0.80.1 signatures and source layouts have been compared. Do not add runtime version branches to shared tests.

## Current runtime evidence

| Behavior | BOXER markers | Real production source and entry point | Faked dependencies | Mutation evidence | Status |
|---|---|---|---|---|---|
| Joystick/gameport | `gameport-poll-activation`, `gameport-timing-config`, `dos-visible-joystick-state`, `preserve-controller-ownership`, `joystick-handler-install-end` | `src/hardware/joystick.cpp`; `JOYSTICK_Init`, registered real read/write handlers, `JOYSTICK_Destroy` | Boxer activation sink, configuration, PIC time, I/O registry | Removed activation and duplicate install both fail | Passing |
| Run-loop context | `runloop-context` | `src/dosbox.cpp`; `DOSBOX_SetLoop`, `DOSBOX_RunMachine` | Controlled CPU loop and Boxer context sink | Removing either context callback fails | Passing, narrow |
| MIDI | `midi-routing` | `src/midi/midi.cpp`; `MIDI_RawOutByte` | Boxer MIDI sinks and capture boundary | Existing harness; mutation conversion pending | Passing |
| Mixer volume | `mixer-volume-bridge` | `src/hardware/mixer.cpp`; real mixer channel/update path | Boxer volume and SDL audio boundary | Existing harness; mutation conversion pending | Passing |
| Filesystem policy/lifecycle | `local-file-unavailable`, `local-dir-create-policy`, `drive-cache-filter-bridge`, `hide-host-metadata`, `file-create-write-policy`, `file-open-write-policy`, `file-delete-write-policy`, `local-file-created`, `local-file-removed`, `drive-system-path`, `local-drive-unmount`, `drive-cache-clear` | `src/dos/drive_local.cpp` and `src/dos/drive_cache.cpp`; real local-file operations, `MakeDir`, `CreateEntry`, `FileCreate`, `FileOpen`, `FileUnlink`, backing-path entry points, `localDrive::UnMount`, and cache construction/reset/destruction | DOS error sink, Boxer policy/notification bridges, minimal host/open-file dependencies, deterministic cache mapping, temporary storage | Unavailable-read removal, policy bypasses, removed notifications/routes/downgrade/path expansion/cache clear all fail independently | Passing |
| Shell lifecycle/batch | `shell-run-lifecycle`, `batch-lifecycle-bridge` | `src/shell/shell.cpp`; exact `DOS_Shell::Run`. `src/shell/shell_batch.cpp`; exact `BatchFile::~BatchFile` | Command-line results, batch/input bodies, display output, owning shell, continuation and Boxer callback sinks | Independent removal of start, pending-command, prompt-return, finish, autoexec, and batch-finish callbacks fails; normal source reruns pass | Passing |
| Executable/batch dispatch | `program-launch-lifecycle` | `src/shell/shell_misc.cpp`; exact `DOS_Shell::Execute(char *, char *)` | Path lookup/canonicalization, batch construction, DOS process parameter/register/memory services, INT 21h dispatch, output, Boxer callback sinks | Independent start/finish removal and duplication, plus both order-reversal substitutions, fail; normal source reruns pass | Passing |
| Command input | `shell-input-injection` | `src/shell/shell_misc.cpp`; exact `DOS_Shell::InputCommand(char *)` | Keyboard reads/writes, completion/history filesystem services, DOS configuration, cursor output, Boxer callback sinks | Independent read-start/read-finish removals and Boxer-handler bypass fail; normal source reruns pass | Passing |

The joystick harness performs two complete initialize/destroy cycles, covers timed and non-timed configuration, separates DOS visibility from host ownership, retains and invokes the production-installed handlers, and verifies one registration per direction per cycle. The run-loop harness performs two sessions and verifies paired context identities and strict will/did order.

## Remaining runtime prerequisites

There are no remaining v0.79.1 runtime-test skips. v0.80.0 and v0.80.1 adapter work remains deliberately unstarted and is outside this validation.

## Marker inventory

The inventory was regenerated from production sources on 2026-09-02 using unique `BOXER-HOOK` and `BOXER-BEGIN` marker names. It contains **111 unique markers**, matching `BOXER_PATCHES.md`. The executable marker test remains the authoritative full marker-to-subsystem inventory; this document does not duplicate the old hard-coded marker table.

Historical guards currently exercised directly include:

| Commit | Regression | Runtime guard |
|---|---|---|
| `3eb5394a`, `072b6c764` | Duplicate gameport port `0x201` registration | Registration inspector and duplicate-install mutation |
| `5152357a7` | Unsafe joystick destroy/reinitialize | Two full joystick lifecycle cycles |
| `c27a3c91` | Exceptional termination cleanup | Production-linked char-pointer and `boxer_emulatorException` rollback; each cleanup-removal mutation independently blocks the next initialization with status 9 |
| `dcaa4c60` | Background exception routing | Source contract only; runtime extension pending |
| `5ca68dd9` | Legacy gamebox fallback | Existing source contract; generated-config runtime test pending |
| `d9174a04` | Dynamic-core/JIT entitlements | Existing signed-build test |

## Running tests

In Xcode, select the **Boxer CI** scheme and run Product > Test. From a shell:

```sh
xcodebuild test -workspace ../Boxer.xcworkspace -scheme "Boxer CI" -destination 'platform=macOS'
```

Focused runtime tests are `BoxerJoystickRuntimeTests` and `BoxerLifecycleRuntimeTests`. The child compilers require Xcode's `clang++`; their temporary directories are removed after each case. No native Meson runtime target has been added yet, so there is no duplicate native expectation suite to run.

## Adding a future adapter

1. Copy only the nearest version adapter.
2. Never copy the shared expectation suite.
3. Update source lists and API mappings.
4. Run shared tests before changing assertions.
5. Treat failures as regressions until disproven.
6. Change a shared expectation only for a documented intentional Boxer behavior change.
7. Add or migrate BOXER markers when production behavior moves.
8. Update `BOXER_PATCHES.md` and this document together.
9. Run mutation guards.
10. Run the full version matrix.

Before sharing a v0.80 adapter, compare function signatures, source locations, configuration properties, the new mouse subsystem, mixer lifecycle, callback lifecycle, shell APIs, and filesystem APIs between v0.80.0 and v0.80.1. The known mouse-acceleration restoration must be a narrow version regression test, not a weakened ownership expectation.

## Counts and validation log

| Checkpoint | Passed | Failed | Skipped | Not run |
|---|---:|---:|---:|---:|
| Untouched baseline, 2026-09-01 | 23 | 0 | 6 | 0 |
| Shared infrastructure | 27 | 0 | 6 | 0 |
| Joystick harness and mutations | 28 | 0 | 5 | 0 |
| Run-loop context harness | 29 | 0 | 5 | 0 |
| Keyboard production harnesses | 33 | 0 | 4 | 0 |
| Printer redirection production harness | 34 | 0 | 4 | 0 |
| BIOS INT 17h production harness | 35 | 0 | 4 | 0 |
| DOS INT 21h printer production harness | 36 | 0 | 4 | 0 |
| BIOS LPT registration/count harness | 37 | 0 | 4 | 0 |
| Parallel-port lifecycle harness | 38 | 0 | 4 | 0 |
| Parallel handler installation; printer skip removed | 39 | 0 | 3 | 0 |
| Unavailable local-file production harness | 40 | 0 | 3 | 0 |
| Directory policy/atomic-denial harness | 41 | 0 | 3 | 0 |
| Drive-cache metadata filter harness | 42 | 0 | 3 | 0 |
| File-create policy/notification harness | 43 | 0 | 3 | 0 |
| File-delete policy/notification harness | 44 | 0 | 3 | 0 |
| Host backing-path production harness | 45 | 0 | 3 | 0 |
| File-open policy/read-only downgrade harness | 46 | 0 | 3 | 0 |
| Drive/cache teardown; filesystem skip removed | 47 | 0 | 2 | 0 |
| Run-loop cancellation and shutdown/reuse slice | 47 | 0 | 2 | 0 |
| Executable and command-input production harnesses; shell skip removed | 54 | 0 | 0 | 0 |

The baseline, joystick, run-loop, keyboard, and printer-redirection checkpoints built successfully with Xcode. The keyboard replacement compiles and invokes real `src/hardware/keyboard.cpp`, `src/ints/bios_keyboard.cpp`, `src/dos/dev_con.h`, and `src/dos/dos_keyboard_layout.cpp`. It covers paste peek/pop order and fallback, full-buffer boundaries, console and INT 16h cancellation, lock callbacks, preferred-layout initialization, layout switching, teardown, and second cycles. Its off-by-one, reversed-pop, removed-cancellation, removed-lock-callback, removed-preferred-layout, and removed-layout-switch mutations all fail their shared expectations.

The printer-redirection checkpoint compiles and invokes the real `src/hardware/parport/printer_redir.cpp` constructor, register methods, `Putchar`, and destructor against ordered fake Boxer bridge functions. Two complete object cycles verify the initialization query, exactly one data byte, the `0xD4`, `0xD5`, `0xD4` strobe sequence, one acknowledgement-status read, and direct data/status/control register forwarding. Independently executed bypass and duplicate-data-write mutations both compile and fail with behavioral status 12; the normal source passes again afterward. The harness applies a temporary preprocessor type-token shim because v0.79.1 declares `handleUpperEvent(uint16_t)` in `printer_redir.h` but defines `handleUpperEvent(int16_t)` in the production `.cpp`; the shim exists only in the generated temporary harness translation unit.

The BIOS checkpoint reads and compiles the real `INT17_Handler` implementation from `src/ints/bios.cpp` together with the real `CParallel::getPrinterStatus` implementation from `src/hardware/parport/parport.cpp`; no behavioral logic is reimplemented in Swift or in the adapter. A recording fake parallel-port object proves that function 0 routes exactly one byte to the selected port, reads status exactly once after success, returns the production-transformed status, and performs no write or status read for an absent port across two cycles. An independently compiled duplicate-`Putchar` mutation fails with behavioral status 12, and the normal source passes afterward.

The DOS checkpoint reads the exact INT 21h function 05 dispatch clause from real `src/dos/dos.cpp` and compiles it inside a controlled switch dispatcher. Recording parallel-port objects prove that the first available port receives exactly one byte, later ports receive none, and an empty port table stays silent across two cycles. Independently compiled bypass and duplicate-`Putchar` mutations both fail with behavioral status 11, and the normal source passes afterward. The broad printer skip remains until full `parport.cpp` registration and teardown/reinitialization are covered. The broader lifecycle skip remains because shutdown and rollback coverage is not yet complete.

The BIOS registration checkpoint reads and compiles the real `BIOS_SetLPTPort` function from `src/ints/bios.cpp` against isolated emulated BIOS memory. Two cycles prove base-address and timeout-byte writes, one/two/zero-port equipment counts, removal behavior, and preservation of unrelated equipment bits. An independently compiled removed-count mutation fails with behavioral status 11, and the normal source passes afterward. The broad printer skip remains until the `parport.cpp` handler-installation path is production-linked. The broader lifecycle skip remains because shutdown and rollback coverage is not yet complete.

The parallel lifecycle checkpoint reads and compiles the real `PARPORTS` constructor/destructor and `PARALLEL_Init`/`PARALLEL_Destroy` functions from `src/hardware/parport/parport.cpp`. Controlled backend objects prove that an occupied BIOS LPT1 slot is not replaced, only LPT2/LPT3 backends are created, reinitialization destroys the first set before creating the second, registered shutdown destroys the second set, and every global slot is cleared. Independently compiled occupied-check bypass and retained-slot mutations fail with behavioral statuses 10 and 14 respectively, and the normal source passes afterward.

The handler-installation checkpoint reads and compiles the real `CParallel` constructor/destructor from `src/hardware/parport/parport.cpp`. Recording IO objects prove exactly three byte-wide read handlers and three byte-wide write handlers at the correct base offsets, one BIOS base registration, one DOS device attachment, BIOS removal and device detachment during destruction, handler-object teardown, and a clean second cycle on a different LPT base. An independently compiled removed-read-handler mutation fails with behavioral status 10, and the normal source passes afterward. Together with the redirection, INT 17h, DOS function 05, BIOS count, and PARPORTS lifecycle checkpoints, this completes the production-linked printer replacement and permits removal of its placeholder skip. The broader lifecycle skip remains because shutdown and rollback coverage is not yet complete.

The unavailable local-file checkpoint reads and compiles the real `localFile` constructor, read, write, seek, timestamp, close, invalidation, and position-helper implementations from `src/dos/drive_local.cpp`. A real temporary host file is opened for each of two cycles and invalidated through `willBecomeUnavailable`; subsequent reads and writes succeed with zero transferred bytes, seeks succeed at position zero, timestamp refresh returns false, and close remains safe. An independently compiled removed-read-safety mutation fails with behavioral status 11, and the normal source passes afterward. The broad filesystem skip remains until create/open/truncate/delete/mkdir policy, callback cardinality, metadata filtering, denied-operation atomicity, backing paths, and drive/cache teardown are production-linked.

The directory-policy checkpoint reads and compiles the real `localDrive::MakeDir` implementation from `src/dos/drive_local.cpp` against an isolated temporary directory and controlled cache/Boxer bridges. Across two drive-shaped lifecycles, denied creation reports access denied without touching disk, invoking the creation bridge, or mutating cache state; allowed creation reaches the Boxer bridge exactly once, creates the host directory, and invalidates the cache exactly once. Independently compiled policy-bypass and removed-creation-route mutations fail with behavioral statuses 10 and 12, and the normal source passes afterward. The broad filesystem skip remains until file create/open/truncate/delete policy and notifications, backing paths, and full drive/cache teardown are production-linked.

The drive-cache checkpoint reads and compiles the real `DOS_Drive_Cache::CreateEntry` implementation from `src/dos/drive_cache.cpp`. Across two fresh cache lifecycles, `.DS_Store` and the Finder `Icon` metadata name are rejected before short-name generation or insertion, while `SAVE.DAT` generates and inserts exactly one entry. An independently compiled filter-bypass mutation fails with behavioral status 10, and the normal source passes afterward. The broad filesystem skip remains until file-open and delete policy/notifications, backing paths, and full drive/cache teardown are production-linked.

The file-create checkpoint reads and compiles the real `localDrive::FileCreate` implementation from `src/dos/drive_local.cpp` against isolated temporary storage and controlled file/cache/Boxer dependencies. Across two fresh lifecycles, denied creation reports access denied without creating a host file, cache entry, or notification; allowed creation produces one empty file, one cache entry, and one Boxer notification; recreating an existing populated file truncates it, emits one additional notification, and does not duplicate the cache entry. Independently compiled policy-bypass and removed-notification mutations fail with behavioral statuses 10 and 13, and the normal source passes afterward. The broad filesystem skip remains until file-open policy, backing paths, and full drive/cache teardown are production-linked.

The file-delete checkpoint reads and compiles the real `localDrive::FileUnlink` implementation from `src/dos/drive_local.cpp` against isolated temporary storage and controlled cache/open-file/Boxer dependencies. Across two fresh lifecycles, a missing file reports file-not-found without consulting policy or mutating state, a denied existing file reports access denied and remains intact, and an allowed existing file removes one host file, one cache entry, and emits one Boxer notification. Independently compiled policy-bypass and removed-notification mutations fail with behavioral statuses 10 and 13, and the normal source passes afterward. The broad filesystem skip remains until file-open policy and full drive/cache teardown are production-linked.

The backing-path checkpoint reads and compiles the real `localDrive::GetSystemFilename` and `localDrive::GetSystemFilePtr` implementations from `src/dos/drive_local.cpp` against isolated deterministic storage and a controlled case-expansion cache. Across two fresh lifecycles, DOS alias resolution returns the expanded host path and opens the same real host file for the expected payload, with both entry points consulting the cache exactly once. An independently compiled removed-expansion mutation fails with behavioral status 10, and the normal source passes afterward. The broad filesystem skip remains until file-open policy and full drive/cache teardown are production-linked.

The file-open checkpoint reads and compiles the real `localDrive::FileOpen` implementation from `src/dos/drive_local.cpp` against isolated temporary storage and controlled cache/file/Boxer dependencies. Across two fresh lifecycles, denied write-only access reports access denied without returning a DOS file, denied read/write access opens the host file read-only and records read-only DOS flags, allowed read/write access preserves writable flags, and ordinary read access does not consult the write-policy bridge. Independently compiled policy-bypass and removed-downgrade mutations fail with behavioral statuses 10 and 12, and the normal source passes afterward. Xcode diagnostics were clean, the focused test passed, the project built successfully, and the complete 49-test suite recorded 46 passed, 0 failed, 3 skipped, 0 expected failures, and 0 not run. The broad filesystem skip remains until full drive/cache handle teardown and second-session cleanup are production-linked.

The drive/cache teardown checkpoint compiles the real `localDrive::UnMount` implementation from `src/dos/drive_local.cpp` and the real `DOS_Drive_Cache` constructors, destructor, `Clear`, `EmptyCache`, `ClearFileInfo`, and `DeleteFileInfo` implementations from `src/dos/drive_cache.cpp`. Instrumented cache nodes prove that reset releases the stale root tree and search references before creating an independent root, unmount deletes the drive and all remaining root/search objects, and two complete mount/reset/unmount cycles retain no cache state. An independently compiled destructor-`Clear` removal mutation fails with behavioral status 12, and normal production source passes afterward. Combined with the unavailable-file, directory, metadata-filter, create/truncate, open/downgrade, delete/notification, and backing-path checkpoints, this completes the production-linked filesystem replacement and permits removal of its placeholder skip. Xcode diagnostics were clean, the focused test passed, the project built successfully, and the complete 49-test suite recorded 47 passed, 0 failed, 2 skipped, 0 expected failures, and 0 not run; only lifecycle and shell remain skipped.

The run-loop cancellation checkpoint now invokes the real `Normal_Loop` and `DOSBOX_RunMachine` implementations from `src/dosbox.cpp`. Controlled host dependencies prove immediate cancellation before PIC/event processing, cancellation immediately after one idle event-processing pass, prompt exit when `shutdown_requested` is already set, balanced context callbacks after reuse, and clean subsequent sessions. Independently compiled removals of the entry cancellation check, event-side cancellation check, context-start callback, and context-finish callback all fail the shared expectation; normal source passes afterward. Xcode diagnostics were clean, the focused test passed, the project built successfully, and the complete suite remained 47 passed, 0 failed, 2 skipped, 0 expected failures, and 0 not run. The broad lifecycle skip remains because the Objective-C exceptional cleanup paths, failed-initialization rollback, shutdown cardinality, and retained shell/mixer/MIDI/callback state still require production-linked runtime coverage.

The Boxer-host lifecycle checkpoint invokes the real private `-[BXEmulator _startDOSBox]` entry point and therefore the production `DOSBOX_Init`, `Config::Init`, `Config::StartUp` ownership, and `Config` destruction paths. A private nil-by-default startup-phase callback in `BXEmulatorPrivate.h` is deliberately retained as a merge-visible regression seam; ordinary production execution installs no callback. The harness reaches the post-`Config::Init` phase, then independently triggers DOSBox's normal integer killswitch, a `char *` failure, and a `boxer_emulatorException`. Four normal completions surrounding the two exceptional completions prove repeated initialization, while direct assertions verify one video shutdown, released global configuration and command-line ownership, cleared drives, and no retained shell. Repeated real subsystem initialization behaviorally covers stale mixer, MIDI, callback, and gameport registrations. In an isolated temporary repository copy, removing the `char *` exceptional cleanup blocked the following initialization with mutation status 9. After restoring that cleanup and removing only the `boxer_emulatorException` cleanup, the following initialization independently failed with the same status. Normal production source remained untouched throughout the mutations. The focused normal-source lifecycle test passed, Xcode build-for-testing succeeded, and the complete suite recorded 50 total, 49 passed, 0 failed, 1 skipped, 0 expected failures, and 0 not run. This completes the broad lifecycle replacement and permits removal of its placeholder skip.

The shell-run checkpoint extracts and compiles the exact production `DOS_Shell::Run` body from `src/shell/shell.cpp` against controlled command-line, input, and Boxer callback dependencies. Normal prompt processing, pending-command execution, cancellation, `/C` early completion, `/INIT` autoexec notification, restoration of a pre-existing outer `currentShell`, and a second complete shell cycle produce strict ordered event sequences. Independent removals of shell-start, pending-command, prompt-return, shell-finish, and autoexec callbacks all fail; a bounded fake continuation sink prevents the pending-command mutation from spinning indefinitely, and normal production source passes again afterward. Xcode build-for-testing succeeded and the complete suite recorded 51 total, 50 passed, 0 failed, 1 skipped, 0 expected failures, and 0 not run.

The batch-completion checkpoint extracts and compiles the exact production `BatchFile::~BatchFile` implementation from `src/shell/shell_batch.cpp`. Across two complete batch lifecycles it proves the parent batch pointer and echo state are restored before exactly one `boxer_shellDidEndBatchFile` callback. An independent callback-removal mutation fails and normal source passes afterward. Xcode build-for-testing succeeded and the complete suite recorded 52 total, 51 passed, 0 failed, 1 skipped, 0 expected failures, and 0 not run. The broad shell placeholder remains because the executable launch/finish fragment and command-input mutation paths are not yet production-linked.

The executable checkpoint extracts and compiles the exact production `DOS_Shell::Execute(char *, char *)` implementation from `src/shell/shell_misc.cpp`. Controlled `Which`/canonicalization, batch construction, DOS parameter-block/register/memory operations, INT 21h dispatch, output, and Boxer sinks prove `boxer_shellWillExecuteFileAtDOSPath` precedes dispatch, `boxer_shellDidExecuteFileAtDOSPath` follows it, and both receive the identical canonical path. Separate cases prove `.BAT` uses only `boxer_shellWillBeginBatchFile`, unsupported extensions emit no executable callbacks, `.EXE` and `.COM` complete independently, and a third normal executable rerun remains clean. Independent removal and duplication of each executable callback and independent substitution of each callback with its opposite all fail strict order/cardinality/path expectations. The exact normal production method is recompiled and rerun after every mutation.

The command-input checkpoint extracts and compiles the exact production `DOS_Shell::InputCommand(char *)` implementation from `src/shell/shell_misc.cpp`. Controlled keyboard, display, completion/history filesystem, DOS configuration, and Boxer sinks prove will-read precedes did-read and input handling; immediate execution retains an injected `HELLO` command and ends collection in one read cycle; a separate injected cursor index of 2 causes production to insert `a` as `HEaLLO`; cancellation returns immediately after the paired read callbacks; ordinary `a` plus Return remains unchanged; and a second ordinary cycle is clean. Independent removal of each read callback and bypass of `boxer_handleShellCommandInput` fail, with the exact normal method recompiled and rerun after every mutation. The remaining limitation is intentional isolation of hardware keyboard, display, filesystem completion, and real DOS INT 21h execution behind fakes; the production branch, loop, path selection, callback calls, and relative ordering are not reproduced by the harnesses.

## Manual-only coverage

Physical controller behavior, real MIDI/MT-32 output, audible audio, Cocoa/Metal presentation and frame pacing, fullscreen/window transitions, real printer UI/output, real removable-media handling, and user-facing localization selection remain manual. Automated fakes should protect routing and lifecycle semantics without claiming those physical or presentation checks.
