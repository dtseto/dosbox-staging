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

void boxer_registerDOSBoxAudioBridge(const BXDOSBoxAudioBridgeCallbacks *callbacks);
const BXDOSBoxAudioBridgeCallbacks *boxer_registeredDOSBoxAudioBridge(void);

#ifdef __cplusplus
}
#endif

#endif /* BXDOSBoxBridgeRegistration_h */
