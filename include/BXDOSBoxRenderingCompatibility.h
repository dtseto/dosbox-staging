#ifndef BXDOSBoxRenderingCompatibility_h
#define BXDOSBoxRenderingCompatibility_h

#include "video.h"
#include "BXDOSBoxBridgeRegistration.h"

static inline void boxer_renderingBridgeApplyStrategy(void)
{
    boxer_applyRenderingStrategy();
}

static inline uintptr_t boxer_renderingBridgePrepareForFrameSize(uintptr_t width, uintptr_t height,
                                                                  uintptr_t flags, double scaleX,
                                                                  double scaleY, uintptr_t callback,
                                                                  double pixelAspect)
{
    return boxer_prepareForFrameSize((Bitu)width, (Bitu)height, (Bitu)flags, scaleX, scaleY,
                                     (GFX_CallBack_t)callback, pixelAspect);
}

static inline uintptr_t boxer_renderingBridgeIdealOutputMode(uintptr_t flags)
{
    return boxer_idealOutputMode((Bitu)flags);
}

static inline bool boxer_renderingBridgeStartFrame(uint8_t **pixels, int *pitch)
{
    return boxer_startFrame(*pixels, *pitch);
}

static inline void boxer_renderingBridgeFinishFrame(const uint16_t *dirtyBlocks)
{
    boxer_finishFrame(dirtyBlocks);
}

static inline uintptr_t boxer_renderingBridgeGetRGBPaletteEntry(uint8_t red, uint8_t green,
                                                                 uint8_t blue)
{
    return boxer_getRGBPaletteEntry(red, green, blue);
}

static inline void boxer_renderingBridgeSetShader(const char *source)
{
    boxer_setShader(source);
}

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
static inline const BXDOSBoxRenderingBridgeCallbacks *boxer_registeredDOSBoxRenderingBridge(void)
{
    static const BXDOSBoxRenderingBridgeCallbacks callbacks = {
        boxer_renderingBridgeApplyStrategy,
        boxer_renderingBridgePrepareForFrameSize,
        boxer_renderingBridgeIdealOutputMode,
        boxer_renderingBridgeStartFrame,
        boxer_renderingBridgeFinishFrame,
        boxer_renderingBridgeGetRGBPaletteEntry,
        boxer_renderingBridgeSetShader
    };
    return &callbacks;
}
#endif

#if defined(BXDOSBOX_RENDERING_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#ifdef GFX_StartUpdate
#undef GFX_StartUpdate
#endif
#ifdef GFX_EndUpdate
#undef GFX_EndUpdate
#endif
#ifdef GFX_GetRGB
#undef GFX_GetRGB
#endif
#ifdef GFX_GetBestMode
#undef GFX_GetBestMode
#endif
#ifdef GFX_SetShader
#undef GFX_SetShader
#endif
#ifdef GFX_SetSize
#undef GFX_SetSize
#endif

#define boxer_applyRenderingStrategy() boxer_registeredDOSBoxRenderingBridge()->applyRenderingStrategy()
#define GFX_StartUpdate(pixels, pitch) boxer_registeredDOSBoxRenderingBridge()->startFrame((uint8_t **)&(pixels), &(pitch))
#define GFX_EndUpdate(dirtyBlocks) boxer_registeredDOSBoxRenderingBridge()->finishFrame(dirtyBlocks)
#define GFX_GetRGB(red, green, blue) boxer_registeredDOSBoxRenderingBridge()->getRGBPaletteEntry(red, green, blue)
#define GFX_GetBestMode(flags) boxer_registeredDOSBoxRenderingBridge()->idealOutputMode(flags)
#define GFX_SetShader(source) boxer_registeredDOSBoxRenderingBridge()->setShader((source).c_str())
#define GFX_SetSize(width, height, flags, scaleX, scaleY, callback, pixelAspect) \
    boxer_registeredDOSBoxRenderingBridge()->prepareForFrameSize(width, height, flags, scaleX, scaleY, \
                                                                  (uintptr_t)(callback), pixelAspect)
#endif

#endif /* BXDOSBoxRenderingCompatibility_h */
