/*
 * DOSBox-only compatibility declarations for extracted Boxer harnesses.
 *
 * This header deliberately sits outside BXDOSBoxBridgeRegistration.h. The
 * registration header is the public opaque callback contract; this file owns
 * weak legacy symbols, harness fallback tables, and call-site remaps needed
 * by DOSBox source and standalone contract tests.
 */

#ifndef BXDOSBoxLegacyCompatibility_h
#define BXDOSBoxLegacyCompatibility_h

#include "BXDOSBoxBridgeRegistration.h"

#ifdef __cplusplus
#include <string>

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)

#define BXDOSBOX_WEAK __attribute__((weak))

bool boxer_MIDIAvailable(void) BXDOSBOX_WEAK;
void boxer_sendMIDIMessage(uint8_t *message) BXDOSBOX_WEAK;
void boxer_sendMIDISysex(uint8_t *message, size_t length) BXDOSBOX_WEAK;
float boxer_masterVolume(BXDOSBoxAudioChannel channel) BXDOSBOX_WEAK;
void boxer_updateVolumes(void) BXDOSBOX_WEAK;
void boxer_suggestMIDIHandler(std::string const &handlerName,
                              const char *configParams) BXDOSBOX_WEAK;

/* These weak declarations intentionally use C++ linkage so standalone
 * harnesses can define their legacy fallbacks directly in the harness TU. */
bool boxer_PRINTER_isInited(uintptr_t port) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readdata(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readcontrol(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
uintptr_t boxer_PRINTER_readstatus(uintptr_t port, uintptr_t width) BXDOSBOX_WEAK;
void boxer_PRINTER_writedata(uintptr_t port, uintptr_t value, uintptr_t width) BXDOSBOX_WEAK;
void boxer_PRINTER_writecontrol(uintptr_t port, uintptr_t value, uintptr_t width) BXDOSBOX_WEAK;
void boxer_setJoystickActive(bool active) BXDOSBOX_WEAK;

/* Standalone harnesses provide these C++-linkage callbacks directly. */
uintptr_t boxer_numKeyCodesInPasteBuffer(void) BXDOSBOX_WEAK;
bool boxer_continueListeningForKeyEvents(void) BXDOSBOX_WEAK;
bool boxer_getNextKeyCodeInPasteBuffer(uint16_t *outKeyCode, bool consumeKey) BXDOSBOX_WEAK;
const char *boxer_preferredKeyboardLayout(void) BXDOSBOX_WEAK;
void boxer_setCapsLockActive(bool active) BXDOSBOX_WEAK;
void boxer_setNumLockActive(bool active) BXDOSBOX_WEAK;
void boxer_setScrollLockActive(bool active) BXDOSBOX_WEAK;

static inline void boxer_bridgeSuggestMIDIHandler(const char *handlerName,
                                                   const char *configParams)
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
        boxer_PRINTER_writecontrol,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxInputBridgeCallbacks *boxer_registeredDOSBoxInputBridge(void)
{
    static const BXDOSBoxInputBridgeCallbacks callbacks = {
        boxer_numKeyCodesInPasteBuffer,
        boxer_continueListeningForKeyEvents,
        boxer_getNextKeyCodeInPasteBuffer,
        boxer_preferredKeyboardLayout,
        boxer_setCapsLockActive,
        boxer_setNumLockActive,
        boxer_setScrollLockActive,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxJoystickBridgeCallbacks *boxer_registeredDOSBoxJoystickBridge(void)
{
    static const BXDOSBoxJoystickBridgeCallbacks callbacks = {
        boxer_setJoystickActive,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxConfigurationBridgeCallbacks *boxer_registeredDOSBoxConfigurationBridge(void)
{
    static const BXDOSBoxConfigurationBridgeCallbacks callbacks = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxCPUControlBridgeCallbacks *boxer_registeredDOSBoxCPUControlBridge(void)
{
    static const BXDOSBoxCPUControlBridgeCallbacks callbacks = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxShellControlBridgeCallbacks *boxer_registeredDOSBoxShellControlBridge(void)
{
    static const BXDOSBoxShellControlBridgeCallbacks callbacks = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };
    return &callbacks;
}

static inline const BXDOSBoxFilesystemControlBridgeCallbacks *boxer_registeredDOSBoxFilesystemControlBridge(void)
{
    static const BXDOSBoxFilesystemControlBridgeCallbacks callbacks = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
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
#define boxer_preferredKeyboardLayout() boxer_registeredDOSBoxInputBridge()->preferredKeyboardLayout()
#define boxer_setCapsLockActive(active) boxer_registeredDOSBoxInputBridge()->setCapsLockActive(active)
#define boxer_setNumLockActive(active) boxer_registeredDOSBoxInputBridge()->setNumLockActive(active)
#define boxer_setScrollLockActive(active) boxer_registeredDOSBoxInputBridge()->setScrollLockActive(active)
#endif

#if defined(BXDOSBOX_JOYSTICK_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_setJoystickActive(active) boxer_registeredDOSBoxJoystickBridge()->setActive(active)
#endif

#if !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define BXLeftChannel BXDOSBoxAudioChannelLeft
#define BXRightChannel BXDOSBoxAudioChannelRight
#define boxer_masterVolume(channel) boxer_registeredDOSBoxAudioBridge()->masterVolume(channel)
#endif

#endif /* __cplusplus */

#endif /* BXDOSBoxLegacyCompatibility_h */
