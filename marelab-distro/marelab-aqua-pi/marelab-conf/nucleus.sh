#!/bin/bash
# /etc/init.d/servoblaster

### BEGIN INIT INFO
# Provides:          marelab-nucleus
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: marelab-nucleus deamon for reef tank control
# Description:       marelab-nucleus deamon for reef tank control
### END INIT INFO


case "$1" in 
    start)
        echo "Starting servoblaster"
        START_NUCLEUS_PATH
        ;;
    stop)
        echo "Stopping servoblaster"
        killall marelab_nucleus
        ;;
    *)
        echo "Usage: /etc/init.d/nucleus start|stop"
        exit 1
        ;;
esac

exit 0