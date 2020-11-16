.PHONY: help deps build package push
.DEFAULT_GOAL := help
VERSION := $(shell head -1 VERSION)
$(VERBOSE).SILENT:

help:
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

require-%:
	if ! command -v ${*} 2>&1 >/dev/null; then \
		echo "! ${*} not installed"; \
		exit 1; \
	fi

clean: ## Clean
	rm -rf obj

deps: require-qpm ## Restore dependencies
	qpm restore

build: require-ndk-build ## Build
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk NDK_APPLICATION_MK=Application.mk VERSION=$(VERSION) $(ARGS)

package: build ## Build and package
	zip -j -u BeatTogether_v$(VERSION).zip "./libs/arm64-v8a/libbeattogether.so" "./libs/arm64-v8a/libbeatsaber-hook_0_8_4.so" "./libs/arm64-v8a/libcodegen_0_4_0.so" "./Cover.png" "./bmbfmod.json"

push: build ## Build and push to device
	adb push libs/arm64-v8a/libbeattogether.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libbeattogether.so
	adb shell am force-stop com.beatgames.beatsaber
