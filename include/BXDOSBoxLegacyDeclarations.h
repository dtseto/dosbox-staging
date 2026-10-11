/*
 * DOSBox-side declarations for the legacy Boxer symbols still referenced by
 * compatibility adapters and standalone harnesses.
 *
 * This is intentionally not part of BXDOSBoxBridgeRegistration.h. The
 * registration header contains only opaque callback contracts; this header is
 * the DOSBox-only declaration surface for the existing C-linkage symbols.
 */

#ifndef BXDOSBoxLegacyDeclarations_h
#define BXDOSBoxLegacyDeclarations_h

#if defined(BXDOSBOX_PRODUCTION) || defined(BXDOSBOX_FORCE_LEGACY_COMPATIBILITY) || defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)

#include "config.h"
#include "types.h"

#include <cstdint>
#include <cstdio>

class DOS_Drive;
class DOS_Shell;

/*
 * video.h defines these later in the normal include graph.  Forward-declare
 * the callback contract here so this legacy declaration header does not
 * recursively include video.h through dosbox.h.
 */
enum GFX_CallBackFunctions_t : int;
typedef void (*GFX_CallBack_t)(GFX_CallBackFunctions_t function);

/* This audio hook has a C++ host definition, unlike the older C ABI hooks. */
bool boxer_MIDIAvailable(void);
#define MIDI_Available boxer_MIDIAvailable

#ifdef __cplusplus
extern "C" {
#endif

/* Rendering and frame capture. */
Bitu boxer_prepareForFrameSize(Bitu width, Bitu height, Bitu gfx_flags,
                               double scalex, double scaley,
                               GFX_CallBack_t callback, double pixel_aspect);
void boxer_getCanvasSizeInPixels(int *width, int *height);
bool boxer_startFrame(uint8_t *&frameBuffer, int &pitch);
void boxer_finishFrame(const uint16_t *dirtyBlocks);
Bitu boxer_idealOutputMode(Bitu flags);
void boxer_applyRenderingStrategy(void);
Bitu boxer_getRGBPaletteEntry(uint8_t red, uint8_t green, uint8_t blue);
void boxer_setShader(const char *source);
FILE *boxer_openCaptureFile(const char *typeDescription, const char *fileExtension);

#define OpenCaptureFile boxer_openCaptureFile

/* DOSBox implementations called by the host through the legacy ABI. */
uint8_t boxer_herculesTintMode(void);
void boxer_setHerculesTintMode(uint8_t tint);
double boxer_CGACompositeHueOffset(void);
void boxer_setCGACompositeHueOffset(double hue);
uint8_t boxer_CGAComponentMode(void);
void boxer_setCGAComponentMode(uint8_t mode);
int boxer_GetDisplayRefreshRate(void);
Bitu boxer_keyboardBufferRemaining(void);
bool boxer_keyboardLayoutLoaded(void);
const char *boxer_keyboardLayoutName(void);
bool boxer_keyboardLayoutSupported(const char *code);
bool boxer_keyboardLayoutActive(void);
void boxer_setKeyboardLayoutActive(bool active);

/* Shell callbacks retain DOS_Shell in the legacy ABI. */
void boxer_shellWillStart(DOS_Shell *shell);
void boxer_shellDidFinish(DOS_Shell *shell);
bool boxer_shellShouldContinue(DOS_Shell *shell);
bool boxer_shellShouldRunCommand(DOS_Shell *shell, char *command, char *arguments);
bool boxer_handleShellCommandInput(DOS_Shell *shell, char *line,
                                   Bitu *cursorPosition, bool *executeImmediately);
bool boxer_hasPendingCommandsForShell(DOS_Shell *shell);
bool boxer_executeNextPendingCommandForShell(DOS_Shell *shell);
void boxer_didReturnToShell(DOS_Shell *shell);
void boxer_shellWillStartAutoexec(DOS_Shell *shell);
bool boxer_shellShouldDisplayStartupMessages(DOS_Shell *shell);
void boxer_shellWillReadCommandInputFromHandle(DOS_Shell *shell, uint16_t handle);
void boxer_shellDidReadCommandInputFromHandle(DOS_Shell *shell, uint16_t handle);
void boxer_shellWillExecuteFileAtDOSPath(DOS_Shell *shell, const char *path,
                                         const char *arguments);
void boxer_shellDidExecuteFileAtDOSPath(DOS_Shell *shell, const char *path);
void boxer_shellWillBeginBatchFile(DOS_Shell *shell, const char *path,
                                   const char *arguments);
void boxer_shellDidEndBatchFile(DOS_Shell *shell, const char *path);

/* Run-loop and diagnostics callbacks. */
bool boxer_runLoopShouldContinue(void);
void boxer_runLoopWillStartWithContextInfo(void **contextInfo);
void boxer_runLoopDidFinishWithContextInfo(void *contextInfo);
const char *boxer_localizedStringForKey(const char *key);

/* Remaining legacy callbacks used by DOSBox compatibility adapters. */
void boxer_setJoystickActive(bool active);
bool boxer_continueListeningForKeyEvents(void);
bool boxer_shouldShowFileWithName(const char *name);
bool boxer_shouldAllowWriteAccessToPath(const char *path, DOS_Drive *drive);
void boxer_driveDidMount(uint8_t driveIndex);
void boxer_driveDidUnmount(uint8_t driveIndex);
bool boxer_createLocalDir(const char *path, DOS_Drive *drive);
void boxer_didCreateLocalFile(const char *path, DOS_Drive *drive);
void boxer_didRemoveLocalFile(const char *path, DOS_Drive *drive);

#ifdef __cplusplus
}
#endif

#endif /* production or host compatibility build */

#endif /* BXDOSBoxLegacyDeclarations_h */
