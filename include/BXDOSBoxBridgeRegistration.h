/*
 * Narrow callback boundary between DOSBox and the Boxer host.
 * This header contains no Objective-C or DOSBox implementation types.
 */

#ifndef BXDOSBoxBridgeRegistration_h
#define BXDOSBoxBridgeRegistration_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum BXDOSBoxAudioChannel {
    BXDOSBoxAudioChannelLeft,
    BXDOSBoxAudioChannelRight
} BXDOSBoxAudioChannel;

typedef struct BXDOSBoxAudioBridgeCallbacks {
    bool (*midiAvailable)(void);
    void (*sendMIDIMessage)(uint8_t *message);
    void (*sendMIDISysex)(uint8_t *message, size_t length);
    float (*masterVolume)(BXDOSBoxAudioChannel channel);
    void (*updateVolumes)(void);
    void (*suggestMIDIHandler)(const char *handlerName, const char *configParams);
} BXDOSBoxAudioBridgeCallbacks;

typedef struct BXDOSBoxPrinterBridgeCallbacks {
    bool (*isInitialized)(uintptr_t port);
    uintptr_t (*readData)(uintptr_t port, uintptr_t width);
    uintptr_t (*readControl)(uintptr_t port, uintptr_t width);
    uintptr_t (*readStatus)(uintptr_t port, uintptr_t width);
    void (*writeData)(uintptr_t port, uintptr_t value, uintptr_t width);
    void (*writeControl)(uintptr_t port, uintptr_t value, uintptr_t width);
} BXDOSBoxPrinterBridgeCallbacks;

typedef struct BXDOSBoxInputBridgeCallbacks {
    uintptr_t (*numKeyCodesInPasteBuffer)(void);
    bool (*continueListeningForKeyEvents)(void);
    bool (*getNextKeyCodeInPasteBuffer)(uint16_t *outKeyCode, bool consumeKey);
    const char *(*preferredKeyboardLayout)(void);
    void (*setCapsLockActive)(bool active);
    void (*setNumLockActive)(bool active);
    void (*setScrollLockActive)(bool active);
    void (*switchKeyboardLayout)(const char *layout, int32_t codepage);
    void (*mouseMoved)(float deltaX, float deltaY, int32_t absoluteX, int32_t absoluteY);
    void (*mouseButton)(uint8_t button, bool pressed);
} BXDOSBoxInputBridgeCallbacks;

typedef struct BXDOSBoxJoystickBridgeCallbacks {
    void (*setActive)(bool active);
    void (*enable)(uint8_t which, bool enabled);
    void (*setButton)(uint8_t which, uint8_t button, bool pressed);
    void (*moveX)(uint8_t which, int16_t value);
    void (*moveY)(uint8_t which, int16_t value);
    bool (*isAccessible)(uint8_t which);
    bool (*getButton)(uint8_t which, uint8_t button);
    double (*getMoveX)(uint8_t which);
    double (*getMoveY)(uint8_t which);
    uint8_t (*gameportTimingMode)(void);
    void (*setGameportTimingMode)(uint8_t mode);
    uint8_t (*joystickSupportLevel)(void);
} BXDOSBoxJoystickBridgeCallbacks;

typedef struct BXDOSBoxShellBridgeCallbacks {
    void (*willStart)(uintptr_t shell);
    void (*didFinish)(uintptr_t shell);
    bool (*shouldContinue)(uintptr_t shell);
    bool (*shouldRunCommand)(uintptr_t shell, char *command, char *arguments);
    bool (*handleCommandInput)(uintptr_t shell, char *line, uintptr_t *cursorPosition, bool *executeImmediately);
    bool (*hasPendingCommands)(uintptr_t shell);
    bool (*executeNextPendingCommand)(uintptr_t shell);
    void (*didReturnToShell)(uintptr_t shell);
    void (*willStartAutoexec)(uintptr_t shell);
    bool (*shouldDisplayStartupMessages)(uintptr_t shell);
    void (*willReadCommandInputFromHandle)(uintptr_t shell, uint16_t handle);
    void (*didReadCommandInputFromHandle)(uintptr_t shell, uint16_t handle);
    void (*willExecuteFileAtDOSPath)(uintptr_t shell, const char *path, const char *arguments);
    void (*didExecuteFileAtDOSPath)(uintptr_t shell, const char *path);
    void (*willBeginBatchFile)(uintptr_t shell, const char *path, const char *arguments);
    void (*didEndBatchFile)(uintptr_t shell, const char *path);
} BXDOSBoxShellBridgeCallbacks;

typedef struct BXDOSBoxShellControlBridgeCallbacks {
    uintptr_t (*currentShell)(void);
    void (*writeOutput)(uintptr_t shell, const char *text);
    bool (*echoEnabled)(uintptr_t shell);
    bool (*hasNoBatchFiles)(uintptr_t shell);
    void (*showPrompt)(uintptr_t shell);
    void (*parseLine)(uintptr_t shell, char *line);
    void (*doCommand)(uintptr_t shell, char *command);
    bool (*changeDirectory)(const char *path);
    bool (*setDrive)(uint8_t driveIndex);
    uintptr_t (*maximumCommandLineLength)(void);
} BXDOSBoxShellControlBridgeCallbacks;

typedef struct BXDOSBoxFilesystemBridgeCallbacks {
    bool (*shouldShowFileWithName)(const char *name);
    bool (*shouldAllowWriteAccessToPath)(const char *path, uintptr_t drive);
    void (*didCreateLocalFile)(const char *path, uintptr_t drive);
    void (*didRemoveLocalFile)(const char *path, uintptr_t drive);
    bool (*createLocalDir)(const char *path, uintptr_t drive);
    void (*driveDidMount)(uint8_t driveIndex);
    void (*driveDidUnmount)(uint8_t driveIndex);
} BXDOSBoxFilesystemBridgeCallbacks;

/* Host-facing filesystem queries. DOSBox drive objects remain opaque to Boxer. */
typedef struct BXDOSBoxFilesystemControlBridgeCallbacks {
    uintptr_t (*driveCount)(void);
    uint8_t (*defaultDriveIndex)(void);
    uintptr_t (*maximumPathLength)(void);
    bool (*makeName)(const char *dosPath, char *outPath, uintptr_t outPathLength,
                     uint8_t *outDriveIndex);
    uintptr_t (*driveAtIndex)(uint8_t driveIndex);
    uint8_t (*driveType)(uintptr_t drive);
    const char *(*driveSystemPath)(uintptr_t drive);
    const char *(*driveCurrentDirectory)(uintptr_t drive);
    bool (*driveFileExists)(uintptr_t drive, const char *path);
    bool (*driveTestDirectory)(uintptr_t drive, const char *path);
    uintptr_t (*maximumShortNameLength)(void);
    bool (*driveGetShortName)(uintptr_t drive, const char *directory, const char *name,
                              char *outName, uintptr_t outNameLength);
    void (*emptyDriveCache)(uintptr_t drive);
    bool (*addDrive)(uintptr_t drive, uint8_t driveIndex, uint8_t mediaByte);
    int32_t (*unmountDrive)(uint8_t driveIndex);
    void (*closeFilesForDrive)(uint8_t driveIndex);
    void (*destroyDrive)(uintptr_t drive);
    void (*setDriveLabel)(uintptr_t drive, const char *label, bool isCDROM);
    const char *(*driveLabel)(uintptr_t drive);
    uintptr_t (*createCDROMImage)(const char *path, uint8_t driveIndex, uint8_t mediaByte,
                                  int32_t *outErrorCode);
    uintptr_t (*createFloppyImage)(const char *path, int32_t *outErrorCode);
    uintptr_t (*createCDROMDirectory)(const char *path, uint8_t driveIndex,
                                      bool useAudio, const uint16_t *geometry, uint8_t mediaByte,
                                      int32_t *outErrorCode);
    uintptr_t (*createLocalDirectory)(const char *path, const char *shadowPath,
                                      int64_t freeSpace, const uint16_t *geometry,
                                      uint8_t mediaByte, int32_t *outErrorCode);
    uintptr_t (*createFloppyDirectory)(const char *path, int64_t freeSpace,
                                       const uint16_t *geometry, uint8_t mediaByte,
                                       int32_t *outErrorCode);
} BXDOSBoxFilesystemControlBridgeCallbacks;

typedef struct BXDOSBoxRunLoopBridgeCallbacks {
    bool (*shouldContinue)(void);
    void (*willStartWithContextInfo)(void **contextInfo);
    void (*didFinishWithContextInfo)(void *contextInfo);
} BXDOSBoxRunLoopBridgeCallbacks;

typedef struct BXDOSBoxConfigurationBridgeCallbacks {
    uintptr_t (*create)(void);
    void (*initializeConfigDirectory)(void);
    void (*initializeDOSBox)(void);
    void (*parseConfigFile)(uintptr_t configuration, const char *path);
    void (*initialize)(uintptr_t configuration);
    void (*startUp)(uintptr_t configuration);
    void (*destroy)(uintptr_t configuration);
    bool (*shutdownRequested)(void);
} BXDOSBoxConfigurationBridgeCallbacks;

typedef struct BXDOSBoxEventBridgeCallbacks {
    bool (*processEvents)(void);
    void (*handleTitleChange)(int32_t cycles, bool paused);
    void (*refreshTitle)(void);
    void (*setMouseHint)(uintptr_t hint);
    void (*setMouseCapture)(bool captured);
    void (*setMouseRawInput)(bool rawInput);
    void (*setMouseVisibility)(bool visible);
    bool (*hasDesktopEnvironment)(void);
    void (*centerMouse)(void);
} BXDOSBoxEventBridgeCallbacks;

typedef struct BXDOSBoxRenderingBridgeCallbacks {
    void (*applyRenderingStrategy)(void);
    uintptr_t (*prepareForFrameSize)(uintptr_t width, uintptr_t height, uintptr_t flags,
                                     double scaleX, double scaleY, uintptr_t callback,
                                     double pixelAspect);
    uintptr_t (*idealOutputMode)(uintptr_t flags);
    bool (*startFrame)(uint8_t **pixels, int *pitch);
    void (*finishFrame)(const uint16_t *dirtyBlocks);
    uintptr_t (*getRGBPaletteEntry)(uint8_t red, uint8_t green, uint8_t blue);
    void (*setShader)(const char *source);
    uint8_t (*herculesTintMode)(void);
    void (*setHerculesTintMode)(uint8_t mode);
    double (*cgaCompositeHueOffset)(void);
    void (*setCGACompositeHueOffset)(double offset);
    uint8_t (*cgaComponentMode)(void);
    void (*setCGAComponentMode)(uint8_t mode);
    int (*getDisplayRefreshRate)(void);
    uintptr_t (*currentVideoWidth)(void);
    uintptr_t (*currentVideoHeight)(void);
    bool (*currentVideoModeIsText)(void);
    bool (*currentMachineIsHercules)(void);
    bool (*currentMachineIsCGA)(void);
    void (*setRenderScale)(uintptr_t scale);
} BXDOSBoxRenderingBridgeCallbacks;

typedef struct BXDOSBoxDiagnosticsBridgeCallbacks {
    const char *(*localizedStringForKey)(const char *key);
    void (*logMessage)(const char *message);
    void (*fatalError)(const char *functionName, const char *fileName, int lineNumber,
                       const char *message);
} BXDOSBoxDiagnosticsBridgeCallbacks;

void boxer_registerDOSBoxAudioBridge(const BXDOSBoxAudioBridgeCallbacks *callbacks);
/*
 * Registration copies the callback table by value. Passing NULL is ignored;
 * it does not unregister an existing table. Accessors lazily install the
 * Boxer host defaults on first use, and the copied function pointers must
 * remain callable for the lifetime of the DOSBox session.
 */
#if defined(BXDOSBOX_PRODUCTION) || defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void);
void boxer_registerDOSBoxPrinterBridge(const BXDOSBoxPrinterBridgeCallbacks *callbacks);
const BXDOSBoxPrinterBridgeCallbacks *boxer_registeredDOSBoxPrinterBridge(void);
void boxer_registerDOSBoxInputBridge(const BXDOSBoxInputBridgeCallbacks *callbacks);
const BXDOSBoxInputBridgeCallbacks *boxer_registeredDOSBoxInputBridge(void);
void boxer_registerDOSBoxJoystickBridge(const BXDOSBoxJoystickBridgeCallbacks *callbacks);
const BXDOSBoxJoystickBridgeCallbacks *boxer_registeredDOSBoxJoystickBridge(void);
void boxer_registerDOSBoxShellBridge(const BXDOSBoxShellBridgeCallbacks *callbacks);
const BXDOSBoxShellBridgeCallbacks *boxer_registeredDOSBoxShellBridge(void);
void boxer_registerDOSBoxShellControlBridge(const BXDOSBoxShellControlBridgeCallbacks *callbacks);
const BXDOSBoxShellControlBridgeCallbacks *boxer_registeredDOSBoxShellControlBridge(void);
void boxer_registerDOSBoxFilesystemBridge(const BXDOSBoxFilesystemBridgeCallbacks *callbacks);
const BXDOSBoxFilesystemBridgeCallbacks *boxer_registeredDOSBoxFilesystemBridge(void);
void boxer_registerDOSBoxFilesystemControlBridge(const BXDOSBoxFilesystemControlBridgeCallbacks *callbacks);
const BXDOSBoxFilesystemControlBridgeCallbacks *boxer_registeredDOSBoxFilesystemControlBridge(void);
void boxer_registerDOSBoxRunLoopBridge(const BXDOSBoxRunLoopBridgeCallbacks *callbacks);
const BXDOSBoxRunLoopBridgeCallbacks *boxer_registeredDOSBoxRunLoopBridge(void);
void boxer_registerDOSBoxConfigurationBridge(const BXDOSBoxConfigurationBridgeCallbacks *callbacks);
const BXDOSBoxConfigurationBridgeCallbacks *boxer_registeredDOSBoxConfigurationBridge(void);
void boxer_registerDOSBoxEventBridge(const BXDOSBoxEventBridgeCallbacks *callbacks);
const BXDOSBoxEventBridgeCallbacks *boxer_registeredDOSBoxEventBridge(void);
void boxer_registerDOSBoxRenderingBridge(const BXDOSBoxRenderingBridgeCallbacks *callbacks);
const BXDOSBoxRenderingBridgeCallbacks *boxer_registeredDOSBoxRenderingBridge(void);
void boxer_registerDOSBoxDiagnosticsBridge(const BXDOSBoxDiagnosticsBridgeCallbacks *callbacks);
const BXDOSBoxDiagnosticsBridgeCallbacks *boxer_registeredDOSBoxDiagnosticsBridge(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BXDOSBoxBridgeRegistration_h */
