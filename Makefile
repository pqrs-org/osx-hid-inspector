all:
	mkdir -p build \
		&& cd build \
		&& cmake .. \
		&& make

clean:
	rm -rf build

run:
	./build/osx-iokit-inspector

update_vendor:
	rm -rf vendor
	cget install pqrs-org/cget-recipes --prefix vendor
	cget install -f cget-requirements.txt --prefix vendor
