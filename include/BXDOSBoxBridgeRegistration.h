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
#include <string>
#endif

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
    void (*setCapsLockActive)(bool active);
    void (*setNumLockActive)(bool active);
    void (*setScrollLockActive)(bool active);
} BXDOSBoxInputBridgeCallbacks;

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

typedef struct BXDOSBoxFilesystemBridgeCallbacks {
    bool (*shouldShowFileWithName)(const char *name);
    bool (*shouldAllowWriteAccessToPath)(const char *path, uintptr_t drive);
    void (*didCreateLocalFile)(const char *path, uintptr_t drive);
    void (*didRemoveLocalFile)(const char *path, uintptr_t drive);
    bool (*createLocalDir)(const char *path, uintptr_t drive);
    void (*driveDidMount)(uint8_t driveIndex);
    void (*driveDidUnmount)(uint8_t driveIndex);
} BXDOSBoxFilesystemBridgeCallbacks;

typedef struct BXDOSBoxRunLoopBridgeCallbacks {
    bool (*shouldContinue)(void);
    void (*willStartWithContextInfo)(void **contextInfo);
    void (*didFinishWithContextInfo)(void *contextInfo);
} BXDOSBoxRunLoopBridgeCallbacks;

void boxer_registerDOSBoxAudioBridge(const BXDOSBoxAudioBridgeCallbacks *callbacks);
#if defined(BXDOSBOX_PRODUCTION) || defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void);
void boxer_registerDOSBoxPrinterBridge(const BXDOSBoxPrinterBridgeCallbacks *callbacks);
const BXDOSBoxPrinterBridgeCallbacks *boxer_registeredDOSBoxPrinterBridge(void);
void boxer_registerDOSBoxInputBridge(const BXDOSBoxInputBridgeCallbacks *callbacks);
const BXDOSBoxInputBridgeCallbacks *boxer_registeredDOSBoxInputBridge(void);
void boxer_registerDOSBoxShellBridge(const BXDOSBoxShellBridgeCallbacks *callbacks);
const BXDOSBoxShellBridgeCallbacks *boxer_registeredDOSBoxShellBridge(void);
void boxer_registerDOSBoxFilesystemBridge(const BXDOSBoxFilesystemBridgeCallbacks *callbacks);
const BXDOSBoxFilesystemBridgeCallbacks *boxer_registeredDOSBoxFilesystemBridge(void);
void boxer_registerDOSBoxRunLoopBridge(const BXDOSBoxRunLoopBridgeCallbacks *callbacks);
const BXDOSBoxRunLoopBridgeCallbacks *boxer_registeredDOSBoxRunLoopBridge(void);
#endif

#ifdef __cplusplus
}

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
/*
 * Compatibility fallback for DOSBox-only harnesses. The production Boxer
 * target supplies the registered table from BXCoalfaceAudio.mm; lightweight
 * runtime harnesses can continue providing the legacy symbols directly.
 */
#define BXDOSBOX_WEAK __attribute__((weak))
bool boxer_MIDIAvailable(void) BXDOSBOX_WEAK;
void boxer_sendMIDIMessage(uint8_t *message) BXDOSBOX_WEAK;
void boxer_sendMIDISysex(uint8_t *message, size_t length) BXDOSBOX_WEAK;
float boxer_masterVolume(BXDOSBoxAudioChannel channel) BXDOSBOX_WEAK;
void boxer_updateVolumes(void) BXDOSBOX_WEAK;
void boxer_suggestMIDIHandler(std::string const &handlerName, const char *configParams) BXDOSBOX_WEAK;
extern "C" {
bool boxer_PRINTER_isInited(uintptr_t port) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readdata(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readcontrol(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readstatus(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
void boxer_PRINTER_writedata(uintptr_t port, uintptr_t value, uintptr_t width) BXDOSBOX_WEAK;
void boxer_PRINTER_writecontrol(uintptr_t port, uintptr_t value, uintptr_t width) BXDOSBOX_WEAK;
uintptr_t boxer_numKeyCodesInPasteBuffer(void) BXDOSBOX_WEAK;
bool boxer_continueListeningForKeyEvents(void) BXDOSBOX_WEAK;
bool boxer_getNextKeyCodeInPasteBuffer(uint16_t *outKeyCode, bool consumeKey) BXDOSBOX_WEAK;
void boxer_setCapsLockActive(bool active) BXDOSBOX_WEAK;
void boxer_setNumLockActive(bool active) BXDOSBOX_WEAK;
void boxer_setScrollLockActive(bool active) BXDOSBOX_WEAK;
}

static inline void boxer_bridgeSuggestMIDIHandler(const char *handlerName, const char *configParams)
{
    boxer_suggestMIDIHandler(std::string(handlerName), configParams);
}

static inline const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void)
{
#if defined(BXDOSBOX_MIXER_COMPATIBILITY)
    static const BXDOSBoxAudioBridgeCallbacks callbacks = {
        NULL,
        NULL,
        NULL,
        boxer_masterVolume,
        NULL,
        NULL
    };
#elif defined(BXDOSBOX_MIDI_COMPATIBILITY)
    static const BXDOSBoxAudioBridgeCallbacks callbacks = {
        boxer_MIDIAvailable,
        boxer_sendMIDIMessage,
        boxer_sendMIDISysex,
        NULL,
        NULL,
        boxer_bridgeSuggestMIDIHandler
    };
#else
    static const BXDOSBoxAudioBridgeCallbacks callbacks = {
        boxer_MIDIAvailable,
        boxer_sendMIDIMessage,
        boxer_sendMIDISysex,
        boxer_masterVolume,
        boxer_updateVolumes,
        boxer_bridgeSuggestMIDIHandler
    };
#endif
    return &callbacks;
}

static inline const BXDOSBoxPrinterBridgeCallbacks *boxer_registeredDOSBoxPrinterBridge(void)
{
    static const BXDOSBoxPrinterBridgeCallbacks callbacks = {
        boxer_PRINTER_isInited,
        boxer_PRINTER_readdata,
        boxer_PRINTER_readcontrol,
        boxer_PRINTER_readstatus,
        boxer_PRINTER_writedata,
        boxer_PRINTER_writecontrol
    };
    return &callbacks;
}

static inline const BXDOSBoxInputBridgeCallbacks *boxer_registeredDOSBoxInputBridge(void)
{
    static const BXDOSBoxInputBridgeCallbacks callbacks = {
        boxer_numKeyCodesInPasteBuffer,
        boxer_continueListeningForKeyEvents,
        boxer_getNextKeyCodeInPasteBuffer,
        boxer_setCapsLockActive,
        boxer_setNumLockActive,
        boxer_setScrollLockActive
    };
    return &callbacks;
}
#endif

#if defined(BXDOSBOX_PRINTER_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_PRINTER_isInited(port) boxer_registeredDOSBoxPrinterBridge()->isInitialized(port)
#define boxer_PRINTER_readdata(port, width) boxer_registeredDOSBoxPrinterBridge()->readData(port, width)
#define boxer_PRINTER_readcontrol(port, width) boxer_registeredDOSBoxPrinterBridge()->readControl(port, width)
#define boxer_PRINTER_readstatus(port, width) boxer_registeredDOSBoxPrinterBridge()->readStatus(port, width)
#define boxer_PRINTER_writedata(port, value, width) boxer_registeredDOSBoxPrinterBridge()->writeData(port, value, width)
#define boxer_PRINTER_writecontrol(port, value, width) boxer_registeredDOSBoxPrinterBridge()->writeControl(port, value, width)
#endif

#if defined(BXDOSBOX_INPUT_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_numKeyCodesInPasteBuffer() boxer_registeredDOSBoxInputBridge()->numKeyCodesInPasteBuffer()
#define boxer_continueListeningForKeyEvents() boxer_registeredDOSBoxInputBridge()->continueListeningForKeyEvents()
#define boxer_getNextKeyCodeInPasteBuffer(outKeyCode, consumeKey) boxer_registeredDOSBoxInputBridge()->getNextKeyCodeInPasteBuffer(outKeyCode, consumeKey)
#define boxer_setCapsLockActive(active) boxer_registeredDOSBoxInputBridge()->setCapsLockActive(active)
#define boxer_setNumLockActive(active) boxer_registeredDOSBoxInputBridge()->setNumLockActive(active)
#define boxer_setScrollLockActive(active) boxer_registeredDOSBoxInputBridge()->setScrollLockActive(active)
#endif
#endif

#endif /* BXDOSBoxBridgeRegistration_h */
