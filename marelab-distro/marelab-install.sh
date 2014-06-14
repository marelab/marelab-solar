#!/bin/bash 
# marelab aqua installation script

# VARIABLES CAN CHANGED BY USER
WWW_PORT=80


VERSION="1.0"
DEFAULT_USER="tomtom"					#only use should be with the deamon
DEFAULT_PASSWORD="password"				#no function yet
#Was old repos becaus of bug with wget usage switched to github
#MARELAB_REPO="https://marelab-solar.googlecode.com/git/marelab-distro"
MARELAB_REPO="https://github.com/marelab/marelab-solar/raw/master/marelab-distro"
PI_REPO="/marelab-aqua-pi"				#switch between differnt OS repos
MARELAB_BASE_DIR=""						#its the dir the script is started from
MARELAB_OS=""							#automatic set to PC or ARM for getting the right distribution
INSTALL_USER=$USER						#the user that starts the script is used as webserver user

function _getinstalldir(){
	SOURCE="${BASH_SOURCE[0]}"
	while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
	  TARGET="$(readlink "$SOURCE")"
	  if [[ $SOURCE == /* ]]; then
 		#echo "SOURCE '$SOURCE' is an absolute symlink to '$TARGET'"
	    SOURCE="$TARGET"
	  else
	    DIR="$( dirname "$SOURCE" )"
	  	#echo "SOURCE '$SOURCE' is a relative symlink to '$TARGET' (relative to '$DIR')"
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


function _checkingPacket(){
	problem=$(dpkg -s $1|grep installed)
 	echo Checking for $1 : $problem
 	if [ "" == "$problem" ]; then
      	echo "No $1 installed. installing it now..."
      	sudo apt-get install $1
 	fi
 }

function _cleanup(){
    echo "removing installation ..."
    echo "bye bye ..."
	rm -rf $MARELAB_BASE_DIR/marelab-aqua
}

function _fail ( ){   
        echo -e $@
        _cleanup                                  
        exit 1                      
}  

function _makedir(){
 	echo "create $1 dir ..."
 	TESTDIR=$1
 	if [ -d "$1" ]; then
        echo " "
        _fail "ERROR: $1 dir couln't created ... \n"
        exit
  	fi
 	mkdir $1
 	return 0
}

function _downloadMarelab(){
 	#finding if linux86 or arm like pi beagle mini2440 etc.
 	foundos=$(uname -m)
 	if [ "$foundos" == "i686" ]; then
    	MARELAB_OS="PC"
 	fi
 	
 	if [ "$foundos" == "armv6l" ]; then
   	 	MARELAB_OS="ARM"
 	fi
 	
 	echo "OS FOUNDED = $MARELAB_OS"
 	if [ "$MARELAB_OS" == "PC" ]; then
    	echo "downloading linux pc based version of marelab..."
    	cd marelab-plug
    	
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-plugin/X86-32/libmarelab-adapter-i2c"
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-plugin/X86-32/libmarelab-plugin-led"
 		cd ..
 		cd marelab-cgi
 		wget "$MARELAB_REPO/marelab-aqua-pi/marelab-cgi/X86-32/marelab-cgi"
 		wget "$MARELAB_REPO/marelab-aqua-pi/marelab-cgi/X86-32/marelab-phcgi"
 		chmod 770 marelab-cgi
 		chmod 770 marelab-phcgi
 	fi	
 	if [ "$MARELAB_OS" == "ARM" ]; then
    	echo "downloading linux ARM based version of marelab..."
    	cd marelab-plug
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-plugin/ARM/libmarelab-adapter-i2c.so"
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-plugin/ARM/libmarelab-plugin-led.so"
 		cd ..
 		cd marelab-cgi
 		wget "$MARELAB_REPO/marelab-aqua-pi/marelab-cgi/ARM/marelab-cgi"
 		wget "$MARELAB_REPO/marelab-aqua-pi/marelab-cgi/ARM/marelab-phcgi"
 		chmod 775 marelab-cgi
 		chmod 775 marelab-phcgi
 	fi
}


function _testdir(){
#Does this directory exist?
TEST_DIR=$1
if [ -d "$TEST_DIR" ]; then
    echo $TEST_DIR already exists. Exiting!
    exit
fi
}

function _isvarempty(){
#Is variable empty?
X=abc
echo "$X"
if [ "$X" == "" ]; then
    echo yes
fi
}

# Detect package type from /etc/issue
function _found_arch() {
  local _ostype="$1"
  shift
  grep -qis "$*" /etc/issue && _OSTYPE="$_ostype"
}

function _pre_install(){
 echo "######## marelab Pre installation ################################"
 cd $MARELAB_BASE_DIR
 _makedir "marelab-aqua"
  cd marelab-aqua
 _makedir "marelab-web"
 _makedir "marelab-cgi"
 _makedir "marelab-conf"
 _makedir "marelab-plug"
 _makedir "marelab-logs"
 _makedir "marelab-deamon"
 _makedir "marelab-scripts"
 _makedir "temp-install"
 
 #Check the needed packages and install them
 	_checkingPacket "boa"
	_checkingPacket "lua5.2"
	_checkingPacket "wget"
	_checkingPacket "sqlite3"
#MAIL PACKAGE
#	_checkingPacket "ssmtp"
#	_checkingPacket "mailutils"
#	_checkingPacket "mpack"

echo "downloading marelab from marelab.org git repository ..."
	_downloadMarelab

cd $MARELAB_BASE_DIR
cd marelab-aqua/marelab-scripts
wget "$MARELAB_REPO/marelab-aqua-pi/marelab-scripts/init.lua"

	
#changing all to user marelab
cd ..
#chown -R marelab:marelab *
	
#CONFIGURE BOA FOR MARELAB
sudo /etc/init.d/boa stop
echo "configure boa webserver for marelab use ..."
echo "getting a clean installed boa.conf from marelab git ..."
cd $MARELAB_BASE_DIR
cd marelab-aqua/temp-install
wget "$MARELAB_REPO/marelab-aqua-pi/marelab-conf/boa.conf"
cd $MARELAB_BASE_DIR
cd marelab-aqua/marelab-web
wget "$MARELAB_REPO/marelab-aqua-pi/marelab-web/marelabwebpack.tar.gz"
tar xfz marelabwebpack.tar.gz
rm marelabwebpack.tar.gz

cd ..
cd ..
#cp /etc/boa/boa.conf $MARELAB_BASE_DIR/marelab-aqua/temp-install     		#first get a boa.conf copy to the temp dir
#CHANGE PARAMETERS OF BOA.CONF
sed -e 's:^ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/:ScriptAlias /cgi/ '"$MARELAB_BASE_DIR"'/marelab-aqua/marelab-cgi/:' \
    -e 's:^DocumentRoot /var/www:DocumentRoot '"$MARELAB_BASE_DIR"'/marelab-aqua/marelab-web:' \
    -e 's:^#VerboseCGILogs:VerboseCGILogs :' \
    -e 's:^#UseLocaltime:UseLocaltime:' \
    -e 's:^User www-data:User '"$INSTALL_USER"':' \
    -e 's:^Group www-data:Group '"$INSTALL_USER"':' \
    -e 's:^ErrorLog /var/log/boa/error_log:ErrorLog '"$MARELAB_BASE_DIR"'/marelab-aqua/marelab-logs/boa_error.log:' \
    -e 's:^AccessLog /var/log/boa/access_log:AccessLog '"$MARELAB_BASE_DIR"'/marelab-aqua/marelab-logs/boa_access.log:' \
    -e 's:^Port 80:Port '"$WWW_PORT"':' < marelab-aqua/temp-install/boa.conf > marelab-aqua/temp-install/boachanged.conf
mv marelab-aqua/temp-install/boachanged.conf marelab-aqua/temp-install/boa.conf
sudo cp marelab-aqua/temp-install/boa.conf /etc/boa/boa.conf

#chown -R $DEFAULT_USER:$DEFAULT_USER *
#VerboseCGILogs    
sudo /etc/init.d/boa start
#!!!changing rights of logs to see
chmod 666 $MARELAB_BASE_DIR/marelab-aqua/marelab-logs/boa_error.log
chmod 666 $MARELAB_BASE_DIR/marelab-aqua/marelab-logs/boa_access.log
echo "boa setup & configuration complete ..." 

#CONFIGURE LINUX LOGGING DEAMON 
/etc/init.d/rsyslog stop
#ADD NEW MARELAB to rssyslog conf
echo "configure rsyslog deamon for for marelab use ..."

/etc/init.d/rsyslog start 
	

}

#configures the OS i2c device bus
function _configureI2Cbus(){
	echo "configure I2C bus on OS ..."
} 

#configures the marelab nucleus deamon and put the init.d script at place
#the deamon is configured to start with the OS automatic
function _configureMarelabNucleus(){

	echo "start configuration of marelab nucleus deamon ..."

cd $MARELAB_BASE_DIR
	cd marelab-aqua/temp-install
	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-conf/marelab.conf"

cd $MARELAB_BASE_DIR
	cd marelab-aqua/marelab-deamon
	
	if [ "$MARELAB_OS" == "PC" ]; then
    	echo "downloading linux pc based version of marelab..."
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-deamon/X86-32/marelab-nucleus"
 	fi	
 	if [ "$MARELAB_OS" == "ARM" ]; then
    	echo "downloading linux ARM based version of marelab..."
    	wget "$MARELAB_REPO/marelab-aqua-pi/marelab-deamon/ARM/marelab-nucleus"
 	fi

	chmod 770 $MARELAB_BASE_DIR/marelab-aqua/marelab-deamon/marelab-nucleus

cd $MARELAB_BASE_DIR
	echo " -> update marelab.conf ..."
	sed -e 's|\"DAEMON_LOCK_PATH\" : \"TEMP\",|\"DAEMON_LOCK_PATH\" : \"'"$MARELAB_BASE_DIR/marelab-aqua/marelab-conf"'\",|' \
		-e 's|\"DEAMON_PATH\" : \"TEMP\",|\"DEAMON_PATH\" : \"'"$MARELAB_BASE_DIR/marelab-aqua/marelab-deamon"'\",|' \
		-e 's|\"PLUGINDIR\" : \"TEMP\",|\"PLUGINDIR\" : \"'"$MARELAB_BASE_DIR/marelab-aqua/marelab-plug"'\",|' \
		-e 's|\"SOCK_PATH\" : \"TEMP\",|\"SOCK_PATH\" : \"'"$MARELAB_BASE_DIR/marelab-aqua/marelab-conf/marelabstreamsocket"'\",|' \
		-e 's|\"DB_PATH\" : \"TEMP\",|\"DB_PATH\" : \"'"$MARELAB_BASE_DIR/marelab-aqua/marelab-logs"'\",|' \
		-e 's|\"RUN_AS_USER\" : \"marelab\",|\"RUN_AS_USER\" : \"'"$DEFAULT_USER"'\",|' < marelab-aqua/temp-install/marelab.conf > marelab-aqua/temp-install/marelabtemp.conf
    mv marelab-aqua/temp-install/marelabtemp.conf marelab-aqua/marelab-conf/marelab.conf
    echo " -> make it start with the OS ...";
} 


clear

echo "marelab aqua install version: $VERSION on port: $WWW_PORT"
echo "---------------------------------------------------------------"
echo "the use of this software is at your own risk. Keep in mind that"
echo "it is a early development and you should understand what you"
echo "are doing here. If you agree the install process continues!"
echo " "

#CREATE MARELAB USER
ret=false
#getent passwd $DEFAULT_USER >/dev/null 2>&1 && ret=true

# check if sudo / root called it
#if [ `/usr/bin/id -u` != "0" ]; then
#	echo " "
#    fail "ERROR: Sorry you must be root to run this script exp: ( sudo marelab-install.sh ) \n"
#    exit
#fi

#if $ret; then
#    echo "ERROR user: $DEFAULT_USER exists already exit install process!"
#    echo "please choose another user or delete the user name if it is a"
#    echo "old marelab installation" 
#    _cleanup
#    exit
#else
#    echo " No, the user does not exist"
#    echo " creating user: $DEFAULT_USER "
#    groupadd $DEFAULT_USER
#	useradd $DEFAULT_USER -g $DEFAULT_USER
#fi
_getinstalldir  						# get the dir the script is in thats the base for all installation & removing 
_pre_install
_configureI2Cbus
_configureMarelabNucleus
# SAFETY MAKE AGAIN ALL FILES TO USER
#echo "$USER"
echo "clean up"
cd $MARELAB_BASE_DIR/marelab-aqua/temp-install
rm *
cd ..
rmdir temp-install
cd ..
#chown $DEFAULT_USER:$DEFAULT_USER * -R 
echo "marelab configured successfull ..."
echo "boa webserver runs under $INSTALL_USER"
echo "just type 'http://localhost:$WWW_PORT/' in your browser...."
echo "have fun bye bye ..."
echo " "
echo " "
