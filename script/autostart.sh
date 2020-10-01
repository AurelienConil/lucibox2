#!/bin/sh
sleep 1
#node /home/patch/open-stage-control/app -l /home/patch/lucibox/machines/6/osc.json -s 127.0.0.1:9999 -o 9998&
#python /media/patch/LUCIBOX/script/python_daemon.py
pd -nogui -jack /home/patch/lucibox/myapp/mp/nogui.pd
