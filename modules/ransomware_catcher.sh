#!/bin/bash

doing_profiling() {
    # Checks if the profiling is done
    ranges=('1-7' '1-31' '1-12' '0-23')
    for i in ${!ranges[@]}
    do
        start=$(cut -d '-' -f 1 <<< "${ranges[$i]}")
        end=$(cut -d '-' -f 2 <<< "${ranges[$i]}")
        for (( j=$start; j<=$end; j++ ))
        do
            found=$(cut -d ':' -f $(($i+1)) <<< "$1" | grep $j)
            if [ -z "$found" ]
            then
                return 1
            fi
        done
    done
    return 0
}

detect_anomaly() {
    # $1=x=average, $2=detected value
    awk '{
            y=50*$1^(3/5)+5*$1;
            if (y < $2)
                print "1";
            else print "0";
        }' <<< "$1 $2"
}

read -r pwd

if [ -f /var/MAST/modules/rc_out ]
then
    saved=$(/opt/MAST/bin/cryptofile -r /var/MAST/modules/rc_out <<< "$pwd")
    if [ $? -eq 1 ]
    then
        exit 1
    fi
else
    saved=""
fi


while true
do
    doing_profiling "$saved"
    profiling=$?

    if [ $profiling -eq 1 ]
    then
        # Modulo in profiling
        echo "$(date '+%x %X')|Ransomware Catcher|Profiling" >> /tmp/mast_modules_status
    else
        # Modulo in esecuzione
        echo "$(date '+%x %X')|Ransomware Catcher|Running" >> /tmp/mast_modules_status
    fi

    # Raccolta dei nuovi dati
    timestamp=$(date "+%-u:%-d:%-m:%-H")
    out_new=$(perf stat --no-big-num -a -e 'syscalls:sys_enter_read,syscalls:sys_enter_write,syscalls:sys_enter_open' sleep 600 2>&1 | awk '/[0-9]+.+syscalls/ {ORS=/^>/?"\n":""; print $1 ":" $3;}' | cut -d ':' -f 1-3)

    if [ $profiling -eq 1 ]
    then
        line="$timestamp:$out_new"
        saved=$saved$'\n'$line
        /opt/MAST/bin/cryptofile -w /var/MAST/modules/rc_out "$line" <<< "$pwd"
        prova=$(/opt/MAST/bin/cryptofile -r /var/MAST/modules/rc_out <<< "$pwd")
    else
        anomalies=0
        for i in {1..4}
        do
            timeinfo_new=$(cut -d ':' -f $i <<< "$timestamp")
            sums=()
            counts=()
            for j in $saved
            do
                timeinfo_old=$(cut -d ':' -f $i <<< "$j")
                if [ $timeinfo_old -eq $timeinfo_new ]
                then
                    for k in {1..3}
                    do
                        value=$(cut -d ':' -f $(($k+4)) <<< "$j")
                        sums[$k]=$((${sums[$k]}+$value))
                        counts[$k]=$((${counts[$k]}+1))
                    done
                fi
            done

            avgs=()
            for j in {1..3}
            do
                avgs[$j]=$(awk '{print $1/$2;}' <<< "${sums[$j]} ${counts[$j]}")
            done

            anomaly=0
            for j in $out_new
            do
                for k in {1..3}
                do
                    val=$(cut -d ':' -f $k <<< "$j")
                    anomaly=$(detect_anomaly ${avgs[$k]} $val)
                    if [ $anomaly -eq 1 ]
                    then
                        break
                    fi
                done

                if [ $anomaly -eq 1 ]
                then
                    break
                fi
            done

            if [ $anomaly -eq 1 ]
            then
                anomalies=$(($anomalies+1))
            fi
        done

        if [ $anomalies -gt 2 ]
        then
            /opt/MAST/bin/alert_user "MAST - Ransomware Catcher" "Ransomware alert!"
            echo "$(date '+%x %X')|Ransomware Catcher|Anomaly|detected high number of file operations!" >> /tmp/mast_modules_status
        fi
    fi
done
