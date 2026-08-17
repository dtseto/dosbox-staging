# Boxer DOSBox-Staging Regression Coverage

This document maps every `BOXER_PATCHES.md` subsystem to automated coverage and remaining manual checks for future DOSBox-Staging merges. The automated tests are intentionally contract-oriented and avoid physical MIDI hardware, physical controllers, physical printers, MT-32 ROMs, real Metal presentation, the user's DOS Games folder, and internet access.

Run the Xcode smoke/source-contract suite:

```sh
xcodebuild test -workspace ../Boxer.xcworkspace -scheme "Boxer CI" -destination 'platform=macOS'
```

Or use Xcode's `Product > Test` with the `Boxer Integration Tests` target enabled in the active scheme.

Run the Meson GoogleTest contract suite, if the DOSBox-Staging native dependencies are installed:

```sh
meson test -C build boxer_integration_regression
meson test -C build boxer_merge_smoke
```

Coverage categories:

- `FULLY AUTOMATABLE`: current automated tests cover the documented failure mode without GUI or hardware.
- `PARTIALLY AUTOMATABLE`: automated tests protect source/bridge contracts, but some user-visible behavior still needs manual smoke testing.
- `MANUAL ONLY`: behavior genuinely requires the macOS app, hardware, or interactive presentation.

| BOXER_PATCHES subsystem | Marker(s) | Automated test(s) | Smoke test(s) | Manual test(s) | Coverage status | Remaining risk |
|---|---|---|---|---|---|---|
| Core bridge, SDL/event/mouse/video capture remaps | `coalface-remaps` | `BoxerIntegrationContractTests.testCoreBridgeContracts`; `BoxerIntegrationRegression.CoreBridgeRemapsStayBoxerOwned` | Xcode fast suite; `BoxerMergeSmoke.CoreInitializationAndBridgeOwnershipSurvive` | Launch a gamebox; verify frame presentation, mouse capture/release, app event handling, title/log/error handling, and screenshot/audio capture destination. | PARTIALLY AUTOMATABLE | Static bridge ownership is covered; actual Cocoa/Metal presentation and real mouse capture require the app. |
| Emulator run loop and shutdown lifecycle | `runloop-termination`, `runloop-event-cancellation`, `runloop-context`, `shutdown-drive-clear` | `BoxerIntegrationContractTests.testRunLoopAndShutdownContracts`; skipped `testRuntimeLifecycleBehavior`; `BoxerIntegrationRegression.RunLoopAndShutdownLifecycleRemainCancellableAndBalanced` | Xcode fast suite; `BoxerMergeSmoke.TerminationShutdownAndReinitializationHooksSurvive` | Start a game, close the session/window, quit during emulation, then relaunch and verify no retained mounted media. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime cancellation/reinit test is blocked until an XCTest target links DOSBox run-loop symbols with fake Boxer hooks. |
| Build/Xcode compatibility | `xcode-lazyflags-include`, `keyboard-enum-c-compat` | `BoxerIntegrationContractTests.testBuildCompatibilityContracts`; `BoxerIntegrationRegression.XcodeCompatibilityHooksRemainBuildable` plus ARM64 Xcode build | Xcode fast suite; `BoxerMergeSmoke.MarkerInventoryIsStillConsistent` | None beyond ARM64 Xcode build. | FULLY AUTOMATABLE | Xcode project settings can still fail outside these source-level assertions. |
| Configuration: MT-32, MIDI, and parallel printer | `boxer-mt32-config-include`, `mt32-device-value`, `mt32-help-unconditional`, `mt32-midiconfig-help`, `mt32-config-section`, `dosbox-parport-init`, `parallel-config-section` | `BoxerIntegrationContractTests.testConfigurationContracts`; `BoxerIntegrationRegression.ConfigurationStillExposesBoxerMidiMt32AndPrinterSections` | Xcode fast suite; `BoxerMergeSmoke.CoreInitializationAndBridgeOwnershipSurvive` | Configure MT-32 and printer in a gamebox; verify app-managed MIDI and printer routing. | PARTIALLY AUTOMATABLE | Config hooks and values are covered; actual device selection/playback/printing UI need app testing. |
| MIDI routing and sysex policy | `midi-routing` | `BoxerIntegrationContractTests.testMIDIRoutingContracts`; `BoxerIntegrationContractTests.testRuntimeMIDIRoutingBehavior`; `BoxerIntegrationRegression.MidiRoutingBypassesUpstreamHandlersAndPreservesBytes` | Xcode fast suite; `BoxerMergeSmoke.AudioMidiVideoAndPrinterBoundariesSurvive` | Run General MIDI and MT-32 titles; verify selected Boxer MIDI target receives notes and sysexes without stuck playback. | PARTIALLY AUTOMATABLE | Runtime channel, realtime, and sysex byte delivery is covered with a fake Boxer sink and real `src/midi/midi.cpp`; timing-sensitive playback and real device selection remain manual. |
| Audio mixer volume bridge | `mixer-volume-bridge` | `BoxerIntegrationContractTests.testMixerVolumeBridgeContracts`; `BoxerIntegrationContractTests.testRuntimeMixerVolumeBridgeBehavior`; `BoxerIntegrationRegression.MixerVolumeBridgeControlsActiveChannelVolume` | Xcode fast suite; `BoxerMergeSmoke.AudioMidiVideoAndPrinterBoundariesSurvive` | Change Boxer volume while audio is playing; verify audible volume and `MIXER.COM` display. | PARTIALLY AUTOMATABLE | Runtime sample-output coverage now exercises real `src/hardware/envelope.cpp` and `src/hardware/mixer.cpp` with fake Boxer master-volume and host audio boundaries; actual audible output and `MIXER.COM` display still need app testing. |
| Video rendering, display options, capture files | `render-reset-strategy`, `display-mode-controls`, `display-refresh-rate`, `capture-file-routing`, `core-mode-title-refresh` | `BoxerIntegrationContractTests.testVideoRenderingContracts`; `BoxerIntegrationRegression.VideoRenderAndCaptureStayRoutedThroughBoxer` | Xcode fast suite; `BoxerMergeSmoke.AudioMidiVideoAndPrinterBoundariesSurvive` | Change rendering strategy, Hercules tint, CGA composite settings; take captures; trigger auto core changes; verify Boxer UI/title and capture destinations. | PARTIALLY AUTOMATABLE | Source-level callbacks are covered; Metal presentation, frame pacing, and actual capture files require the app. |
| Keyboard input, paste, lock keys, and layout | `keyboard-buffer-capacity`, `console-read-cancel`, `console-paste-availability`, `bios-key-paste-pop`, `bios-key-paste-peek`, `caps-lock-state`, `num-lock-state`, `scroll-lock-state`, `int16-cancel`, `keyboard-layout-switching-api`, `keyboard-cpi-buffer-storage`, `keyboard-layout-state-methods`, `keyboard-layout-bridge`, `macos-preferred-keyboard-layout`, `us-layout-remap-fix` | `BoxerIntegrationContractTests.testKeyboardContracts`; skipped `testRuntimeKeyboardPasteAndCancellationBehavior`; `BoxerIntegrationRegression.KeyboardInputPasteCancellationLocksAndLayoutsStayBridged` | Xcode fast suite; `BoxerMergeSmoke.InputDriveAndShellBoundariesSurvive` | Paste into a DOS prompt, cancel input during shutdown, toggle Caps/Num/Scroll Lock, and test `keyboardlayout=auto` under a non-US macOS layout. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime paste/cancellation test is blocked until XCTest links keyboard/console code with fake Boxer paste hooks. |
| Joystick and controller ownership | `gameport-timing-export`, `gameport-timing-state`, `mapper-free-autofire`, `gameport-poll-activation`, `gameport-timing-config`, `preserve-controller-ownership`, `dos-visible-joystick-state`, `joystick-handler-install-end` | `BoxerIntegrationContractTests.testJoystickOwnershipContracts`; skipped `testRuntimeJoystickOwnershipBehavior`; `BoxerIntegrationRegression.JoystickOwnershipRemainsSinglePathAndPollActivated` | Xcode fast suite; `BoxerMergeSmoke.InputDriveAndShellBoundariesSurvive` | Launch a joystick-aware game, verify first-poll activation, timed mode changes, DOS visibility, and no duplicate physical controller registration. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime handler/activation test is blocked until XCTest links joystick code with fake Boxer callbacks and IO-handler inspection. |
| Gamebox drive paths, file policy, and mounted media | `drive-system-path`, `initialize-drive-system-path`, `retrieve-drive-system-path`, `fat-drive-system-path`, `iso-drive-system-path`, `local-drive-system-path`, `drive-cache-filter-bridge`, `hide-host-metadata`, `file-create-write-policy`, `file-open-write-policy`, `file-open-write-policy-end`, `file-delete-write-policy`, `local-dir-create-policy`, `local-file-created`, `local-file-removed`, `local-open-file-removed`, `imgmount-drive-mounted`, `mount-drive-mounted`, `drive-unmounted`, `invalid-fat-image-fails-construction`, `invalid-fat-bootsector-fails-construction`, `suppress-cdrom-image-error-text`, `file-unavailable-notification`, `local-file-unavailable-notification`, `local-file-unavailable`, `unavailable-file-read`, `unavailable-file-write`, `unavailable-file-seek`, `unavailable-file-timestamp` | `BoxerIntegrationContractTests.testGameboxDriveAndMediaContracts`; skipped `testRuntimeGameboxFilesystemBehavior`; `BoxerIntegrationRegression.GameboxDriveFileAndMediaContractsStayBoxerManaged` | Xcode fast suite; `BoxerMergeSmoke.InputDriveAndShellBoundariesSurvive` | Mount/unmount temporary folders/images; create/delete files; try protected metadata writes; import invalid disk images; remove/eject backing media with DOS files open. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime filesystem/write-policy test is blocked until XCTest links DOSBox filesystem code with fake Boxer callbacks and temporary-drive harness. |
| Shell lifecycle, command injection, and launch tracking | `current-shell-export`, `active-shell-global`, `shell-run-lifecycle`, `shell-misc-bridge`, `shell-input-injection`, `shell-command-filter`, `batch-lifecycle-bridge`, `batch-file-ended`, `program-launch-lifecycle` | `BoxerIntegrationContractTests.testShellLifecycleContracts`; skipped `testRuntimeShellCallbackOrderingBehavior`; `BoxerIntegrationRegression.ShellLifecycleCommandInjectionAndLaunchTrackingStayOrdered` | Xcode fast suite; `BoxerMergeSmoke.InputDriveAndShellBoundariesSurvive` | Launch a game via Boxer, run injected commands, execute a `.BAT`, return to prompt, and verify delegate/state changes fire once in order. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime callback-order test is blocked until XCTest links shell code with fake Boxer callbacks. |
| Shell command UX compatibility | `hide-intro-command`, `shell-command-ux`, `delete-help-if-no-args`, `delete-unix-path-tolerance`, `rename-help-if-no-args`, `mkdir-help-if-no-args`, `mkdir-unix-path-tolerance`, `rmdir-help-if-no-args`, `rmdir-unix-path-tolerance`, `dir-unix-path-trailing-slash`, `dir-unix-path-tolerance`, `copy-help-if-no-args`, `copy-unix-path-tolerance`, `if-help-if-no-args`, `type-help-if-no-args`, `call-help-if-no-args`, `subst-help-if-no-args`, `loadhigh-help-if-no-args`, `loadhigh-unix-path-tolerance` | `BoxerIntegrationContractTests.testShellCommandUXContracts`; `BoxerIntegrationRegression.ShellCommandUxCompatibilityRemainsPresent`; existing `gtest shell_cmds` parser tests | Xcode fast suite; `BoxerMergeSmoke.MarkerInventoryIsStillConsistent` | Run built-ins with no args and slash-heavy host paths during import/install scenarios; verify no `INTRO.COM`. | PARTIALLY AUTOMATABLE | Marker and command table coverage exists; command-level behavioral cases should be expanded in the native harness. |
| Printer and parallel-port routing | `printer-redirection`, `parport-skip-occupied-lpt`, `bios-parport-include`, `int17-printer-emulation`, `bios-parport-detection-disabled`, `bios-equipment-parport-count`, `bios-refresh-parport-count`, `int21-printer-output` | `BoxerIntegrationContractTests.testPrinterRoutingContracts`; skipped `testRuntimePrinterRoutingBehavior`; `BoxerIntegrationRegression.PrinterAndParallelOutputStayOnBoxerBridge` | Xcode fast suite; `BoxerMergeSmoke.AudioMidiVideoAndPrinterBoundariesSurvive` | Print from a DOS app and DOS printer APIs; verify Boxer print session and LPT status/equipment count. | PARTIALLY AUTOMATABLE | Source contracts are covered; runtime output-stream test is blocked until XCTest links parport/printer code with fake Boxer printer sinks. |
| Localization | `localization-routing`, `upstream-localization-disabled` | `BoxerIntegrationContractTests.testLocalizationContracts`; `BoxerIntegrationRegression.LocalizationRoutesThroughBoxerResources` | Xcode fast suite; `BoxerMergeSmoke.AudioMidiVideoAndPrinterBoundariesSurvive` | Switch app language/localization and verify DOSBox shell/help strings resolve through Boxer resources. | PARTIALLY AUTOMATABLE | Message lookup routing is covered; app bundle language selection remains manual. |

## Manual Regression Checklist

- Actual Metal presentation and frame pacing.
- Fullscreen/window switching.
- Real mouse capture and release.
- Physical controller registration, first-poll activation, timed mode, and no duplicate event path.
- Actual CoreMIDI or external MIDI device selection.
- Actual MT-32 playback and sysex timing.
- Real audio output, mute/zero boundary, volume restoration, pause/resume/reset/shutdown around active mixer channels.
- CD audio and mounted-media lifecycle.
- Printing UI and emitted print stream.
- Screenshot/audio capture destinations in Boxer-managed storage.
- Quit while a game is actively running.
- Reopen/relaunch after shutdown and verify no duplicate callbacks or retained mounted media.

## Current Inventory

- `BOXER_PATCHES.md` subsystems: 14.
- Documented unique source markers: 111.
- Source unique Boxer markers: 111.
- Documented markers not found in source: none.
- Source markers not documented in `BOXER_PATCHES.md`: none.
- Automated coverage added for all 111 markers at source/bridge contract level.
- Xcode source-contract tests are split into one marker inventory test plus 14 subsystem tests.
- Xcode runtime MIDI routing is executable and uses real `src/midi/midi.cpp` with fake Boxer sinks.
- Xcode runtime mixer-volume coverage is executable and uses real `src/hardware/envelope.cpp` plus `src/hardware/mixer.cpp` with fake Boxer master-volume and host audio boundaries.
- Remaining Xcode runtime behavioral tests exist as skipped tests documenting the required fake-sink/test-seam contracts; they are not counted as passing automated runtime coverage yet.
- Production files modified for testability: none.
- `BOXER_TESTING` seams added: none.
