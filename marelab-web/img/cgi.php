<?php
//echo $_POST;

// LED ARRAYS LESEN
if ($_POST['GETLEDARRY']==1)
{
  $arr = array ('LED1'=>"100,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,100");
  echo json_encode($arr);
}
if ($_POST['GETLEDARRY']==2)
{
  $arr = array ('LED1'=>"10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0");
  echo json_encode($arr);
}
if ($_POST['GETLEDARRY']==3)
{
  $arr = array ('LED1'=>"10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0");
  echo json_encode($arr);
}

// Gibt alle LEDS ZURÜCK
// www<-marelab
////////////////////////////////
if ($_POST['GETLEDS']==1)
{
  $arr = array (array('LED_CH_NAME'=>"RED",'LED_CH_VALUE'=>array(10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0)),
				array('LED_CH_NAME'=>"GREEN",'LED_CH_VALUE'=>array(10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0)),
				array('LED_CH_NAME'=>"BLUE",'LED_CH_VALUE'=>array(10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0)),
				array('LED_CH_NAME'=>"CREE",'LED_CH_VALUE'=>array(10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0)),
				array('LED_CH_NAME'=>"MOON",'LED_CH_VALUE'=>array(10,20,30,40,50,60,70,80,90,100,90,80,70,60,50,40,30,20,10,0,0,0,0,0)));
  echo json_encode($arr);
}

// Setzt alle LEDS in marlab
// www->marelab
////////////////////////////////
if ($_POST['SETMARELABLEDS']==1)
{
	$ledarr = $_POST['LEDARRAY'];
	$arr = array ('LEDARR'=>"$ledarr");
	echo json_encode($arr);
}

// Setzt Zeit in Marelab
// www->marelab
////////////////////////////////
if (isset($_POST['SENDRTC2MARELAB']))
{
	$Zeit = $_POST['SENDRTC2MARELAB'];
	$arr = array ('ZEIT'=>"$Zeit");
	echo json_encode($arr);
}

// Holt RTC Datum und Uhrzeit vom marelab
// www<-marelab
////////////////////////////////
if ($_POST['GETRTC']==1)
{
	$arr = array('MTIME'=>"12:12:00",'MDATE'=>"01.01.12");
	echo json_encode($arr);
}

// Holt NTP Datum und Uhrzeit vom marelab
// www<-marelab
////////////////////////////////
if ($_POST['GETNTP']==1)
{
	$arr = array('MTIME'=>"13:13:00",'MDATE'=>"01.01.12");
	echo json_encode($arr);
}


?>
