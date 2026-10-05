#ifndef BXDOSBoxRunLoopCompatibility_h
#define BXDOSBoxRunLoopCompatibility_h

#include "BXDOSBoxBridgeRegistration.h"

static inline bool boxer_runLoopBridgeShouldContinue(void)
{
    return boxer_runLoopShouldContinue();
}

static inline void boxer_runLoopBridgeWillStartWithContextInfo(void **contextInfo)
{
    boxer_runLoopWillStartWithContextInfo(contextInfo);
}

static inline void boxer_runLoopBridgeDidFinishWithContextInfo(void *contextInfo)
{
    boxer_runLoopDidFinishWithContextInfo(contextInfo);
}

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
static inline const BXDOSBoxRunLoopBridgeCallbacks *boxer_registeredDOSBoxRunLoopBridge(void)
{
    static const BXDOSBoxRunLoopBridgeCallbacks callbacks = {
        boxer_runLoopBridgeShouldContinue,
        boxer_runLoopBridgeWillStartWithContextInfo,
        boxer_runLoopBridgeDidFinishWithContextInfo
    };
    return &callbacks;
}
#endif

#if defined(BXDOSBOX_RUNLOOP_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_runLoopShouldContinue() boxer_registeredDOSBoxRunLoopBridge()->shouldContinue()
#define boxer_runLoopWillStartWithContextInfo(contextInfo) boxer_registeredDOSBoxRunLoopBridge()->willStartWithContextInfo(contextInfo)
#define boxer_runLoopDidFinishWithContextInfo(contextInfo) boxer_registeredDOSBoxRunLoopBridge()->didFinishWithContextInfo(contextInfo)
#endif

#endif /* BXDOSBoxRunLoopCompatibility_h */
