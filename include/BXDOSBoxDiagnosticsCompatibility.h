/*
 * DOSBox-side diagnostics compatibility adapter.
 *
 * The public diagnostics bridge uses plain C strings and explicit fatal
 * context. This adapter preserves the existing variadic Boxer symbols for
 * standalone harnesses while production call sites use the registered table.
 */

#ifndef BXDOSBoxDiagnosticsCompatibility_h
#define BXDOSBoxDiagnosticsCompatibility_h

#include "BXDOSBoxLegacyDeclarations.h"
#include "BXDOSBoxBridgeRegistration.h"

#ifdef __cplusplus

#include <cstdarg>
#include <cstdio>

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
const char *boxer_localizedStringForKey(const char *key) __attribute__((weak));
void boxer_log(const char *format, ...) __attribute__((weak));
void boxer_die(const char *functionName, const char *fileName, int lineNumber,
               const char *format, ...) __attribute__((weak));

static inline void boxer_diagnosticsLegacyLogMessage(const char *message)
{
    boxer_log("%s", message);
}

static inline void boxer_diagnosticsLegacyFatalError(const char *functionName,
                                                      const char *fileName,
                                                      int lineNumber,
                                                      const char *message)
{
    boxer_die(functionName, fileName, lineNumber, "%s", message);
}

static inline const BXDOSBoxDiagnosticsBridgeCallbacks *
boxer_registeredDOSBoxDiagnosticsBridge(void)
{
    static const BXDOSBoxDiagnosticsBridgeCallbacks callbacks = {
        boxer_localizedStringForKey,
        boxer_diagnosticsLegacyLogMessage,
        boxer_diagnosticsLegacyFatalError
    };
    return &callbacks;
}

#endif

static inline void boxer_diagnosticsLog(const char *format, ...)
{
    char message[1024];
    va_list arguments;
    va_start(arguments, format);
    std::vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    boxer_registeredDOSBoxDiagnosticsBridge()->logMessage(message);
}

static inline void boxer_diagnosticsFatal(const char *functionName,
                                          const char *fileName,
                                          int lineNumber,
                                          const char *format, ...)
{
    char message[1024];
    va_list arguments;
    va_start(arguments, format);
    std::vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    boxer_registeredDOSBoxDiagnosticsBridge()->fatalError(functionName, fileName,
                                                           lineNumber, message);
}

#if defined(BXDOSBOX_DIAGNOSTICS_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_localizedStringForKey(key) \
    boxer_registeredDOSBoxDiagnosticsBridge()->localizedStringForKey(key)
#endif

#if !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define GFX_ShowMsg(...) boxer_diagnosticsLog(__VA_ARGS__)
#define E_Exit(format,...) \
    boxer_diagnosticsFatal(__PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)
#endif

#endif /* __cplusplus */

#endif /* BXDOSBoxDiagnosticsCompatibility_h */
