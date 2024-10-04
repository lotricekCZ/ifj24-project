all: cmake

cmake:
	@mkdir -p build
	@cd build && cmake .. && make -j12

run:
	@cd build && ./ifj2024