#!/bin/bash

TEST_1='resources/9.3-pie-sample64'
TEST_2='resources/9.3-pie-pwd'
TEST_3='resources/9.3-pie-ls'
TEST_4='resources/9.3-pie-cat'
TEST_5='resources/9.3-no-pie-sample64'
TEST_6='resources/9.3-no-pie-corewar-asm'
TEST_7='resources/4.7-no-pie-ls'
TEST_8='resources/4.7-no-pie-pwd'
TEST_9='resources/4.7-no-pie-cat'
TEST_10='resources/9.3-no-pie-push-swap'
TEST_11='resources/9.3-no-pie-lem-in'
TEST_12='woody'

./woody-woodpacker $TEST_1 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_1 ok
else 
    echo $TEST_1 fail
    exit 1
fi

./woody-woodpacker $TEST_2 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_2 ok
else 
    echo $TEST_2 fail
    exit 1 
fi

./woody-woodpacker $TEST_3 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_3 ok
else 
    echo $TEST_3 fail
    exit 1
fi

./woody-woodpacker $TEST_4 > /dev/null
./woody resources/lem-in-map > /dev/null
if [[ $? == 0 ]]
then echo $TEST_4 ok
else 
    echo $TEST_4 fail
    exit 1
fi

./woody-woodpacker $TEST_5 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_5 ok
else 
    echo $TEST_5 fail
    exit 1
fi

./woody-woodpacker $TEST_6 > /dev/null
./woody resources/corewar-champ.cor > /dev/null
if [[ $? == 0 ]]
then echo $TEST_6 ok
else 
    echo $TEST_6 fail
    exit 1
fi

./woody-woodpacker $TEST_7 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_7 ok
else 
    echo $TEST_7 fail
    exit 1
fi

./woody-woodpacker $TEST_8 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_8 ok
else 
    echo $TEST_8 fail
    exit 1
fi

./woody-woodpacker $TEST_10 > /dev/null
./woody 1 4 2 3 > /dev/null
if [[ $? == 0 ]]
then echo $TEST_10 ok
else 
    echo $TEST_10 fail
    exit 1
fi

./woody-woodpacker $TEST_11 > /dev/null
./woody < resources/lem-in-map > /dev/null
if [[ $? == 0 ]]
then echo $TEST_11 ok
else 
    echo $TEST_11 fail
    exit 1
fi


# self infection

./woody-woodpacker $TEST_1 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_8 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi

./woody-woodpacker $TEST_12 > /dev/null
./woody > /dev/null
if [[ $? == 0 ]]
then echo $TEST_12 ok
else 
    echo $TEST_12 fail
    exit 1
fi