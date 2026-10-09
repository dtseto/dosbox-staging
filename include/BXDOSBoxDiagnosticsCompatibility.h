/*
 * DOSBox-side diagnostics compatibility adapter.
 *
 * The public diagnostics bridge uses plain C strings and explicit fatal
 * context. This adapter preserves the existing variadic Boxer symbols for
 * standalone harnesses while production call sites use the registered table.
 */

#ifndef BXDOSBoxDiagnosticsCompatibility_h
#define BXDOSBoxDiagnosticsCompatibility_h

#include "BXDOSBoxBridgeRegistration.h"

#ifdef __cplusplus

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
const char *boxer_localizedStringForKey(const char *key) __attribute__((weak));
void boxer_log(const char *format, ...) __attribute__((weak));
void boxer_die(const char *functionName, const char *fileName, int lineNumber,
               const char *format, ...) __attribute__((weak));

static inline const BXDOSBoxDiagnosticsBridgeCallbacks *
boxer_registeredDOSBoxDiagnosticsBridge(void)
{
    static const BXDOSBoxDiagnosticsBridgeCallbacks callbacks = {
        boxer_localizedStringForKey,
        nullptr,
        nullptr
    };
    return &callbacks;
}

static inline void boxer_diagnosticsLegacyLog(const char *message)
{
    boxer_log("%s", message);
}

static inline void boxer_diagnosticsLegacyFatal(const char *functionName,
                                                 const char *fileName,
                                                 int lineNumber,
                                                 const char *message)
{
    boxer_die(functionName, fileName, lineNumber, "%s", message);
}

#endif

#if defined(BXDOSBOX_DIAGNOSTICS_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_localizedStringForKey(key) \
    boxer_registeredDOSBoxDiagnosticsBridge()->localizedStringForKey(key)
#endif

#endif /* __cplusplus */

#endif /* BXDOSBoxDiagnosticsCompatibility_h */
