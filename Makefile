all: cmake

cmake:
	@mkdir -p build
	@cd build && cp Makefile tmpMakefile && cmake .. && make -j12 && rm -rf CMakeFiles Testing && mv tmpMakefile Makefile

test:
	@cd build && make test

clean:
	@cd build && cp Makefile tmpMakefile && cmake .. && make clean && rm -rf CMakeFiles Testing && mv tmpMakefile Makefile

run:
	@cd build && ./ifj2024

flat:
	./flattener.sh