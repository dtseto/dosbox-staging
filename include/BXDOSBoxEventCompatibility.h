#ifndef BXDOSBoxEventCompatibility_h
#define BXDOSBoxEventCompatibility_h

#include "video.h"
#include "BXDOSBoxBridgeRegistration.h"

#ifdef __cplusplus

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
bool boxer_processEvents(void) __attribute__((weak));
void boxer_handleDOSBoxTitleChange(int32_t cycles, bool paused) __attribute__((weak));
void boxer_refreshDOSBoxTitle(void) __attribute__((weak));
void boxer_setMouseHint(MouseHint hint) __attribute__((weak));
void boxer_setMouseCapture(bool captured) __attribute__((weak));
void boxer_setMouseRawInput(bool rawInput) __attribute__((weak));
void boxer_setMouseVisibility(bool visible) __attribute__((weak));
bool boxer_hasDesktopEnvironment(void) __attribute__((weak));
void boxer_centerMouse(void) __attribute__((weak));

static inline void boxer_eventBridgeSetMouseHint(uintptr_t hint)
{
    boxer_setMouseHint((MouseHint)hint);
}

static inline const BXDOSBoxEventBridgeCallbacks *boxer_registeredDOSBoxEventBridge(void)
{
    static const BXDOSBoxEventBridgeCallbacks callbacks = {
        boxer_processEvents,
        boxer_handleDOSBoxTitleChange,
        boxer_refreshDOSBoxTitle,
        boxer_eventBridgeSetMouseHint,
        boxer_setMouseCapture,
        boxer_setMouseRawInput,
        boxer_setMouseVisibility,
        boxer_hasDesktopEnvironment,
        boxer_centerMouse
    };
    return &callbacks;
}
#endif

static inline void boxer_eventBridgeSetTitle(int32_t cycles)
{
    boxer_registeredDOSBoxEventBridge()->handleTitleChange(cycles, false);
}

static inline void boxer_eventBridgeSetTitle(int32_t cycles, bool paused)
{
    boxer_registeredDOSBoxEventBridge()->handleTitleChange(cycles, paused);
}

#if defined(BXDOSBOX_EVENT_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#ifdef GFX_Events
#undef GFX_Events
#endif
#ifdef GFX_SetTitle
#undef GFX_SetTitle
#endif
#ifdef GFX_RefreshTitle
#undef GFX_RefreshTitle
#endif
#ifdef GFX_SetMouseHint
#undef GFX_SetMouseHint
#endif
#ifdef GFX_SetMouseCapture
#undef GFX_SetMouseCapture
#endif
#ifdef GFX_SetMouseRawInput
#undef GFX_SetMouseRawInput
#endif
#ifdef GFX_SetMouseVisibility
#undef GFX_SetMouseVisibility
#endif
#ifdef GFX_HaveDesktopEnvironment
#undef GFX_HaveDesktopEnvironment
#endif
#ifdef GFX_CenterMouse
#undef GFX_CenterMouse
#endif

#define GFX_Events() boxer_registeredDOSBoxEventBridge()->processEvents()
#define GFX_SetTitle(...) boxer_eventBridgeSetTitle(__VA_ARGS__)
#define GFX_RefreshTitle() boxer_registeredDOSBoxEventBridge()->refreshTitle()
#define GFX_SetMouseHint(hint) boxer_registeredDOSBoxEventBridge()->setMouseHint((uintptr_t)(hint))
#define GFX_SetMouseCapture(captured) boxer_registeredDOSBoxEventBridge()->setMouseCapture(captured)
#define GFX_SetMouseRawInput(rawInput) boxer_registeredDOSBoxEventBridge()->setMouseRawInput(rawInput)
#define GFX_SetMouseVisibility(visible) boxer_registeredDOSBoxEventBridge()->setMouseVisibility(visible)
#define GFX_HaveDesktopEnvironment() boxer_registeredDOSBoxEventBridge()->hasDesktopEnvironment()
#define GFX_CenterMouse() boxer_registeredDOSBoxEventBridge()->centerMouse()
#endif

#endif /* __cplusplus */

#endif /* BXDOSBoxEventCompatibility_h */
