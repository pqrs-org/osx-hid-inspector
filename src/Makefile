all:
	mkdir -p build_xcode \
					&& cd build_xcode \
					&& cmake -G Xcode .. \
					&& xcodebuild -configuration Release SYMROOT="$(CURDIR)/build_xcode/build"

clean:
	rm -rf build_xcode

run-help:
	@./build_xcode/build/Release/hid-inspector

run-output-all-properties-json:
	@./build_xcode/build/Release/hid-inspector --output-all-properties-json

run-output-usages:
	@./build_xcode/build/Release/hid-inspector --output-usages

update_vendor:
	rm -rf vendor
	cget install pqrs-org/cget-recipes --prefix vendor
	cget install -f cget-requirements.txt --prefix vendor
