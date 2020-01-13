all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

dist: all
	rm -rf hid-inspector
	mkdir -p hid-inspector/bin
	install -m 755 src/build_xcode/build/Release/hid-inspector hid-inspector/bin
	bash scripts/codesign.sh hid-inspector
	hdiutil create -nospotlight hid-inspector.dmg -srcfolder hid-inspector -fs 'Journaled HFS+'
	rm -rf hid-inspector
	mkdir -p dist
	mv hid-inspector.dmg dist

notarize:
	xcrun altool \
					--notarize-app \
					-t osx \
					-f dist/hid-inspector.dmg \
					--primary-bundle-id org.pqrs.hid-inspector \
					-u 'tekezo@pqrs.org' \
					-p '@keychain:pqrs.org-notarize-app'

staple:
	xcrun stapler staple dist/hid-inspector.dmg

check-staple:
	xcrun stapler validate dist/hid-inspector.dmg
