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
    # $1 = average, $2 = detected value
    # Threshold function: f(x) = 15000*x^(3/5)+x+1000
    awk '{
            y=15000*$1^(3/5)+$1+1000;
            if ($2 < y)
                printf 0;
            else printf 1;
        }' <<< "$1 $2"
}

# Lettura della password da input
read pwd
if [ -f /var/MAST/modules/ta_out ]
then
    saved=$(/opt/MAST/bin/cryptofile -r /var/MAST/modules/ta_out <<< "$pwd")
    if [ $? -eq 1 ]
    then
        exit 1
    fi
else
    saved=""
fi


prevCollection=()
while true
do
    profiling=$(doing_profiling "$saved")

    # Raccolta dei nuovi dati
    timestamp=$(date "+%-d:%-u:%-m:%-H")
    out_new=$(netstat -i | awk '/[0-9]+/ {print $1 ":" $3+$7}')

    if [ $profiling -eq 1 ]
    then
        # Profiling dei dati raccolti da ogni interfaccia di rete
        for i in $out_new
        do
            iface=$(cut -d ':' -f 1 <<< "$i")
            packets=$(cut -d ':' -f 2 <<< "$i")
            packetsDiff=$(awk '{printf $1-$2;}' <<< "$packets ${prevCollection[$iface]}")
            line="$timestamp:$iface:$packetsDiff"
            /opt/MAST/bin/cryptofile -w /var/MAST/modules/ta_out "$line" <<< "$pwd"
            saved=$saved$'\n'$line
            prevCollection[$iface]=$packets
        done
        echo "$(date '+%x %X')|Traffic Analyzer|Profiling" >> /tmp/mast_modules_status
    else
        anomalies=()
        for i in {1..4}
        do
            timeinfo_new=$(cut -d ':' -f $i <<< "$timestamp")

            # Somma del traffico effettuato su ogni interfaccia di rete durante il profiling
            sums=()
            captures=()
            for j in $saved
            do
                timeinfo_old=$(cut -d ':' -f $i <<< "$j")
                if [ $timeinfo_old -eq $timeinfo_new ]
                then
                    iface=$(cut -d ':' -f 5 <<< "$j")
                    packets=$(cut -d ':' -f 6 <<< "$j")
                    sums[$iface]=$((${sums[$iface]}+$packets))
                    captures[$iface]=$((${captures[$iface]}+1))
                fi
            done

            # Calcolo delle medie di traffico effettuato su ogni interfaccia durante il profiling
            avgs=()
            for j in ${!sums[@]}
            do
                avgs[$j]=$(awk '{printf $1/$2}' <<< "${sums[$j]} ${captures[$j]}")
            done

            # Verifica delle anomalie
            for j in $out_new
            do
                iface=$(cut -d ':' -f 1 <<< "$j")
                packets=$(cut -d ':' -f 2 <<< "$j")
                anomaly=$(detect_anomaly ${avgs[$iface]} $packets)
                anomalies[$iface]=$((${anomalies[$iface]}+$anomaly))
            done
        done

        for i in ${!anomalies[@]}
        do
            if [ ${anomalies[$i]} -gt 2 ]
            then
                # Notificazione dell'anomalia all'utente e alla GUI
                /opt/MAST/bin/alert_user "MAST - Traffic Analyzer" "Anomaly found on the interface $i"
                echo "$(date '+%x %X')|Traffic Analyzer|Anomaly|too much data traffic on the interface $i" >> /tmp/mast_modules_status
            fi
        done

        # Aggiornamento sullo stato del modulo
        echo "$(date '+%x %X')|Traffic Analyzer|Running" >> /tmp/mast_modules_status
    fi

    sleep 600 # Aspetta 10 minuti prima di effettuare la prossima scansione
done
