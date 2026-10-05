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

void boxer_registerDOSBoxAudioBridge(const BXDOSBoxAudioBridgeCallbacks *callbacks);
#if defined(BXDOSBOX_PRODUCTION) || defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void);
void boxer_registerDOSBoxPrinterBridge(const BXDOSBoxPrinterBridgeCallbacks *callbacks);
const BXDOSBoxPrinterBridgeCallbacks *boxer_registeredDOSBoxPrinterBridge(void);
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
#endif

#if defined(BXDOSBOX_PRINTER_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_PRINTER_isInited(port) boxer_registeredDOSBoxPrinterBridge()->isInitialized(port)
#define boxer_PRINTER_readdata(port, width) boxer_registeredDOSBoxPrinterBridge()->readData(port, width)
#define boxer_PRINTER_readcontrol(port, width) boxer_registeredDOSBoxPrinterBridge()->readControl(port, width)
#define boxer_PRINTER_readstatus(port, width) boxer_registeredDOSBoxPrinterBridge()->readStatus(port, width)
#define boxer_PRINTER_writedata(port, value, width) boxer_registeredDOSBoxPrinterBridge()->writeData(port, value, width)
#define boxer_PRINTER_writecontrol(port, value, width) boxer_registeredDOSBoxPrinterBridge()->writeControl(port, value, width)
#endif
#endif

#endif /* BXDOSBoxBridgeRegistration_h */
