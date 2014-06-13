#!/bin/bash          
echo "Writing PH Log"  
while [  1 ]; do
/mnt/marelab/marelab-phcgi/Default/marelab-phcgi -i /dev/i2c-0 -a 41 -c 1 -l    
sleep 10    
done
