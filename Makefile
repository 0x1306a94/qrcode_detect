MAKEFLAGS += --silent

clean:
	echo "♻️ Cleaning ..."
	rm -rf ./download_deps/*
	rm -rf ./build_3rdparty/*

download_dependencies:
	./script/download_libargparse.sh
	./script/download_libhv.sh
	./script/download_libopencv.sh
	./script/download_libspdlog.sh
	./script/download_libzbar.sh

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