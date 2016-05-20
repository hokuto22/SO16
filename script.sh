killall -9 -q ligarServidor
killall -9 -q servidor
killall -9 -q cliente
make

make users-log

make dummy-files

clear
./ligarServidor
sleep 1

./cliente << EOF
1 
Rui
Password
123
1
Rui
123
2
Carlos
Silva
1
Carlos
Silva
backup Carlos/huge.txt
restore Carlos/huge.txt
exit
2
Rui

20
1
Rui

123
1
Claudio
jokas
backup Claudio/1.txt
backup Claudio/2.pdf
backup Claudio/dummy.c
backup Claudio/4.fanecas
restore Claudio/4.fanecas
restore Claudio/3.c
exit
9
EOF

ls -l

cmp Dummies/4.fanecas 4.fanecas
cmp Dummies/3.c 3.c
cmp Dummies/huge.txt huge.txt

killall -9 -q ligarServidor
killall -9 -q servidor
