all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

dist: all
	rm -rf tmp
	mkdir -p tmp/hid-inspector/bin
	install -m 755 src/build/Release/hid-inspector tmp/hid-inspector/bin
	bash scripts/codesign.sh tmp
	hdiutil create -nospotlight hid-inspector.dmg -srcfolder tmp/hid-inspector -fs 'Journaled HFS+'
	rm -rf tmp
	mkdir -p dist
	mv hid-inspector.dmg dist

notarize:
	xcrun notarytool \
		submit dist/hid-inspector.dmg \
		--keychain-profile "pqrs.org notarization" \
		--wait
	$(MAKE) staple
	say "notarization completed"

staple:
	xcrun stapler staple dist/hid-inspector.dmg

check-staple:
	xcrun stapler validate dist/hid-inspector.dmg
