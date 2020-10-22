import os
import sys
import glob
import time
import threading
import struct
import socket
import subprocess 
from OSC import OSCClient, OSCMessage, OSCServer

#################################################
# This is Lucibox daemon
# Always running 
# turn off rpi, start and stop service, manage read only
#################################################

################################################
#  MESSAGE from open stage control go directly to python :
# /app /rpi manage app itself and raspberry pi
# others message are forward to puredata, nodejs, openframeworks ..
################################################

################################################
#           PORTS
# Node js in = 9998
# Python in =  12345
# Puredata in = 9999
# Openframeworks in = 9997
################################################


class SimpleServer(OSCServer):
    def __init__(self,t):
        OSCServer.__init__(self,t)
        self.selfInfos = t
        self.addMsgHandler('default', self.handleMsg)

    
    def handleMsg(self,oscAddress, tags, data, client_address):
        global machine
        global client_lucibox
        global client_nodejs
        global client_of
        
        print("OSC message received on : "+oscAddress)

        splitAddress = oscAddress.split("/")
        #DEBUG
        #print(splitAddress)
        
        ############## SERVICE itself #############
        if(splitAddress[1]=="app"):
            # TODO : restart dedicated services here
            if(splitAddress[2]=="close"):
                print("closing the app")
                quit_app()
            if(splitAddress[2]=="start"):
                print("starting the app")
                start_app()
            if(splitAddress[2]=="restart"):
                print("restart the app")
                quit_app()
                time.sleep(2)
                start_app()
        ############## RPI itself #############
        elif(splitAddress[1]=="rpi"):
            if(splitAddress[2]=="shutdown"):
                print("Turning off the rpi")
                powerOff()
            if(splitAddress[2]=="read"):
                read_disk()
            if(splitAddress[2]=="write"):
                write_disk()
        ############# LUCIBOX  ####
        elif(splitAddress[1]=="lucibox"):
            separator="/"
            splitAddress.remove("lucibox")
            finalAddress = separator.join(splitAddress)
            oscmsg = OSCMessage()
            oscmsg.setAddress(finalAddress)
            oscmsg.append(data)
            client_lucibox.send(oscmsg)
            ############# OPENFRAMEWORKS  ####
        elif(splitAddress[1]=="of"):
            separator="/"
            splitAddress.remove("of")
            finalAddress = separator.join(splitAddress)
            oscmsg = OSCMessage()
            oscmsg.setAddress(finalAddress)
            oscmsg.append(data)
            client_of.send(oscmsg)  
        ############ FORWARD TO NODEJS ###
        else :
            oscmsg = OSCMessage()
            oscmsg.setAddress(oscAddress)
            oscmsg.append(data)
            client_nodejs.send(oscmsg)


def powerOff():

    print("========= POWER OFF IN 5 SECONDS ======")
    if sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        time.sleep(5)
        # TODO change it to "sudo poweroff" in terminal directly
        
        os.chdir("/home/patch/lucibox/script/")
        subprocess.call(['./shutdown.sh'])

def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP
        
# This close daemon server
def closing_app():
    global runningApp
    runningApp = False
    print("Closing App")

# There is no APP, only lucibox app, web app, of app
# DEPRECATED, DO NOT USE IT
def quit_app():
    # Change it to sudo systemctl stop lucibox.service
    print("========= QUIT PUREDATA ======")

    os.chdir("/home/patch/lucibox/script/")
    subprocess.call(["./quit_pd.sh"])
    print("======== PUREDATA QUITTED ====")

# There is no APP, only lucibox app, web app, of app
# DEPRECATED, DO NOT USE IT
def start_app():
    global machine
    
    if sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        print("========= START PUREDATA======")
        cmd = ["pd",  "-nogui",  "-jack",  "/media/patch/LUCIBOX/machines/"+str(machine)+"/nogui.pd"]
        subprocess.Popen(cmd)
        print("======== PUREDATA STARTED ====")
        print("========= START OPEN STAGE CONTROL ======")
        cmd = ["node",  "/home/patch/open-stage-control/app",  "-l",  "/home/patch/lucibox/machines/"+str(machine)+"/osc.json", "-s", "127.0.0.1:9999", "-o", "9998"]
        subprocess.Popen(cmd)
        print("========= OPEN STAGE CONTROL STARTED ======")

def write_disk():
    print("========= WRITING ENABLE ======")
    os.chdir("/home/patch/lucibox/script/")
    subprocess.call(['./write_only.sh'])

def read_disk():
    print("========= READONLY ======")
    os.chdir("/home/patch/lucibox/script/")
    subprocess.call(['./read_only.sh'])

def main():
        
        # OSC SERVER      
        # This is a trick to get IP, but 0.0.0.0 allows localhost and external connection
        #myip = get_ip()
        myip = "0.0.0.0"
        print("osc server IP adress is : "+myip)
        try:
            server = SimpleServer((myip, 12345)) 
        except:
            print(" ERROR : creating server") 
        print("server created") 
        try:
            st = threading.Thread(target = server.serve_forever) 
        except:
            print(" ERROR : creating thread") 
        try:
            st.start()
        except:
            print(" ERROR : starting thread")

        print(" OSC server is running") 

        # OSC LUCIBBOX CLIENT
        global client_lucibox
        client_lucibox = OSCClient()
        client_lucibox.connect( ('127.0.0.1', 9999))

        # OSC NODEJS CLIENT
        global client_nodejs
        client_nodejs = OSCClient()
        client_nodejs.connect( ('127.0.0.1', 9998))

        # OSC OF CLIENT
        global client_of
        client_of = OSCClient()
        client_of.connect( ('127.0.0.1', 9997))

        # MAIN LOOP 
        global runningApp
        runningApp = True

        
        print(" ===== STARTING MAIN LOOP ====" )
        while runningApp:
            # This is the main loop
            # Do something here
            try:
                time.sleep(1)
            except:
                print("User attempt to close programm")
                runningApp = False
        
        #CLOSING THREAD AND SERVER
        print(" Ending programme") 
        server.running = False
        print(" Join thread") 
        st.join()
        server.close()
        print(" This is probably the end") 



if __name__ == "__main__":
    main()
