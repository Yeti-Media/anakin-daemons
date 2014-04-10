#!/bin/sh

#set the current directory to use by the daemon
#argument
#1 -current or a path
function setDirectory() {
	echo "setDirectory: " "$1"
	if [[ "$1" == "-current" ]] ; then
		directory="$(pwd)"
	else
		directory="$1"
	fi
}

#stops the daemon
#arguments
#1 name of the daemon to stop
function stop() {
	start-stop-daemon \
	--stop \
	--name $1
}

#starts the daemon
#arguments
#1 : executable name
#2 : ip
#3 : port
function start() {
	echo "start: " $1 $2 $3
	start-stop-daemon \
	--chdir $directory \
      	--start --oknodo \
	--pidfile $directory/$1".pid" --make-pidfile \
	--startas $1 \
	--background \
	--start -- -iHTTP $3 -oHTTP -verbose
}

#stop and starts the daemon
#arguments
#1 : executable name
#2 : ip
#3 : port
function restart() {
	stop $1
	start $1 $2 $3
}

#calls start or restart
#arguments :
#1 : action (start, restart)
#2 : path to the folder containing the executable (use -current to use the current directory)
#3 : executable name
#4 : ip
#5 : port
function startRestart() {
	echo "startRestart: " $1 $2 $3 $4 $5
	setDirectory $2
	if [[ "$1" == "start" ]] ; then
		start $3 $4 $5
	else
		restart $3 $4 $5
	fi
}


function usage() {
	echo './anakind.sh start / anakin2 127.0.0.1 8000'
	echo 'or' 
	echo './anakind.sh start -current anakin2 127.0.0.1 8000'
	echo 'or' 
	echo './anakind.sh / anakin2 restart 127.0.0.1 8000'
	echo 'or' 
	echo './anakind.sh stop anakin2'
}


#main
#arguments :
#1 : action (start, stop, restart)
	#if action == start then use all args
	#if action == restart the use all args
	#if action == stop use only the third arg
#2 : path to the folder containing the executable (use -current to use the current directory)
#3 : executable name
#4 : ip
#5 : port
echo "arg1 : " $1
echo "arg2 : " $2
echo "arg3 : " $3
echo "arg4 : " $4
echo "arg5 : " $5
error=0
if [[ "$#" < 2 ]] ; then
	echo "need at least two parameters"
	usage
	exit 1
fi
directory=""
if [[ ("$1" == "start" || "$1" == "restart") && $# == 5 ]] ; then
	startRestart $1 $2 $3 $4 $5
	let error=1
fi
if [[ "$1" == "stop" && $# == 2 ]] ; then
	stop $2
	let error=1
fi
if [[ error == 0 ]] ; then
	usage
	exit 1
fi

