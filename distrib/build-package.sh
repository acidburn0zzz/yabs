#!/usr/bin/env bash

TMP="/tmp/yabs-build"
DIR=$(dirname $0)

echo "Building ${1}..."
if [ -d "$DIR/yabs-git" ] ; then
	rm -rf $DIR/yabs-git
fi

case $2 in
	'release')
		VERS=`git tag -l | tail -n 1`
		git clone git://github.com/0X1A/yabs $DIR/yabs-git
		git checkout tags/$VERS
		;;
	'master')
		git clone git://github.com/0X1A/yabs $DIR/yabs-git
		VERS="git"
		;;
esac

cd $DIR/yabs-git
make PREFIX=/usr CXXFLAGS="-pipe -std=c++11 -march=x86-64 -mtune=generic -O2 -pipe --param=ssp-buffer-size=4 -Wall" LIBS="-lssh2 -lyaml -larchive" LFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro -Wl,-O1"
make PREFIX=/usr DEST=$TMP install
fpm --url "https://github.com/0X1A/yabs" --vendor 0x1a.us --license BSD -v $VERS \
	--description "Yet another build system" -f -p "../yabs-VERSION_ARCH.${1}" \
	-d "libyaml" -d "libarchive" -d "libssh2" -m "Alberto Corona <alberto@0x1a.us>" \
	-s dir -t ${1} -n "yabs-$VERS" -C $TMP 
