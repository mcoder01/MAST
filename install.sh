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

    echo ""
}

check_installation() {
    if [ $1 -ne 0 ]
    then
        printf "\nThe installation has encountered some errors!\n"
        exit 1
    fi
}

compile() {
    # Compiling libraries
    echo "Compiling libraries..."
    mkdir -p lib/CryptoFile/obj && g++ -c lib/CryptoFile/source/crypto.cpp -o lib/CryptoFile/obj/crypto.a; g++ -c lib/CryptoFile/source/cryptofile.cpp -o lib/CryptoFile/obj/cryptofile.a; g++ -c lib/CryptoFile/source/cryptofilemap.cpp -o lib/CryptoFile/obj/cryptofilemap.a
    g++ -c lib/hasher/hasher.cpp -o lib/hasher/hasher.a

    # Compiling binaries
    echo "Compiling binaries..."
    g++ src/MastCore/mast_core.cpp lib/CryptoFile/obj/cryptofile.a lib/CryptoFile/obj/cryptofilemap.a lib/CryptoFile/obj/crypto.a lib/hasher/hasher.a -lpthread -lcryptopp -o bin/mast_core
    check_installation $?
    g++ src/HashGenerator/hashgenerator.cpp lib/CryptoFile/obj/cryptofile.a lib/CryptoFile/obj/cryptofilemap.a lib/CryptoFile/obj/crypto.a lib/hasher/hasher.a -lpthread -lcryptopp -o bin/hashgenerator
    check_installation $?
    g++ src/CryptoFile/cryptofile.cpp lib/CryptoFile/obj/cryptofile.a lib/CryptoFile/obj/cryptofilemap.a lib/CryptoFile/obj/crypto.a -lpthread -lcryptopp -o bin/cryptofile
    check_installation $?
}

copy_files() {
    # Copying files
    echo "Copying files..."
    mkdir -p /opt/MAST /var/MAST /var/MAST/modules && systemctl link ./mast.service 2> /dev/null && cp -rf . /opt/MAST && chmod +x /opt/MAST/bin/*
    check_installation $?

    echo "Generating hashes..."
    while true
    do
        pwd=$(/opt/MAST/bin/password_asker --new)
        if [ ${#pwd} -ge 6 ]
        then
            break
        fi
    done

    while [ ${#pwd} -lt 16 ]
    do
        pwd=$pwd"M"
    done

    if [ ${#pwd} -gt 16 ]
    then
        pwd=${pwd:0:16}
    fi

    for module in $(ls "/opt/MAST/modules")
    do
        /opt/MAST/bin/hashgenerator module "/opt/MAST/modules/$module" <<< "$pwd"
    done

    for bin in $(ls "/opt/MAST/bin")
    do
        if [ "$bin" != "mast_core" ]
        then
            /opt/MAST/bin/hashgenerator binary "/opt/MAST/bin/$bin" <<< "$pwd"
        fi
    done
}

check_root
check_dependencies
compile
copy_files

echo "Installation terminated successfully!"
