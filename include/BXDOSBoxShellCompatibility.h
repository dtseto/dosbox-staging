/*
 * DOSBox-side shell compatibility adapter.
 *
 * This file is deliberately separate from BXDOSBoxBridgeRegistration.h:
 * shell callbacks use DOS_Shell and Bitu in their legacy declarations, while
 * the public registration contract exposes only opaque handles.
 */

#ifndef BXDOSBoxShellCompatibility_h
#define BXDOSBoxShellCompatibility_h

#include "shell.h"
#include "BXDOSBoxBridgeRegistration.h"

static inline void boxer_shellBridgeWillStart(uintptr_t shell)
{
    boxer_shellWillStart((DOS_Shell *)shell);
}

static inline void boxer_shellBridgeDidFinish(uintptr_t shell)
{
    boxer_shellDidFinish((DOS_Shell *)shell);
}

static inline bool boxer_shellBridgeShouldContinue(uintptr_t shell)
{
    return boxer_shellShouldContinue((DOS_Shell *)shell);
}

static inline bool boxer_shellBridgeShouldRunCommand(uintptr_t shell, char *command, char *arguments)
{
    return boxer_shellShouldRunCommand((DOS_Shell *)shell, command, arguments);
}

static inline bool boxer_shellBridgeHandleCommandInput(uintptr_t shell, char *line,
                                                        uintptr_t *cursorPosition,
                                                        bool *executeImmediately)
{
    return boxer_handleShellCommandInput((DOS_Shell *)shell, line, (Bitu *)cursorPosition,
                                         executeImmediately);
}

static inline bool boxer_shellBridgeHasPendingCommands(uintptr_t shell)
{
    return boxer_hasPendingCommandsForShell((DOS_Shell *)shell);
}

static inline bool boxer_shellBridgeExecuteNextPendingCommand(uintptr_t shell)
{
    return boxer_executeNextPendingCommandForShell((DOS_Shell *)shell);
}

static inline void boxer_shellBridgeDidReturnToShell(uintptr_t shell)
{
    boxer_didReturnToShell((DOS_Shell *)shell);
}

static inline void boxer_shellBridgeWillStartAutoexec(uintptr_t shell)
{
    boxer_shellWillStartAutoexec((DOS_Shell *)shell);
}

static inline bool boxer_shellBridgeShouldDisplayStartupMessages(uintptr_t shell)
{
    return boxer_shellShouldDisplayStartupMessages((DOS_Shell *)shell);
}

static inline void boxer_shellBridgeWillReadCommandInput(uintptr_t shell, uint16_t handle)
{
    boxer_shellWillReadCommandInputFromHandle((DOS_Shell *)shell, handle);
}

static inline void boxer_shellBridgeDidReadCommandInput(uintptr_t shell, uint16_t handle)
{
    boxer_shellDidReadCommandInputFromHandle((DOS_Shell *)shell, handle);
}

static inline void boxer_shellBridgeWillExecuteFile(uintptr_t shell, const char *path,
                                                    const char *arguments)
{
    boxer_shellWillExecuteFileAtDOSPath((DOS_Shell *)shell, path, arguments);
}

static inline void boxer_shellBridgeDidExecuteFile(uintptr_t shell, const char *path)
{
    boxer_shellDidExecuteFileAtDOSPath((DOS_Shell *)shell, path);
}

static inline void boxer_shellBridgeWillBeginBatch(uintptr_t shell, const char *path,
                                                   const char *arguments)
{
    boxer_shellWillBeginBatchFile((DOS_Shell *)shell, path, arguments);
}

static inline void boxer_shellBridgeDidEndBatch(uintptr_t shell, const char *path)
{
    boxer_shellDidEndBatchFile((DOS_Shell *)shell, path);
}

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
static inline const BXDOSBoxShellBridgeCallbacks *boxer_registeredDOSBoxShellBridge(void)
{
    static const BXDOSBoxShellBridgeCallbacks callbacks = {
        boxer_shellBridgeWillStart,
        boxer_shellBridgeDidFinish,
        boxer_shellBridgeShouldContinue,
        boxer_shellBridgeShouldRunCommand,
        boxer_shellBridgeHandleCommandInput,
        boxer_shellBridgeHasPendingCommands,
        boxer_shellBridgeExecuteNextPendingCommand,
        boxer_shellBridgeDidReturnToShell,
        boxer_shellBridgeWillStartAutoexec,
        boxer_shellBridgeShouldDisplayStartupMessages,
        boxer_shellBridgeWillReadCommandInput,
        boxer_shellBridgeDidReadCommandInput,
        boxer_shellBridgeWillExecuteFile,
        boxer_shellBridgeDidExecuteFile,
        boxer_shellBridgeWillBeginBatch,
        boxer_shellBridgeDidEndBatch
    };
    return &callbacks;
}
#endif

#if defined(BXDOSBOX_SHELL_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_shellWillStart(shell) boxer_registeredDOSBoxShellBridge()->willStart((uintptr_t)(shell))
#define boxer_shellDidFinish(shell) boxer_registeredDOSBoxShellBridge()->didFinish((uintptr_t)(shell))
#define boxer_shellShouldContinue(shell) boxer_registeredDOSBoxShellBridge()->shouldContinue((uintptr_t)(shell))
#define boxer_shellShouldRunCommand(shell, command, arguments) boxer_registeredDOSBoxShellBridge()->shouldRunCommand((uintptr_t)(shell), command, arguments)
#define boxer_handleShellCommandInput(shell, line, cursorPosition, executeImmediately) boxer_registeredDOSBoxShellBridge()->handleCommandInput((uintptr_t)(shell), line, (uintptr_t *)(cursorPosition), executeImmediately)
#define boxer_hasPendingCommandsForShell(shell) boxer_registeredDOSBoxShellBridge()->hasPendingCommands((uintptr_t)(shell))
#define boxer_executeNextPendingCommandForShell(shell) boxer_registeredDOSBoxShellBridge()->executeNextPendingCommand((uintptr_t)(shell))
#define boxer_didReturnToShell(shell) boxer_registeredDOSBoxShellBridge()->didReturnToShell((uintptr_t)(shell))
#define boxer_shellWillStartAutoexec(shell) boxer_registeredDOSBoxShellBridge()->willStartAutoexec((uintptr_t)(shell))
#define boxer_shellShouldDisplayStartupMessages(shell) boxer_registeredDOSBoxShellBridge()->shouldDisplayStartupMessages((uintptr_t)(shell))
#define boxer_shellWillReadCommandInputFromHandle(shell, handle) boxer_registeredDOSBoxShellBridge()->willReadCommandInputFromHandle((uintptr_t)(shell), handle)
#define boxer_shellDidReadCommandInputFromHandle(shell, handle) boxer_registeredDOSBoxShellBridge()->didReadCommandInputFromHandle((uintptr_t)(shell), handle)
#define boxer_shellWillExecuteFileAtDOSPath(shell, path, arguments) boxer_registeredDOSBoxShellBridge()->willExecuteFileAtDOSPath((uintptr_t)(shell), path, arguments)
#define boxer_shellDidExecuteFileAtDOSPath(shell, path) boxer_registeredDOSBoxShellBridge()->didExecuteFileAtDOSPath((uintptr_t)(shell), path)
#define boxer_shellWillBeginBatchFile(shell, path, arguments) boxer_registeredDOSBoxShellBridge()->willBeginBatchFile((uintptr_t)(shell), path, arguments)
#define boxer_shellDidEndBatchFile(shell, path) boxer_registeredDOSBoxShellBridge()->didEndBatchFile((uintptr_t)(shell), path)
#endif

#endif /* BXDOSBoxShellCompatibility_h */
