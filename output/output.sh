#!/bin/bash
dmesg --clear;
echo "TIME_MEASUREMENT"
./main < ./OS_PJ1_Test/TIME_MEASUREMENT.txt | tee output/TIME_MEASUREMENT_stdout.txt;
dmesg | grep Project1 | tee output/TIME_MEASUREMENT_dmesg.txt;
dmesg --clear;

for i in {1..5};
do
	echo "FIFO_${i}"
    ./main < ./OS_PJ1_Test/FIFO_${i}.txt | tee ./output/FIFO_${i}_stdout.txt;
    dmesg | grep Project1 | tee ./output/FIFO_${i}_dmesg.txt;
    dmesg --clear;
done

for i in {1..5};
do
	echo "PSJF_${i}"
    ./main < ./OS_PJ1_Test/PSJF_${i}.txt | tee ./output/PSJF_${i}_stdout.txt;
    dmesg | grep Project1 | tee ./output/PSJF_${i}_dmesg.txt;
    dmesg --clear;
done

for i in {1..5};
do
	echo "RR_${i}"
    ./main < ./OS_PJ1_Test/RR_${i}.txt | tee ./output/RR_${i}_stdout.txt;
    dmesg | grep Project1 | tee ./output/RR_${i}_dmesg.txt;
    dmesg --clear;
done

for i in {1..5};
do
	echo "SJF_${i}"
    ./main < ./OS_PJ1_Test/SJF_${i}.txt | tee ./output/SJF_${i}_stdout.txt;
    dmesg | grep Project1 | tee ./output/SJF_${i}_dmesg.txt;
    dmesg --clear;
done