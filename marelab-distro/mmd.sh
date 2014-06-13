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


#entpacken tar xvfz datei.tar.gz
