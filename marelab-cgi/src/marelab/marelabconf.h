/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Author      : Marc Philipp Hammermann
 * Version     :
 * Copyright © 2013 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 *
 *
 *
 */

#ifndef MARELABCONF_H_
#define MARELABCONF_H_

/* Change this to whatever your daemon is called */
//#define DAEMON_NAME "marelab-nucleus"

/* Change this to whatever your daemon should place his PID lock file */
//#define DAEMON_LOCK_PATH "/mnt/marelab/marelab-nucleus/marelab_cfg/"
//#define DAEMON_NAME_LOCK "marelab-nucleus-lock"
#define DEAMON_PATH "/home/pi/nucleus"

/* Change this to whatever your daemon places the ipc*/
//#define DAEMON_PIPE_IN "/home/marelab/msp"
//#define DAEMON_PIPE_OUT "/home/marelab/msp_out"

/* Change this to the user under which to run */
#define RUN_AS_USER "marelab"

//#define DAY_SENSE_POINT 48
/* FIFO PIPE BUFFER SIZE */
#define TRANSFER_BUFFER 9192
// FOR LINUX
//#define SOCK_PATH "/mnt/marelab/marelab-nucleus/marelab_cfg/marelab_socket"
// FOR PI
#define SOCK_PATH "/home/pi/nucleus/marelab_socket"



//#define CONFIGFILE "/mnt/marelab/marelab-nucleus/marelab_cfg/marelabsolar.cfg"
//#define CONFIGFILETEST "/mnt/arm-marelab-dev/marelab-deepblue/marelab-nucleus/marelab_cfg/marelabsolartest.cfg"

//#define PLUGINDIR "/mnt/marelab/marelab-nucleus/plugins/"

#define nucleusversion "Nucleus 0.2"

#endif /* MARELABCONF_H_ */
