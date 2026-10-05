/*
 * DOSBox-side filesystem and drive compatibility adapter.
 *
 * The public bridge carries opaque drive handles. This adapter is the only
 * layer that knows the legacy DOS_Drive * callback signatures.
 */

#ifndef BXDOSBoxFilesystemCompatibility_h
#define BXDOSBoxFilesystemCompatibility_h

#include "BXDOSBoxBridgeRegistration.h"

static inline bool boxer_filesystemBridgeShouldShowFileWithName(const char *name)
{
    return boxer_shouldShowFileWithName(name);
}

static inline bool boxer_filesystemBridgeShouldAllowWriteAccessToPath(const char *path,
                                                                       uintptr_t drive)
{
    return boxer_shouldAllowWriteAccessToPath(path, (DOS_Drive *)drive);
}

static inline void boxer_filesystemBridgeDidCreateLocalFile(const char *path, uintptr_t drive)
{
    boxer_didCreateLocalFile(path, (DOS_Drive *)drive);
}

static inline void boxer_filesystemBridgeDidRemoveLocalFile(const char *path, uintptr_t drive)
{
    boxer_didRemoveLocalFile(path, (DOS_Drive *)drive);
}

static inline bool boxer_filesystemBridgeCreateLocalDir(const char *path, uintptr_t drive)
{
    return boxer_createLocalDir(path, (DOS_Drive *)drive);
}

static inline void boxer_filesystemBridgeDriveDidMount(uint8_t driveIndex)
{
    boxer_driveDidMount(driveIndex);
}

static inline void boxer_filesystemBridgeDriveDidUnmount(uint8_t driveIndex)
{
    boxer_driveDidUnmount(driveIndex);
}

#if !defined(BXDOSBOX_PRODUCTION) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
static inline const BXDOSBoxFilesystemBridgeCallbacks *boxer_registeredDOSBoxFilesystemBridge(void)
{
    static const BXDOSBoxFilesystemBridgeCallbacks callbacks = {
        boxer_filesystemBridgeShouldShowFileWithName,
        boxer_filesystemBridgeShouldAllowWriteAccessToPath,
        boxer_filesystemBridgeDidCreateLocalFile,
        boxer_filesystemBridgeDidRemoveLocalFile,
        boxer_filesystemBridgeCreateLocalDir,
        boxer_filesystemBridgeDriveDidMount,
        boxer_filesystemBridgeDriveDidUnmount
    };
    return &callbacks;
}
#endif

#if defined(BXDOSBOX_FILESYSTEM_CALLSITE) && !defined(BXDOSBOX_BRIDGE_IMPLEMENTATION)
#define boxer_shouldShowFileWithName(name) boxer_registeredDOSBoxFilesystemBridge()->shouldShowFileWithName(name)
#define boxer_shouldAllowWriteAccessToPath(path, drive) boxer_registeredDOSBoxFilesystemBridge()->shouldAllowWriteAccessToPath(path, (uintptr_t)(drive))
#define boxer_didCreateLocalFile(path, drive) boxer_registeredDOSBoxFilesystemBridge()->didCreateLocalFile(path, (uintptr_t)(drive))
#define boxer_didRemoveLocalFile(path, drive) boxer_registeredDOSBoxFilesystemBridge()->didRemoveLocalFile(path, (uintptr_t)(drive))
#define boxer_createLocalDir(path, drive) boxer_registeredDOSBoxFilesystemBridge()->createLocalDir(path, (uintptr_t)(drive))
#define boxer_driveDidMount(index) boxer_registeredDOSBoxFilesystemBridge()->driveDidMount(index)
#define boxer_driveDidUnmount(index) boxer_registeredDOSBoxFilesystemBridge()->driveDidUnmount(index)
#endif

#endif /* BXDOSBoxFilesystemCompatibility_h */
