all:
	xcodegen
	xcodebuild -configuration Release SYMROOT="$(CURDIR)/build"

clean:
	rm -rf hid-inspector.xcodeproj
	rm -rf build

run-help:
	@./build/Release/hid-inspector

run-output-all-properties-json:
	@./build/Release/hid-inspector --output-all-properties-json

run-output-usages:
	@./build/Release/hid-inspector --output-usages
