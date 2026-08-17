/*
 * Boxer integration regression helpers.
 *
 * These tests intentionally read Boxer/DOSBox-Staging source files as contract
 * fixtures. They protect the documented Boxer integration markers without
 * requiring GUI, MIDI, joystick, printer, or user gamebox state.
 */

#ifndef BOXER_REGRESSION_CONTRACTS_H
#define BOXER_REGRESSION_CONTRACTS_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace boxer_regression {

struct CoverageRow {
	const char *subsystem;
	std::vector<const char *> markers;
};

inline const std::vector<CoverageRow> &DocumentedRows()
{
	static const std::vector<CoverageRow> rows = {
	        // Protects BOXER marker: coalface-remaps
	        {"Core bridge, SDL/event/mouse/video capture remaps", {"coalface-remaps"}},
	        // Protects BOXER markers: runloop-termination, runloop-event-cancellation, runloop-context, shutdown-drive-clear
	        {"Emulator run loop and shutdown lifecycle", {"runloop-termination", "runloop-event-cancellation", "runloop-context", "shutdown-drive-clear"}},
	        // Protects BOXER markers: xcode-lazyflags-include, keyboard-enum-c-compat
	        {"Build/Xcode compatibility", {"xcode-lazyflags-include", "keyboard-enum-c-compat"}},
	        // Protects BOXER markers: boxer-mt32-config-include, mt32-device-value, mt32-help-unconditional, mt32-midiconfig-help, mt32-config-section, dosbox-parport-init, parallel-config-section
	        {"Configuration: MT-32, MIDI, and parallel printer", {"boxer-mt32-config-include", "mt32-device-value", "mt32-help-unconditional", "mt32-midiconfig-help", "mt32-config-section", "dosbox-parport-init", "parallel-config-section"}},
	        // Protects BOXER marker: midi-routing
	        {"MIDI routing and sysex policy", {"midi-routing"}},
	        // Protects BOXER marker: mixer-volume-bridge
	        {"Audio mixer volume bridge", {"mixer-volume-bridge"}},
	        // Protects BOXER markers: render-reset-strategy, display-mode-controls, display-refresh-rate, capture-file-routing, core-mode-title-refresh
	        {"Video rendering, display options, capture files", {"render-reset-strategy", "display-mode-controls", "display-refresh-rate", "capture-file-routing", "core-mode-title-refresh"}},
	        // Protects BOXER markers: keyboard-buffer-capacity, console-read-cancel, console-paste-availability, bios-key-paste-pop, bios-key-paste-peek, caps-lock-state, num-lock-state, scroll-lock-state, int16-cancel, keyboard-layout-switching-api, keyboard-cpi-buffer-storage, keyboard-layout-state-methods, keyboard-layout-bridge, macos-preferred-keyboard-layout, us-layout-remap-fix
	        {"Keyboard input, paste, lock keys, and layout", {"keyboard-buffer-capacity", "console-read-cancel", "console-paste-availability", "bios-key-paste-pop", "bios-key-paste-peek", "caps-lock-state", "num-lock-state", "scroll-lock-state", "int16-cancel", "keyboard-layout-switching-api", "keyboard-cpi-buffer-storage", "keyboard-layout-state-methods", "keyboard-layout-bridge", "macos-preferred-keyboard-layout", "us-layout-remap-fix"}},
	        // Protects BOXER markers: gameport-timing-export, gameport-timing-state, mapper-free-autofire, gameport-poll-activation, gameport-timing-config, preserve-controller-ownership, dos-visible-joystick-state, joystick-handler-install-end
	        {"Joystick and controller ownership", {"gameport-timing-export", "gameport-timing-state", "mapper-free-autofire", "gameport-poll-activation", "gameport-timing-config", "preserve-controller-ownership", "dos-visible-joystick-state", "joystick-handler-install-end"}},
	        // Protects BOXER markers: drive-system-path, initialize-drive-system-path, retrieve-drive-system-path, fat-drive-system-path, iso-drive-system-path, local-drive-system-path, drive-cache-filter-bridge, hide-host-metadata, file-create-write-policy, file-open-write-policy, file-open-write-policy-end, file-delete-write-policy, local-dir-create-policy, local-file-created, local-file-removed, local-open-file-removed, imgmount-drive-mounted, mount-drive-mounted, drive-unmounted, invalid-fat-image-fails-construction, invalid-fat-bootsector-fails-construction, suppress-cdrom-image-error-text, file-unavailable-notification, local-file-unavailable-notification, local-file-unavailable, unavailable-file-read, unavailable-file-write, unavailable-file-seek, unavailable-file-timestamp
	        {"Gamebox drive paths, file policy, and mounted media", {"drive-system-path", "initialize-drive-system-path", "retrieve-drive-system-path", "fat-drive-system-path", "iso-drive-system-path", "local-drive-system-path", "drive-cache-filter-bridge", "hide-host-metadata", "file-create-write-policy", "file-open-write-policy", "file-open-write-policy-end", "file-delete-write-policy", "local-dir-create-policy", "local-file-created", "local-file-removed", "local-open-file-removed", "imgmount-drive-mounted", "mount-drive-mounted", "drive-unmounted", "invalid-fat-image-fails-construction", "invalid-fat-bootsector-fails-construction", "suppress-cdrom-image-error-text", "file-unavailable-notification", "local-file-unavailable-notification", "local-file-unavailable", "unavailable-file-read", "unavailable-file-write", "unavailable-file-seek", "unavailable-file-timestamp"}},
	        // Protects BOXER markers: current-shell-export, active-shell-global, shell-run-lifecycle, shell-misc-bridge, shell-input-injection, shell-command-filter, batch-lifecycle-bridge, batch-file-ended, program-launch-lifecycle
	        {"Shell lifecycle, command injection, and launch tracking", {"current-shell-export", "active-shell-global", "shell-run-lifecycle", "shell-misc-bridge", "shell-input-injection", "shell-command-filter", "batch-lifecycle-bridge", "batch-file-ended", "program-launch-lifecycle"}},
	        // Protects BOXER markers: hide-intro-command, shell-command-ux, delete-help-if-no-args, delete-unix-path-tolerance, rename-help-if-no-args, mkdir-help-if-no-args, mkdir-unix-path-tolerance, rmdir-help-if-no-args, rmdir-unix-path-tolerance, dir-unix-path-trailing-slash, dir-unix-path-tolerance, copy-help-if-no-args, copy-unix-path-tolerance, if-help-if-no-args, type-help-if-no-args, call-help-if-no-args, subst-help-if-no-args, loadhigh-help-if-no-args, loadhigh-unix-path-tolerance
	        {"Shell command UX compatibility", {"hide-intro-command", "shell-command-ux", "delete-help-if-no-args", "delete-unix-path-tolerance", "rename-help-if-no-args", "mkdir-help-if-no-args", "mkdir-unix-path-tolerance", "rmdir-help-if-no-args", "rmdir-unix-path-tolerance", "dir-unix-path-trailing-slash", "dir-unix-path-tolerance", "copy-help-if-no-args", "copy-unix-path-tolerance", "if-help-if-no-args", "type-help-if-no-args", "call-help-if-no-args", "subst-help-if-no-args", "loadhigh-help-if-no-args", "loadhigh-unix-path-tolerance"}},
	        // Protects BOXER markers: printer-redirection, parport-skip-occupied-lpt, bios-parport-include, int17-printer-emulation, bios-parport-detection-disabled, bios-equipment-parport-count, bios-refresh-parport-count, int21-printer-output
	        {"Printer and parallel-port routing", {"printer-redirection", "parport-skip-occupied-lpt", "bios-parport-include", "int17-printer-emulation", "bios-parport-detection-disabled", "bios-equipment-parport-count", "bios-refresh-parport-count", "int21-printer-output"}},
	        // Protects BOXER markers: localization-routing, upstream-localization-disabled
	        {"Localization", {"localization-routing", "upstream-localization-disabled"}},
	};
	return rows;
}

inline std::filesystem::path ProjectRoot()
{
	auto path = std::filesystem::current_path();
	while (!path.empty()) {
		if (std::filesystem::exists(path / "BOXER_PATCHES.md") &&
		    std::filesystem::exists(path / "src") &&
		    std::filesystem::exists(path / "include"))
			return path;
		path = path.parent_path();
	}
	return std::filesystem::current_path();
}

inline std::string ReadFile(const std::filesystem::path &path)
{
	std::ifstream file(path);
	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

inline bool Contains(const std::filesystem::path &path, const std::string &needle)
{
	return ReadFile(path).find(needle) != std::string::npos;
}

inline void ExpectContains(const std::filesystem::path &path,
                           const std::string &needle)
{
	EXPECT_TRUE(Contains(path, needle)) << path << " missing: " << needle;
}

inline std::set<std::string> DocumentedMarkerSet()
{
	std::set<std::string> markers;
	for (const auto &row : DocumentedRows())
		for (const auto *marker : row.markers)
			markers.insert(marker);
	return markers;
}

inline std::set<std::string> SourceMarkerSet()
{
	static const std::regex marker_regex("BOXER-(BEGIN|END|HOOK):[[:space:]]*([a-z0-9-]+)");
	std::set<std::string> markers;
	for (const auto &root : {ProjectRoot() / "include", ProjectRoot() / "src"}) {
		for (const auto &entry : std::filesystem::recursive_directory_iterator(root)) {
			if (!entry.is_regular_file())
				continue;
			const auto contents = ReadFile(entry.path());
			for (std::sregex_iterator it(contents.begin(), contents.end(), marker_regex), end; it != end; ++it)
				markers.insert((*it)[2]);
		}
	}
	return markers;
}

inline void ExpectBlockContains(const std::filesystem::path &path,
                                const std::string &marker,
                                const std::string &needle)
{
	const auto contents = ReadFile(path);
	const auto begin = contents.find("BOXER-BEGIN: " + marker);
	ASSERT_NE(begin, std::string::npos) << path << " missing begin marker " << marker;
	const auto end = contents.find("BOXER-END: " + marker, begin);
	ASSERT_NE(end, std::string::npos) << path << " missing end marker " << marker;
	const auto block = contents.substr(begin, end - begin);
	EXPECT_NE(block.find(needle), std::string::npos)
	        << path << " marker " << marker << " missing: " << needle;
}

} // namespace boxer_regression

#endif
