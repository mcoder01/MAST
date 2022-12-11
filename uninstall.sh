#!/bin/bash

check_root() {
  if [ $EUID -ne 0 ]
  then
    printf "\nPlease run this script as root!\n"
    exit 1
  fi
}

remove_files() {
  rm -rf /opt/MAST
  systemctl stop mast.service 2> /dev/null
  systemctl disable mast.service 2> /dev/null
  rm -f /etc/systemd/system/mast.service 2> /dev/null
  rm -f /usr/lib/systemd/system/mast.service 2> /dev/null
  systemctl daemon-reload
  systemctl reset-failed
  rm -f /tmp/mast_modules_status 2> /dev/null
}

remove_data() {
  read -p "Do you also want to delete all program data? (s/N): " resp
  if [ "$resp" == "S" ] || [ "$resp" ==  "s" ]
  then
    rm -rf /var/MAST
  fi
}

check_root
remove_files
remove_data
