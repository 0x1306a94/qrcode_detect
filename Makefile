MAKEFLAGS += --silent

clean:
	echo "♻️ Cleaning ..."
	rm -rf ./third_party_build/*

download_dependencies:
	./sync_deps.sh

build_dependencies:
	./script/build_libargparse.sh
	./script/build_libhv.sh
	./script/build_libopencv.sh
	./script/build_libspdlog.sh
	./script/build_libzbar.sh

build_deb:
	./script/build_deb.sh

build_docker:
	./docker/build_docker.sh