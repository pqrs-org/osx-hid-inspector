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
