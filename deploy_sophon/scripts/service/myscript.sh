#!/bin/bash
#Switch to shell dir
ulimit -n 20480

dname=`dirname $0`
cd $dname

export LD_LIBRARY_PATH=`pwd`
chmod 777 . * -R

app='sophon-app.soc'
app_id=`pidof $app|wc -l`

if [ $app_id -gt 0 ];
then
 kill -9 $app_id
fi

while true
do
 th=`pidof $app|wc -l`
 if [ $th -eq 0 ];
 then
  ./$app &
  sleep 3
 fi
 sleep 3
done
