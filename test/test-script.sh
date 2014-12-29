#!/bin/sh

GRN='\033[0;32m'
NC='\033[0m'

./yabs -h
./yabs -n
./yabs -d
./yabs -e include.tar.gz
./yabs -p test/test-script.sh
./yabs -p test/yml
./yabs -p no-file
./yabs -m yabs.ybf

basic() {
	for file in test/yml/*
	do
		echo -e "\nyabs -p $file\n"
		./yabs -p $file
	done

	for file in test/yml/*
	do
		echo -e "\nyabs -v $file\n"
		./yabs -v $file
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
		echo -e "\nvalgrind yabs -v $file\n"
		valgrind --track-origins=yes ./yabs -v $file
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
		echo -e "\n${GRN}yabs -v $file\n${NC}"
		./yabs -v $file
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
		echo -e "\n${GRN}valgrind yabs -v $file\n${NC}"
		valgrind --track-origins=yes ./yabs -v $file
	done
}

case $1 in
	'basic-color')
		_basic
		;;
	'memory-color')
		_memory
		;;
	'all-color')
		_basic
		_memory
		;;
	'basic')
		basic
		;;
	'memory')
		memory
		;;
	'all')
		basic
		memory
		;;
esac
