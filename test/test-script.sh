#!/bin/sh

GRN='\033[0;32m'
NC='\033[0m'

norm() {
	./yabs -h
	./yabs -n
	./yabs -d
	./yabs -e include.tar.gz
	./yabs -p test/test-script.sh
	./yabs -p test/yml
	./yabs -p no-file
	./yabs -b yabs.ybf
	./yabs -m yabs.ybf
	./yabs --version
}

basic() {
	for file in test/yml/*
	do
		echo -e "\nyabs -p $file\n"
		./yabs -p $file
	done

	for file in test/yml/*
	do
		echo -e "\nyabs -v $file\n"
		./yabs -V $file
	done
}

memory() {
	for file in test/yml/*
	do
		echo -e "\nvalgrind yabs -p $file\n"
		valgrind --track-origins=yes ./yabs -p $file
	done

	for file in test/yml/*
	do
		echo -e "\nvalgrind yabs -V $file\n"
		valgrind --track-origins=yes ./yabs -V $file
	done
}

_basic() {
	for file in test/yml/*
	do
		echo -e "\n${GRN}yabs -p $file\n${NC}"
		./yabs -p $file
	done

	for file in test/yml/*
	do
		echo -e "\n${GRN}yabs -V $file\n${NC}"
		./yabs -V $file
	done
}

_memory() {
	for file in test/yml/*
	do
		echo -e "\n${GRN}valgrind yabs -p $file\n${NC}"
		valgrind --track-origins=yes ./yabs -p $file
	done

	for file in test/yml/*
	do
		echo -e "\n${GRN}valgrind yabs -V $file\n${NC}"
		valgrind --track-origins=yes ./yabs -V $file
	done
}

case $1 in
	'basic-color')
		norm
		_basic
		;;
	'memory-color')
		norm
		_memory
		;;
	'all-color')
		norm
		_basic
		_memory
		;;
	'basic')
		basic
		;;
	'memory')
		norm
		memory
		;;
	'all')
		norm
		_basic
		_memory
		;;
esac
