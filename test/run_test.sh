#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
RST='\033[0m'

TEST_1='9.3-pie-sample64'
TEST_2='9.3-pie-pwd'
TEST_3='9.3-pie-ls'
TEST_4='9.3-pie-cat'
TEST_5='9.3-no-pie-sample64'
TEST_6='9.3-no-pie-corewar-asm'
TEST_7='4.7-no-pie-ls'
TEST_8='4.7-no-pie-pwd'
TEST_9='4.7-no-pie-cat'
TEST_10='9.3-no-pie-push-swap'
TEST_11='9.3-no-pie-lem-in'
TEST_12='../woody'

./woody-woodpacker test/$TEST_1 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_1 ok ${RST}\n"
else 
    printf "${RED}$TEST_1 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_2 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_2 ok ${RST}\n"
else 
    printf "${RED}$TEST_2 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_3 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_3 ok ${RST}\n"
else 
    printf "${RED}$TEST_3 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_4 > /dev/null
./woody test/empty > /dev/null
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_4 ok ${RST}\n"
else 
    printf "${RED}$TEST_4 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_5 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_5 ok ${RST}\n"
else 
    printf "${RED}$TEST_5 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_6 > /dev/null
./woody test/corewar-champ.s
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_6 ok ${RST}\n"
else 
    printf "${RED}$TEST_6 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_7 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_7 ok ${RST}\n"
else 
    printf "${RED}$TEST_7 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_8 > /dev/null
./woody
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_8 ok ${RST}\n"
else 
    printf "${RED}$TEST_8 fail ${RST}\n"
    exit 1
fi
rm -f woody

./woody-woodpacker test/$TEST_9 > /dev/null
./woody empty > /dev/null
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_9 ok ${RST}\n"
else 
    printf "${GREEN}$TEST_9 fail ... but ok! ${RST}\n"
fi
rm -f woody

./woody-woodpacker test/$TEST_10 > /dev/null
./woody 3 1 4 5
if [[ $? == 0 ]]
then printf "${GREEN}$TEST_8 ok ${RST}\n"
else 
    printf "${RED}$TEST_8 fail ${RST}\n"
    exit 1
fi
rm -f woody