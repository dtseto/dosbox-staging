#include "boxer_regression_contracts.h"

namespace {

using namespace boxer_regression;

TEST(BoxerIntegrationRegression, BoxerPatchesMarkerInventoryMatchesSource)
{
	// Protects every BOXER marker documented in BOXER_PATCHES.md.
	EXPECT_EQ(DocumentedMarkerSet(), SourceMarkerSet());
	EXPECT_EQ(DocumentedRows().size(), 14u);
	EXPECT_EQ(DocumentedMarkerSet().size(), 111u);
}

TEST(BoxerIntegrationRegression, CoreBridgeRemapsStayBoxerOwned)
{
	const auto root = ProjectRoot();
	// Protects BOXER marker: coalface-remaps
	ExpectBlockContains(root / "include/dosbox.h", "coalface-remaps", "#include \"BXCoalface.h\"");
	for (const auto *remap : {"#define GFX_Events boxer_processEvents",
	                          "#define GFX_StartUpdate boxer_startFrame",
	                          "#define GFX_EndUpdate boxer_finishFrame",
	                          "#define Mouse_AutoLock boxer_setMouseActive",
	                          "#define MIDI_Available boxer_MIDIAvailable",
	                          "#define OpenCaptureFile boxer_openCaptureFile",
	                          "#define E_Exit"})
		ExpectContains(root.parent_path() / "Boxer/BXCoalface.h", remap);
}

TEST(BoxerIntegrationRegression, RunLoopAndShutdownLifecycleRemainCancellableAndBalanced)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: runloop-termination, runloop-event-cancellation, runloop-context, shutdown-drive-clear
	ExpectContains(root / "src/dosbox.cpp", "if (!boxer_runLoopShouldContinue()) return 1;");
	ExpectBlockContains(root / "src/dosbox.cpp", "runloop-context", "boxer_runLoopWillStartWithContextInfo(&contextInfo);");
	ExpectBlockContains(root / "src/dosbox.cpp", "runloop-context", "boxer_runLoopDidFinishWithContextInfo(contextInfo);");
	ExpectContains(root / "src/dos/dos.cpp", "for (Bit16u i = 0; i < DOS_DRIVES; i++)");
	ExpectContains(root / "src/dos/dos.cpp", "Drives[i] = 0;");
}

TEST(BoxerIntegrationRegression, XcodeCompatibilityHooksRemainBuildable)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: xcode-lazyflags-include, keyboard-enum-c-compat
	ExpectContains(root / "src/hardware/iohandler.cpp", "#include \"../cpu/lazyflags.h\"");
	ExpectContains(root / "include/keyboard.h", "typedef enum KBD_KEYS KBD_KEYS;");
}

TEST(BoxerIntegrationRegression, ConfigurationStillExposesBoxerMidiMt32AndPrinterSections)
{
	const auto root = ProjectRoot();
	const auto dosbox = root / "src/dosbox.cpp";
	// Protects BOXER markers: boxer-mt32-config-include, mt32-device-value, mt32-help-unconditional, mt32-midiconfig-help, mt32-config-section, dosbox-parport-init, parallel-config-section
	ExpectContains(dosbox, "#include \"BXMIDIConfig.hpp\"");
	ExpectContains(dosbox, "#include \"parport.h\"");
	ExpectContains(dosbox, "\"mt32\",");
	ExpectContains(dosbox, "BXMIDIMT32_AddConfigSection(control);");
	ExpectContains(dosbox, "PARALLEL_Init");
	ExpectContains(dosbox, "parallel1");
	ExpectContains(dosbox, "parallel2");
	ExpectContains(dosbox, "parallel3");
}

TEST(BoxerIntegrationRegression, MidiRoutingBypassesUpstreamHandlersAndPreservesBytes)
{
	const auto root = ProjectRoot();
	const auto midi = root / "src/midi/midi.cpp";
	// Protects BOXER marker: midi-routing
	ExpectBlockContains(midi, "midi-routing", "#include \"BXCoalfaceAudio.h\"");
	ExpectBlockContains(midi, "midi-routing", "boxer_sendMIDIMessage(midi.rt_buf);");
	ExpectBlockContains(midi, "midi-routing", "boxer_sendMIDIMessage(midi.cmd_buf);");
	ExpectBlockContains(midi, "midi-routing", "boxer_sendMIDISysex(midi.sysex.buf, midi.sysex.used);");
	ExpectBlockContains(midi, "midi-routing", "DOSBox MIDI backends are intentionally disabled");
	ExpectBlockContains(midi, "midi-routing", "goto getdefault;");
	ExpectContains(midi, "boxer_suggestMIDIHandler(dev, fullconf.c_str());");
}

TEST(BoxerIntegrationRegression, MixerVolumeBridgeControlsActiveChannelVolume)
{
	const auto root = ProjectRoot();
	const auto mixer = root / "src/hardware/mixer.cpp";
	// Protects BOXER marker: mixer-volume-bridge
	ExpectBlockContains(mixer, "mixer-volume-bridge", "#import \"BXCoalfaceAudio.h\"");
	ExpectBlockContains(mixer, "mixer-volume-bridge", "boxer_masterVolume(BXLeftChannel)");
	ExpectBlockContains(mixer, "mixer-volume-bridge", "boxer_masterVolume(BXRightChannel)");
	ExpectBlockContains(mixer, "mixer-volume-bridge", "void boxer_updateVolumes()");
	ExpectBlockContains(mixer, "mixer-volume-bridge", "it.second->UpdateVolume();");
	ExpectContains(mixer, "ShowVolume(\"MASTER\", boxer_masterVolume(BXLeftChannel), boxer_masterVolume(BXRightChannel));");
}

TEST(BoxerIntegrationRegression, VideoRenderAndCaptureStayRoutedThroughBoxer)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: render-reset-strategy, display-mode-controls, display-refresh-rate, capture-file-routing, core-mode-title-refresh
	ExpectBlockContains(root / "src/gui/render.cpp", "render-reset-strategy", "boxer_applyRenderingStrategy();");
	ExpectBlockContains(root / "src/hardware/vga_other.cpp", "display-mode-controls", "boxer_setHerculesTintMode");
	ExpectBlockContains(root / "src/hardware/vga_other.cpp", "display-mode-controls", "boxer_setCGACompositeHueOffset");
	ExpectBlockContains(root / "src/hardware/vga_other.cpp", "display-mode-controls", "boxer_setCGAComponentMode");
	ExpectBlockContains(root / "src/hardware/vga_other.cpp", "display-refresh-rate", "int boxer_GetDisplayRefreshRate(void)");
	ExpectBlockContains(root / "src/hardware/hardware.cpp", "capture-file-routing", "#if 0");
	ExpectContains(root / "src/dos/dos_execute.cpp", "GFX_SetTitle(-1,-1,false);");
}

TEST(BoxerIntegrationRegression, KeyboardInputPasteCancellationLocksAndLayoutsStayBridged)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: keyboard-buffer-capacity, console-read-cancel, console-paste-availability, bios-key-paste-pop, bios-key-paste-peek, caps-lock-state, num-lock-state, scroll-lock-state, int16-cancel, keyboard-layout-switching-api, keyboard-cpi-buffer-storage, keyboard-layout-state-methods, keyboard-layout-bridge, macos-preferred-keyboard-layout, us-layout-remap-fix
	ExpectContains(root / "src/hardware/keyboard.cpp", "Bitu boxer_keyboardBufferRemaining()");
	ExpectContains(root / "src/dos/dev_con.h", "boxer_continueListeningForKeyEvents()");
	ExpectContains(root / "src/dos/dev_con.h", "boxer_numKeyCodesInPasteBuffer()");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_getNextKeyCodeInPasteBuffer(&code, true)");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_getNextKeyCodeInPasteBuffer(&code, false)");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_setCapsLockActive");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_setNumLockActive");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_setScrollLockActive");
	ExpectContains(root / "src/ints/bios_keyboard.cpp", "boxer_continueListeningForKeyEvents()");
	ExpectBlockContains(root / "src/dos/dos_keyboard_layout.cpp", "keyboard-layout-bridge", "boxer_keyboardLayoutName()");
	ExpectBlockContains(root / "src/dos/dos_keyboard_layout.cpp", "keyboard-layout-bridge", "boxer_setKeyboardLayoutActive");
	ExpectContains(root / "src/dos/dos_keyboard_layout.cpp", "boxer_preferredKeyboardLayout()");
}

TEST(BoxerIntegrationRegression, JoystickOwnershipRemainsSinglePathAndPollActivated)
{
	const auto root = ProjectRoot();
	const auto joystick = root / "src/hardware/joystick.cpp";
	// Protects BOXER markers: gameport-timing-export, gameport-timing-state, mapper-free-autofire, gameport-poll-activation, gameport-timing-config, preserve-controller-ownership, dos-visible-joystick-state, joystick-handler-install-end
	ExpectContains(root / "include/joystick.h", "extern bool gameport_timed;");
	ExpectContains(joystick, "bool gameport_timed");
	ExpectBlockContains(joystick, "gameport-poll-activation", "boxer_setJoystickActive(true);");
	ExpectContains(joystick, "mapper-free-autofire");
	ExpectContains(joystick, "stick[0].is_visible_to_dos = is_visible;");
	ExpectContains(joystick, "stick[1].is_visible_to_dos = is_visible;");
	ExpectContains(joystick, "ReadHandler.Install(0x201, read_p201_switchable");
	ExpectContains(joystick, "WriteHandler.Install(0x201, write_p201_switchable");
}

TEST(BoxerIntegrationRegression, GameboxDriveFileAndMediaContractsStayBoxerManaged)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: drive-system-path, initialize-drive-system-path, retrieve-drive-system-path, fat-drive-system-path, iso-drive-system-path, local-drive-system-path, drive-cache-filter-bridge, hide-host-metadata, file-create-write-policy, file-open-write-policy, file-open-write-policy-end, file-delete-write-policy, local-dir-create-policy, local-file-created, local-file-removed, local-open-file-removed, imgmount-drive-mounted, mount-drive-mounted, drive-unmounted, invalid-fat-image-fails-construction, invalid-fat-bootsector-fails-construction, suppress-cdrom-image-error-text, file-unavailable-notification, local-file-unavailable-notification, local-file-unavailable, unavailable-file-read, unavailable-file-write, unavailable-file-seek, unavailable-file-timestamp
	ExpectBlockContains(root / "include/dos_system.h", "drive-system-path", "systempath");
	ExpectContains(root / "src/dos/drives.cpp", "DOS_Drive::DOS_Drive()");
	ExpectContains(root / "src/dos/drives.cpp", "char * DOS_Drive::getSystemPath(void)");
	ExpectContains(root / "src/dos/drive_cache.cpp", "boxer_shouldShowFileWithName(name)");
	ExpectContains(root / "src/dos/drive_local.cpp", "boxer_shouldAllowWriteAccessToPath");
	ExpectContains(root / "src/dos/drive_local.cpp", "boxer_didCreateLocalFile");
	ExpectContains(root / "src/dos/drive_local.cpp", "boxer_didRemoveLocalFile");
	ExpectBlockContains(root / "src/dos/drive_local.cpp", "local-dir-create-policy", "boxer_createLocalDir");
	ExpectContains(root / "src/dos/program_mount.cpp", "boxer_driveDidMount");
	ExpectContains(root / "src/dos/program_imgmount.cpp", "boxer_driveDidMount");
	ExpectContains(root / "src/dos/program_mount_common.cpp", "boxer_driveDidUnmount");
	ExpectContains(root / "src/dos/drive_fat.cpp", "created_successfully = false;");
	ExpectBlockContains(root / "src/dos/drive_local.cpp", "local-file-unavailable", "void localFile::willBecomeUnavailable()");
	ExpectBlockContains(root / "src/dos/drive_local.cpp", "local-file-unavailable", "fclose(fhandle);");
}

TEST(BoxerIntegrationRegression, ShellLifecycleCommandInjectionAndLaunchTrackingStayOrdered)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: current-shell-export, active-shell-global, shell-run-lifecycle, shell-misc-bridge, shell-input-injection, shell-command-filter, batch-lifecycle-bridge, batch-file-ended, program-launch-lifecycle
	ExpectContains(root / "include/shell.h", "extern DOS_Shell * currentShell;");
	ExpectContains(root / "src/shell/shell.cpp", "DOS_Shell *currentShell");
	ExpectBlockContains(root / "src/shell/shell.cpp", "shell-run-lifecycle", "boxer_shellWillStart(this);");
	ExpectBlockContains(root / "src/shell/shell.cpp", "shell-run-lifecycle", "boxer_executeNextPendingCommandForShell(this);");
	ExpectBlockContains(root / "src/shell/shell.cpp", "shell-run-lifecycle", "boxer_didReturnToShell(this);");
	ExpectBlockContains(root / "src/shell/shell.cpp", "shell-run-lifecycle", "boxer_shellDidFinish(this);");
	ExpectBlockContains(root / "src/shell/shell_misc.cpp", "shell-input-injection", "boxer_handleShellCommandInput");
	ExpectContains(root / "src/shell/shell_cmds.cpp", "boxer_shellShouldRunCommand");
	ExpectContains(root / "src/shell/shell_batch.cpp", "boxer_shellDidEndBatchFile");
	ExpectBlockContains(root / "src/shell/shell_misc.cpp", "program-launch-lifecycle", "boxer_shellWillExecuteFileAtDOSPath");
	ExpectBlockContains(root / "src/shell/shell_misc.cpp", "program-launch-lifecycle", "boxer_shellDidExecuteFileAtDOSPath");
}

TEST(BoxerIntegrationRegression, ShellCommandUxCompatibilityRemainsPresent)
{
	const auto root = ProjectRoot();
	const auto cmds = root / "src/shell/shell_cmds.cpp";
	// Protects BOXER markers: hide-intro-command, shell-command-ux, delete-help-if-no-args, delete-unix-path-tolerance, rename-help-if-no-args, mkdir-help-if-no-args, mkdir-unix-path-tolerance, rmdir-help-if-no-args, rmdir-unix-path-tolerance, dir-unix-path-trailing-slash, dir-unix-path-tolerance, copy-help-if-no-args, copy-unix-path-tolerance, if-help-if-no-args, type-help-if-no-args, call-help-if-no-args, subst-help-if-no-args, loadhigh-help-if-no-args, loadhigh-unix-path-tolerance
	ExpectContains(root / "src/dos/dos_programs.cpp", "hide-intro-command");
	ExpectBlockContains(cmds, "shell-command-ux", "\"CALL\"");
	for (const auto *marker : {"delete-help-if-no-args", "delete-unix-path-tolerance",
	                           "mkdir-help-if-no-args", "mkdir-unix-path-tolerance",
	                           "rmdir-help-if-no-args", "rmdir-unix-path-tolerance",
	                           "dir-unix-path-tolerance", "copy-help-if-no-args",
	                           "copy-unix-path-tolerance", "if-help-if-no-args",
	                           "type-help-if-no-args", "call-help-if-no-args",
	                           "subst-help-if-no-args", "loadhigh-help-if-no-args",
	                           "loadhigh-unix-path-tolerance"})
		ExpectContains(cmds, marker);
}

TEST(BoxerIntegrationRegression, PrinterAndParallelOutputStayOnBoxerBridge)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: printer-redirection, parport-skip-occupied-lpt, bios-parport-include, int17-printer-emulation, bios-parport-detection-disabled, bios-equipment-parport-count, bios-refresh-parport-count, int21-printer-output
	ExpectBlockContains(root / "src/hardware/parport/printer_redir.cpp", "printer-redirection", "#import \"BXCoalface.h\"");
	ExpectBlockContains(root / "src/hardware/parport/printer_redir.cpp", "printer-redirection", "boxer_PRINTER_isInited");
	ExpectBlockContains(root / "src/hardware/parport/printer_redir.cpp", "printer-redirection", "boxer_PRINTER_writedata");
	ExpectContains(root / "src/hardware/parport/parport.cpp", "parport-skip-occupied-lpt");
	ExpectContains(root / "src/ints/bios.cpp", "parallelPortObjects[reg_dx]->Putchar(reg_al)");
	ExpectContains(root / "src/ints/bios.cpp", "parallelPortObjects[reg_dx]->getPrinterStatus()");
	ExpectContains(root / "src/dos/dos.cpp", "parallelPortObjects[i]->Putchar(reg_dl);");
}

TEST(BoxerIntegrationRegression, LocalizationRoutesThroughBoxerResources)
{
	const auto root = ProjectRoot();
	const auto messages = root / "src/misc/messages.cpp";
	// Protects BOXER markers: localization-routing, upstream-localization-disabled
	ExpectBlockContains(messages, "localization-routing", "#include \"BXCoalface.h\"");
	ExpectBlockContains(messages, "localization-routing", "return boxer_localizedStringForKey(msg);");
	ExpectContains(messages, "upstream-localization-disabled");
	ExpectContains(messages, "#if 0");
}

} // namespace
