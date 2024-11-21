#!/bin/bash
rm -rf flat
mkdir -p flat
find src -name '*.h' -exec cp -v \{\} ./flat/ \;
find src -name '*.c' -exec cp -v \{\} ./flat/ \;
teamleader="xhubacv00"
cd flat
for f in *; do
	awk '/#include ".+\.h"/ \
		{match($0, /#include "(.+\.h)"/); \
		i=index($0, "\"");\
		k=index(substr($0, i+1, RLENGTH), "/");\
		j=index(substr($0, i+1, RLENGTH), "\"");\
		while(index(substr($0, i+k, RLENGTH), "/") != 0) \
			k++; \
		str = "\""substr($0, i+k, i+j-2);
		gsub(/""/, "\"", str); print "#include "str; next} 1' $f > $f.new
	mv $f.new $f
done


cp -v ../Makefile Makefile
cp -v ../CMakeLists.txt CMakeLists.txt

if [ -f ../dokumentace.pdf ]; then
	cp -v ../dokumentace.pdf .
fi

sed -i '/enable_testing()/,$d' CMakeLists.txt
sed -i '/add_library/s/\${.*}\///g' CMakeLists.txt
sed -i '/add_executable/s/\${.*}\///g' CMakeLists.txt
sed -i '/set(SRC/,/set(PARSER/ d' CMakeLists.txt
sed -i '/src\/main\.c/s/src\/main\.c/main\.c/g' CMakeLists.txt

sed -i '/mkdir/d' Makefile
sed -i 's/cd build && //g' Makefile
sed -i 's/cmake \.\./cmake ./g' Makefile

zip -r ${teamleader}.zip .
