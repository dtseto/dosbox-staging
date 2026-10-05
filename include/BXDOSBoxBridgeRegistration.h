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

void boxer_registerDOSBoxAudioBridge(const BXDOSBoxAudioBridgeCallbacks *callbacks);
#ifdef BXDOSBOX_BRIDGE_IMPLEMENTATION
const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void);
#endif

#ifdef __cplusplus
}

#ifndef BXDOSBOX_BRIDGE_IMPLEMENTATION
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
#endif
#endif

#endif /* BXDOSBoxBridgeRegistration_h */
