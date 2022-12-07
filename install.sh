#!/bin/bash

check_root() {
    # Checking user
    if [ $EUID -ne 0 ]
    then
        printf "\nPlease run this script as root!\n"
        exit 1
    fi
}

check_dependencies() {
    # Checking dependencies
    # Color definitions
    red="\033[0;31m"
    green="\033[0;32m"
    reset="\033[0m"

    # Dependencies lists
    tools=('ldconfig' 'g++' 'perf' 'netstat' 'notify-send' 'zenity')
    libs=('libcryptopp')

    error=0
    printf "\nChecking installed tools...\n"
    for t in ${tools[*]}
    do
        if [ ! $(command -v $t) ]
        then
            printf "[${red}ERROR${reset}] $t not installed!\n"
            error=1
        else
            printf "[${green}INFO${reset}] $t installed!\n"
        fi
    done

    if [ $error -eq 0 ]
    then
        printf "\nChecking libraries...\n"
        for l in ${libs[@]}
        do
            if [ -z "$(ldconfig -p | grep $l)" ]
            then
                printf "[${red}ERROR${reset}] Library $l not found!\n"
                error=1
            else
                printf "[${green}INFO${reset}] Library $l found!\n"
            fi
        done
    fi

    if [ $error -eq 1 ]
    then
        printf "\nSome dependencies can not be found on the system. Please install them before running this installation.\n"
        exit 1
    fi
}

check_installation() {
    if [ $1 -ne 0 ]
    then
        printf "\nThe installation has encountered some errors!\n"
        exit 1
    fi
}

compile() {
    # Compiling binaries
    g++ src/MastCore/mast_core.cpp lib/CryptoFile/source/cryptofile.cpp lib/CryptoFile/source/cryptofilemap.cpp lib/CryptoFile/source/crypto.cpp lib/hasher/hasher.cpp -lpthread -lcryptopp -o bin/mast_core
    check_installation $?
    g++ src/HashGenerator/hashgenerator.cpp lib/CryptoFile/source/cryptofile.cpp lib/CryptoFile/source/cryptofilemap.cpp lib/CryptoFile/source/crypto.cpp lib/hasher/hasher.cpp -lpthread -lcryptopp -o bin/hashgenerator
    check_installation $?
    g++ src/CryptoFile/cryptofile.cpp lib/CryptoFile/source/cryptofile.cpp lib/CryptoFile/source/cryptofilemap.cpp lib/CryptoFile/source/crypto.cpp -lpthread -lcryptopp -o bin/cryptofile
    check_installation $?
}

copy_files() {
    # Copying files
    mkdir -p /opt/MAST /var/MAST /var/MAST/modules && systemctl link ./mast.service 2> /dev/null && cp -rf . /opt/MAST && chmod +x /opt/MAST/bin/*
    check_installation $?

    pwd=$(/opt/MAST/bin/password_asker)
    for module in $(ls "/opt/MAST/modules")
    do
        /opt/MAST/bin/hashgenerator "$module" "$pwd"
    done
}

check_root
check_dependencies
compile
copy_files

echo "Installation terminated successfully!"
