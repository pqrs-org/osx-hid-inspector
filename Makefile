all:
	mkdir -p build \
		&& cd build \
		&& cmake .. \
		&& make

clean:
	rm -rf build

run-help:
	@./build/hid-inspector

run-output-all-properties-json:
	@./build/hid-inspector --output-all-properties-json

run-output-usages:
	@./build/hid-inspector --output-usages

update_vendor:
	rm -rf vendor
	cget install pqrs-org/cget-recipes --prefix vendor
	cget install -f cget-requirements.txt --prefix vendor
