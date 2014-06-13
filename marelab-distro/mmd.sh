#!/bin/bash 
# creates marelab distribution
# - copys files from the developer projects
# - packs webfiles to archive


MARELAB_BASE_DIR=""


function _getinstalldir(){
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  TARGET="$(readlink "$SOURCE")"
  if [[ $SOURCE == /* ]]; then
 #   echo "SOURCE '$SOURCE' is an absolute symlink to '$TARGET'"
    SOURCE="$TARGET"
  else
    DIR="$( dirname "$SOURCE" )"
  #  echo "SOURCE '$SOURCE' is a relative symlink to '$TARGET' (relative to '$DIR')"
    SOURCE="$DIR/$TARGET" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
  fi
done
#echo "SOURCE is '$SOURCE'"
RDIR="$( dirname "$SOURCE" )"
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
#if [ "$DIR" != "$RDIR" ]; then
 # echo "DIR '$RDIR' resolves to '$DIR'"
#fi
#echo "DIR is '$DIR'"
MARELAB_BASE_DIR=$DIR
}


WEB_ARCHIEV=marelabwebpack.tar.gz
_getinstalldir
cd $MARELAB_BASE_DIR 
cd ..
cd marelab-web
 if [ -f "$WEB_ARCHIEV" ]; then
 		echo "del old $WEB_ARCHIEV"
        rm $WEB_ARCHIEV
 fi
 
tar -czf marelabwebpack.tar.gz *
mv $WEB_ARCHIEV $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-web/$WEB_ARCHIEV

clear
echo "move executables x86 & ARM to distribution directory"
cd $MARELAB_BASE_DIR 
cd ../marelab-adapter-i2c/Default
cp libmarelab-adapter-i2c $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-plugin/X86-32/libmarelab-adapter-i2c
cd ../DebugCrossPi
cp libmarelab-adapter-i2c.so $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-plugin/ARM/libmarelab-adapter-i2c.so


cd $MARELAB_BASE_DIR 
cd ../marelab-plugin-led/Default
cp libmarelab-plugin-led $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-plugin/X86-32/libmarelab-plugin-led
cd ../DebugCrossPi
cp libmarelab-plugin-led.so $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-plugin/ARM/libmarelab-plugin-led.so

cd $MARELAB_BASE_DIR 
cd ../marelab-cgi/Default
cp marelab-cgi $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-cgi/X86-32/marelab-cgi
cd ../DebugCrossPi
cp marelab-cgi $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-cgi/ARM/marelab-cgi

cd $MARELAB_BASE_DIR 
cd ../marelab-phcgi/Default
cp marelab-phcgi $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-cgi/X86-32/marelab-phcgi 
cd ../DebugCrossPi
cp marelab-phcgi $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-cgi/ARM/marelab-phcgi

cd $MARELAB_BASE_DIR 
cd ../marelab-nucleus/Default
cp marelab-nucleus $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-deamon/X86-32/marelab-nucleus
cd ../PiDebugCrossCompile
cp marelab-nucleus $MARELAB_BASE_DIR/marelab-aqua-pi/marelab-deamon/ARM/marelab-nucleus
echo "FINISH DISTRIBUTION"




