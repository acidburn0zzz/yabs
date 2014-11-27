#!/bin/sh

GRN='\033[0;32m'
NC='\033[0m'

./yabs -h
./yabs -n
./yabs -d
./yabs -e include.tar.gz

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
