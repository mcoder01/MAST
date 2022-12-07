#!/bin/bash

doing_profiling() {
    # Checks if the profiling is done
    ranges=('1-31' '1-7' '1-12' '0-23')
    for i in {1..4}
    do
        start=$(cut -d '-' -f 1 <<< "${ranges[$i]}")
        end=$(cut -d '-' -f 2 <<< "${ranges[$i]}")
        for j in {$start..$end}
        do
            found=$(awk -F ':' '{print $1}' <<< "$1" | grep $j)
            if [ -z "$found" ]
            then
                echo 1
                return
            fi
        done
    done
    echo 0
}

detect_anomaly() {
    # $1=x=average, $2=detected value
    awk '{
            y=50*$1^(3/5)+5*$1;
            if (y < $2)
                printf 1;
            else printf 0;
        }' <<< "$1 $2"
}

read pwd

if [ -f /var/MAST/modules/rc_out ]
then
    saved=$(/opt/MAST/bin/cryptofile -r /var/MAST/modules/rc_out <<< "$pwd")
    if [ $? -eq 1 ]
    then
        exit 1
    fi
else
    saved = ""
fi


while true
do
    profiling=$(doing_profiling "$saved")

    # Raccolta dei nuovi dati
    timestamp=$(date "+%-d:%-u:%-m:%-H")
    out_new=$(perf stat --no-big-num -a -e 'syscalls:sys_enter_read,syscalls:sys_enter_write,syscalls:sys_enter_open' sleep 600 2>&1 | awk '{ORS=/^>/?"\n":""; s=((NR+2)%3==0)?"\n":""; print $2 " " $3 " " s; fflush();}' | awk '{print $1 "-" $3 "-" $5; fflush();}' | grep -v --line-buffered 'time\|counts')

    if [ $profiling -eq 1 ]
    then
        line="$timestamp:$out_new"
        saved=$saved$'\n'$line
        /opt/MAST/bin/cryptofile -w /var/MAST/modules/rc_out "$line" <<< "$pwd"
        echo "$(date '+%x %X')|Ransomware Catcher|Profiling" >> /tmp/mast_modules_status
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
                avgs=$(awk '{printf $1/$2}' <<< "${sums[$j]} ${counts[$j]}")
            done

            anomaly=0
            for j in $out_new
            do
                anomalies=0
                for k in {1..3}
                do
                    val=$(cut -d ':' -f $k <<< "$j")
                    da=$(detect_anomaly ${avgs[$k]} $val)
                    anomaly=1
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
        echo "$(date '+%x %X')|Ransomware Catcher|Running" >> /tmp/mast_modules_status
    fi
done
