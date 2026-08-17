#include "boxer_regression_contracts.h"

namespace {

using namespace boxer_regression;

TEST(BoxerMergeSmoke, MarkerInventoryIsStillConsistent)
{
	// Protects smoke coverage for all BOXER markers documented in BOXER_PATCHES.md.
	EXPECT_EQ(DocumentedMarkerSet(), SourceMarkerSet());
}

TEST(BoxerMergeSmoke, CoreInitializationAndBridgeOwnershipSurvive)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: coalface-remaps, dosbox-parport-init, boxer-mt32-config-include, mt32-config-section, parallel-config-section
	ExpectBlockContains(root / "include/dosbox.h", "coalface-remaps", "#include \"BXCoalface.h\"");
	ExpectContains(root.parent_path() / "Boxer/BXCoalface.h", "#define GFX_Events boxer_processEvents");
	ExpectContains(root / "src/dosbox.cpp", "#include \"BXMIDIConfig.hpp\"");
	ExpectContains(root / "src/dosbox.cpp", "#include \"parport.h\"");
	ExpectContains(root / "src/dosbox.cpp", "BXMIDIMT32_AddConfigSection(control);");
	ExpectContains(root / "src/dosbox.cpp", "parallel1");
}

TEST(BoxerMergeSmoke, InputDriveAndShellBoundariesSurvive)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: keyboard-buffer-capacity, console-read-cancel, gameport-poll-activation, preserve-controller-ownership, drive-system-path, local-drive-system-path, mount-drive-mounted, drive-unmounted, shell-run-lifecycle, shell-input-injection
	ExpectContains(root / "src/hardware/keyboard.cpp", "boxer_keyboardBufferRemaining");
	ExpectContains(root / "src/dos/dev_con.h", "boxer_continueListeningForKeyEvents()");
	ExpectBlockContains(root / "src/hardware/joystick.cpp", "gameport-poll-activation", "boxer_setJoystickActive(true);");
	ExpectBlockContains(root / "include/dos_system.h", "drive-system-path", "system_path");
	ExpectContains(root / "src/dos/drive_local.cpp", "boxer_shouldAllowWriteAccessToPath");
	ExpectContains(root / "src/dos/program_mount.cpp", "boxer_driveDidMount");
	ExpectContains(root / "src/dos/program_mount_common.cpp", "boxer_driveDidUnmount");
	ExpectBlockContains(root / "src/shell/shell.cpp", "shell-run-lifecycle", "boxer_shellWillStart(this);");
	ExpectBlockContains(root / "src/shell/shell_misc.cpp", "shell-input-injection", "boxer_handleShellCommandInput");
}

TEST(BoxerMergeSmoke, AudioMidiVideoAndPrinterBoundariesSurvive)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: mixer-volume-bridge, midi-routing, render-reset-strategy, capture-file-routing, printer-redirection, localization-routing
	ExpectBlockContains(root / "src/hardware/mixer.cpp", "mixer-volume-bridge", "boxer_updateVolumes");
	ExpectBlockContains(root / "src/hardware/mixer.cpp", "mixer-volume-bridge", "boxer_masterVolume(BXLeftChannel)");
	ExpectBlockContains(root / "src/midi/midi.cpp", "midi-routing", "boxer_sendMIDIMessage");
	ExpectBlockContains(root / "src/midi/midi.cpp", "midi-routing", "boxer_sendMIDISysex");
	ExpectBlockContains(root / "src/gui/render.cpp", "render-reset-strategy", "boxer_applyRenderingStrategy();");
	ExpectBlockContains(root / "src/hardware/hardware.cpp", "capture-file-routing", "#if 0");
	ExpectBlockContains(root / "src/hardware/parport/printer_redir.cpp", "printer-redirection", "boxer_PRINTER_writedata");
	ExpectBlockContains(root / "src/misc/messages.cpp", "localization-routing", "boxer_localizedStringForKey");
}

TEST(BoxerMergeSmoke, TerminationShutdownAndReinitializationHooksSurvive)
{
	const auto root = ProjectRoot();
	// Protects BOXER markers: runloop-termination, runloop-event-cancellation, runloop-context, shutdown-drive-clear
	ExpectContains(root / "src/dosbox.cpp", "boxer_runLoopShouldContinue()");
	ExpectBlockContains(root / "src/dosbox.cpp", "runloop-context", "boxer_runLoopWillStartWithContextInfo(&contextInfo);");
	ExpectBlockContains(root / "src/dosbox.cpp", "runloop-context", "boxer_runLoopDidFinishWithContextInfo(contextInfo);");
	ExpectContains(root / "src/dos/dos.cpp", "delete Drives[i];");
	ExpectContains(root / "src/dos/dos.cpp", "Drives[i] = 0;");
}

} // namespace
