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
 * Name        : LedDimmer.js
 * Author      : Marc Philipp Hammermann
 * Version     : 1.0
 * Copyright © 2012 marc philipp hammermann  <marchammermann@googlemail.com>
 *
 * Description : This is the dynamic LED Dimmer HTML Plugin for marelab
 *
 * 12.12.12 - Fixed several JS Problems that brought by the dynamic load process a delay 
 *            between loading JS files fixed it. The Graph is now showing correctly.
 *          - Fixed the Tabel Grid editing ADD DEL is now possible 
 *          - Added Syncronisation between Table & Graph Grid
 *
 */

// Delay function needed because Script files getting loaded as defined, somtimes later defined 
// functions loaded first
function pausecomp(millis) {
	var date = new Date();
	var curDate = null;

	do {
		curDate = new Date();
	} while (curDate - date < millis);
} 


loadjscssfile("script/css/ui.jqgrid.css", "css");
pausecomp(10);
loadjscssfile("script/src/i18n/grid.locale-en.js", "js");
pausecomp(10);
//jQuery.jgrid.no_legacy_api = true;
loadjscssfile("script/js/jquery.jqGrid.src.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.base.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.common.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.custom.js", "js");
pausecomp(10);
loadjscssfile("script/src/jquery.fmatter.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.jqueryui.js", "js");
pausecomp(10);
loadjscssfile("script/src/jqModal.js", "js");
pausecomp(10);
loadjscssfile("script/src/jqDnR.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.formedit.js", "js");
pausecomp(10);
loadjscssfile("script/src/grid.inlinedit.js", "js");
pausecomp(10);
loadjscssfile("script/RGraph/libraries/RGraph.common.core.js", "js");
pausecomp(10);
loadjscssfile("script/RGraph/libraries/RGraph.common.dynamic.js", "js");
pausecomp(10);
loadjscssfile("script/RGraph/libraries/RGraph.common.context.js","js");
pausecomp(10);
loadjscssfile("script/RGraph/libraries/RGraph.line.js", "js");



function Stream() {
	alert("Hello world I'm form the Plugin Stream");
}

var line; 										// RGraph Chart object
var canvas_width = ($(window).width() - 150);	// RGraph canvas width

var leds = [];						// Array of Array with dim values 24H a 30min
var keyLeds = [];					
var LDI2CHANNEL = [];				// Channel of the Led String
var LDNUMBER = [];					
var STREAMCHARTCOLOR = [];				// Array from nucleus config with color for the Graph Line
var foodpower = 0;
var foodtime = 0;

var render_width;
var plugin_info = "marelab";
var timers = {};

var selected_row=0;

var waitForFinalEvent = (function() {
	var timers = {};
	return function(callback, ms, uniqueId) {
		if (!uniqueId) {
			uniqueId = "Don't call this twice without a uniqueId";
		}
		if (timers[uniqueId]) {
			clearTimeout(timers[uniqueId]);
		}
		timers[uniqueId] = setTimeout(callback, ms);
	};
})();

// Window Resize so we resize the RGRAPH CANVAS
$(window).resize(function() {
	waitForFinalEvent(function() {
		StreamGraph2Data();
		StreamReSizeCanvas();
		line = StreamGraph("stream", leds, keyLeds);
	}, 500, "some unique string");
});


////////////////////////////////////////////////////////////////////////////
// INIT OF PLUGIN
// PLACE ALL THINGS THAT HAVE TO BE DONE BEFORE THE CONTENT IS INSTERTED
////////////////////////////////////////////////////////////////////////////
function Stream_Init() {
	//LoadAdditionalPluginScripts();
	//line = GetLedGraph("cvs", leds, keyLeds);	
	$("#BUTTON_SAVELED").button( {icons: {primary: "ui-icon-refresh ui-icon-refresh-1-n"}, text: true } );
	$("#BUTTON_RGBLED").button( {icons: {primary: "ui-icon-refresh ui-icon-refresh-1-n"}, text: true } );
	$("#BUTTON_RGB2LED").button( {icons: {primary: "ui-icon-refresh ui-icon-refresh-1-n"}, text: true } );
	
	
	
	
	StreamGetDataToGraph();
	line = StreamGraph("stream", leds, keyLeds);
	StreamGraph2Data();
	
	// The drawing again is needed to resize to the browser size
	StreamReSizeCanvas();
	line = StreamGraph("stream", leds, keyLeds);
	
	// Event Listener for Context Menu (RGB)
	RGraph.AddCustomEventListener(line, 'oncontextmenu', myListener);
	
	// Add the pulldown for RGB Selcet to the RGB Dialog
	GenerateLedDropDown();
	
	// Close the table led grid on init
	jQuery("#streamlist").setGridState('hidden');
	
	
	
	
	$( "#eq > span" ).each(function() {
	      // read initial values from markup and remove that
	      var value = parseInt( $( this ).text(), 10 );
	      $( this ).empty().slider({
	        value: value,
	        range: "min",
	        animate: true,
	        orientation: "vertical"
	      });
	    });
	
}

//GRAPH DRAWINGS
//formats the json cgi result to use by RGraph
function StreamGetDataToGraph() {
	//line = GetLedGraph("timer",moon, light1, light2, light3);
	var STREAMS = getMareStreamTableRender();

	leds = [];
	keyLeds = [];
	LDI2CHANNEL = [];
	LDNUMBER = [];
	STREAMCHARTCOLOR=[];

	for (x in STREAMS.StreamListe) {
		leds[x] = STREAMS.StreamListe[x].cell[3];
		keyLeds[x] = STREAMS.StreamListe[x].cell[1];
		LDI2CHANNEL[x] = STREAMS.StreamListe[x].cell[2];
		LDNUMBER[x] = STREAMS.StreamListe[x].cell[0];
		STREAMCHARTCOLOR[x] = STREAMS.StreamListe[x].cell[4];
	}
	
	
}

//Resizes the canvas after the size of the sourounding div & redraws it
function StreamReSizeCanvas() {
	div = $("#RECHTS").width();
	var c = document.getElementById('stream');
	if (c != null) {
		render_width = div;
		c.width = div;
		console.log("REMOVE GRAPH");
		RGraph.ObjectRegistry.Clear();
	}
}


// CHART HANDLING SETTING & GETTING DATA FROM THE
// GRID CHART OF LEDS
function Convert2Int(arr) {
	var stringLED = "";
	for ( var i = 0, len = arr.length; i < len; i++) {
		arr[i] = parseInt(arr[i], 10);
		stringLED = stringLED + arr[i];
		if (i < (arr.length - 1))
			stringLED = stringLED + ",";
	}
	return "[" + stringLED + "]";
}

function StreamGraph(id, arrayLeds, keyLeds) {
	console.log("Created Graph");
	line = new RGraph.Line(id, arrayLeds);
	line.Set('chart.colors',STREAMCHARTCOLOR);
	line.Set('chart.linewidth', 2);
	line.Set('chart.background.grid.autofit', true);
	line.Set('chart.background.grid.autofit.numhlines', 10);
	line.Set('chart.background.grid.autofit.numvlines', 47);
	line.Set('chart.hmargin', 0);
	line.Set('chart.shadow', true);
	line.Set('chart.adjustable', true);
	line.Set('chart.title', 'Percent over 24 hour period');
	line.Set('chart.title.vpos', 0.5);
	//line.Set('chart.spline', true);
	line.Set('chart.tickmarks', 'circle');
	line.Set('chart.background.grid.color', 'rgba(57,82,99,1)');
	line.Set('chart.title.color', 'rgba(57,82,99,1)');
	line.Set('chart.title.yaxis.color', 'rgba(57,82,99,1)');
	line.Set('chart.axis.color', 'rgba(57,82,99,1)');
	line.Set('chart.text.color', 'rgba(57,82,99,1)');
	line.Set('chart.numxticks', 47);
	line.Set('chart.labels', [ '00','', '1','', '2','', '3','', '4', '', '5', '', '6','', '7','',
			'8','', '9','', '10','', '11','', '12','', '13','', '14','', '15','', '16','', '17','', '18','',
			'19','', '20','', '21','', '22','', '23','']);
	line.Set('chart.ymin', 0);
	line.Set('chart.ymax', 100);
	line.Draw();
	line.Set('chart.contextmenu', [['RGB', OpenRGBDialog]]);

	return line;
}

/*
// Gets all LEDs & LED Texte from marelab
///////////////////////////////////////////////////////				
 */
function getMareStreamTableRender() {
	var jsonLEDS = getMarelabData('COMMAND=READ_CONFIG&PARAMETER="PLUGIN":"Stream"');
	foodpower = jsonLEDS.foodpower;
	foodtime = jsonLEDS.fooddelay;
	// Drawing the editable table data of the Ledstings
	renderStreamTable(jsonLEDS);
	return jsonLEDS;
}


/*
 *Gets the LED settings from the graphic
 */
function StreamGraph2Data() {
	for ( var x = 0; x < line.original_data.length; x++) {
		for ( var y = 0; y < line.original_data[x].length; y++) {
			leds[x][y] = line.original_data[x][y];
		}
	}
}

$(function() {
	//$("#accordion").accordion();
	$("#BUTTON_SAVELED").button({
		icons : {
			primary : "ui-icon-refresh ui-icon-refresh-1-n"
		},
		text : false
	});
	$("#accordion").accordion({
		heightStyle : "fill"
	});
});



////////////////////////////////////////////////////////////////////
// RENDER TABLE GRID                                              //
////////////////////////////////////////////////////////////////////
/*
 * Renders the json LED result to a html table				
 */
function FormatTableData(Data) {
	var data;
	//  "total": "xxx", 
	//  "page": "yyy", 
	//  "records": "zzz",
	//  "rows" : [
	//    {"id" :"1", "cell" :["cell11", "cell12", "cell13"]},
	//    {"id" :"2", "cell":["cell21", "cell22", "cell23"]},
	//  ]
	data = {
		"total" : "1",
		"page" : "1",
		"records" : "" + Data.LedListe.length + "",
		"rows" : []
	}

	for (x in Data.LedListe) {
		//var data = {id:"'"+jsonLEDS.LedListe[x].LedString.LDNUMBER +"'",invdate:"'"+jsonLEDS.LedListe[x].LedString.LDNAME+"'",name:"'"+jsonLEDS.LedListe[x].LedString.LDI2CHANNEL+"'"};
		//var row = {"id":Data.LedListe[x].LedString.LDNUMBER, "cell" :[]};
		//row.cell=[Data.LedListe[x].LedString.LDNUMBER,Data.LedListe[x].LedString.LDNAME,Data.LedListe[x].LedString.LDI2CCHANNEL];
		//var row = {"id":Data.LedListe[x].LedString.LDNUMBER, "cell" :[]};
		var row = [ Data.LedListe[x].LedString.LDNUMBER,
				Data.LedListe[x].LedString.LDNAME,
				Data.LedListe[x].LedString.LDI2CCHANNEL ];
		data.rows[x] = {
			"id" : "" + Data.LedListe[x].LedString.LDNUMBER + "",
			"cell" : []
		}
		data.rows[x].cell = row;
		//data.rows[x]=row;		
	}
	return data;
}


function renderStreamTable(jsonLEDS) {
	// DYN TABLE LEDS
	var lastsel;
	var mydata = [];

	$("#streamlist")
			.jqGrid(
					{
						datatype : "json",

						jsonReader : {
							total : function(obj) {
								return "1"
							},
							page : function(obj) {
								return "1"
							},
							repeatitems : true,
							root : "StreamListe",
							records : function(obj) {
								console.log("JSON TABLE READER");
								console.log(obj);
								return obj.StreamListe.size;
							}
						},

						height : 160,
						width : 400, 
						colNames : [ 'No', 'Description', 'Channel' , 'Color'],
						colModel : [ {
							name : 'LDNUMBER',
							index : 'LDNUMBER',
							width : 40,
							sorttype : "int"
						}, {
							name : 'LDNAME',
							index : 'LDNAME',
							width : 280,
							sortable : false,
							editable : true,
							edittype : "text"
						}, {
							name : 'LDI2CCHANNEL',
							index : 'LDI2CCHANNEL',
							width : 40,
							editable : true,
							edittype : "text"
						},{ 
							name: 'add', width: 40, sortable: false, search: false,
		                      formatter:function(){
		                    	  return "<span class='ui-icon ui-icon-plus' ></span>"
		                      }
						},
		                ],
						// multiselect: true,
						// pagination:true,
						pager : $('#PAGESTREAM'),
						rowNum : 10,
						rowList : [ 5, 10, 20, 50 ],
						sortname : "id",
						sortorder : "desc",
						viewrecords : true,
						// scroll: true,
						scrollrows : true,

						onSelectRow : function(id) {
							if (id && id !== lastsel) {
								jQuery('#streamlist').jqGrid('restoreRow', lastsel);
								lastsel = id;
								jQuery('#streamlist').jqGrid('editRow', id, true);
							}
							console.log("edit row data");
						},
						
						/*afterInsertRow: function(rowid,rowdata,rowelem){
							alert ("RowInsert");
						},
						*/
						// evertime a complete event is made something might have changed on the table
						// so the grid is refrehed to reflect the table changes
						gridComplete:function(){
							// Set the color of the ID colum with the color of thev value retrieved 
							// from config
							
							StreamGetDataToGraph();
							StreamReSizeCanvas();
							line = StreamGraph("stream", leds, keyLeds);
							colorobj = new Object();
							for (i=0; i <=  $("#streamlist").getGridParam("reccount");i++) {
								colorobj.background = STREAMCHARTCOLOR[i];
								//alert ("ROW:"+i+" COLOR: "+CHARTCOLOR[i]);
								$("#streamlist").setCell (LDNUMBER[i],0,"",colorobj);
							}
						},
						
			
						beforeRequest:function(){
							console.log("TABLE BEFORE REQUEST DATA beforeRequest");
							// First Save the Graph Curve because maybe something has been edit
							// while adding / deleting a new led string 
							//console.log("save graph");
							//led_SAVECURVE();
						},
				
						serializeRowData : function(postData) {
						
								console.log("TABLE BEFORE REQUEST DATA serializeRowData");
								
							/*
							 * prePOST from edit ROW the postData gets
							 * rearranged adding the LEDCHART data to it and the
							 * COMMAND so marelab-deamon knows what to do
							 */
							
							console.log(postData);
							console.log("save table");
							sendCommand = new Object();
							sendCommand.COMMAND = "SAVELEDROW";
							sendCommand.LDID = postData.id;
							sendCommand.LDNAME = postData.LDNAME;
							sendCommand.LDI2CCHANNEL = postData.LDI2CCHANNEL;
							sendCommand.LDLEDARRAY = Convert2Int(line.original_data[postData.id - 1]);
							sendCommand.CHARTCOLOR = STREAMCHARTCOLOR[(postData.id-1) ]
							console.log(sendCommand);
							return sendCommand;
						},

						url : '/cgi/marelab-cgi?COMMAND=READ_CONFIG&PARAMETER="PLUGIN":"Stream"',
						editurl : '/cgi/marelab-cgi?COMMAND=SAVE_CONFIG&PARAMETER="PLUGIN":"Stream"',
						mtype : "POST",
						// editUrl:'/clientArray',
						caption : "<h2>Stream Setup</h2>",
						
						 onCellSelect :  function(rowid, index, contents, event) {
							 if (index == 3) {
								 //alert(rowid);
								
								 selected_row =rowid
								 StreamColorDialog();
							 }
                         }
						
					});
	
	 // Before we save edited Data we save the graph!
	 $.extend($.jgrid.edit, {
         beforeSubmit: function (postData) {
        	 console.log(" BEFORE SUBMIT save graph");
        	 StreamGraph2Data();
        	 //led_SAVECURVE();
        	 return [true, ''];
         }
     });

	

	jQuery("#streamlist").jqGrid('navGrid','#PAGESTREAM', {
		del : true,
		add : true,
		edit : true,
		search : false
	});	
	
	
}


function StreamColorDialog(){
	StreamGraphRgbSelector();
}

function Streamcallback(color) { 
	colorobj = new Object();
	colorobj.background = color;
	STREAMCHARTCOLOR[(selected_row-1)]=color;
	jQuery("#streamlist").setCell (selected_row,0,"",colorobj);
}

var StreamGraphRgbSelector = function() {
	$('#colorpicker').farbtastic(Streamcallback);
	$('#GraphRgbSelector' ).dialog();
}



// //////////////////////////////////////////////////////////////////
// DAEMON COMMUNICATION                                           //
////////////////////////////////////////////////////////////////////

// Saves the LED to a file over the deamon 
function stream_SAVECURVE() {
	var ledarr = [];
	var stringLED = "";
	for (x in line.original_data) {
		if (x != 0)
			stringLED = stringLED + "|";

		ledarr = Convert2Int(line.original_data[x]);
		for ( var i = 0; i < ledarr.length; i++) {
			stringLED = stringLED + ledarr[i];
			if (i < (ledarr.length - 1))
				stringLED = stringLED + ",";
		}
		sendCommand = new Object();
		sendCommand.COMMAND = "SAVELEDROW";
		sendCommand.LDID = LDNUMBER[x];
		sendCommand.LDNAME = keyLeds[x];
		sendCommand.LDI2CCHANNEL = LDI2CHANNEL[x];
		sendCommand.LDLEDARRAY = ledarr;
		sendCommand.CHARTCOLOR = STREAMCHARTCOLOR[x];
		para = '"PLUGIN":"Stream","LDID":"' + sendCommand.LDID + 
				'","LDNAME":"' + sendCommand.LDNAME + 
				'","LDI2CCHANNEL":"'+ sendCommand.LDI2CCHANNEL + 
				'","LDLEDARRAY":'+ ledarr+
				',"CHARTCOLOR":"' + sendCommand.CHARTCOLOR+
				'","fooddelay":' + foodtime+
				',"foodpower":' + foodpower;
		//alert(para);
		getMarelabData('COMMAND=SAVE_CONFIG&PARAMETER=' + para);
	}
}

// Saving of FOOD Care Parameters
function STREAM_SAVEFOOD(){
	//get
	foodpower = $('#FOODPOWER').val();
	foodtime = $('#FOODTIME').val();
	stream_SAVECURVE();
	//set
	//$('#txt_name').val('bla');
	//alert("Foodpwr:"+foodpower+" Time:"+foodtime);
	//getMarelabData('COMMAND=SAVE_CONFIG&PARAMETER=' + para);
}


// start Connector config dialog
function streamCONNECTOR(){
	para = '"PLUGINNAME":"Stream"';
	var LogicConnectors;
	var htmlContent ="<p>";
	LogicConnectors = getMarelabData('COMMAND=GET_CONNECTOR_FOR_PLUGIN&PARAMETER=' + para).CONNECTOR;
	//var myJSONText1 = JSON.stringify(getMarelabData('COMMAND=GET_CONNECTOR_FOR_PLUGIN&PARAMETER=' + para));

	// generate the logic connector list
	titeltxt = LogicConnectors.PLUGIN_NAME + " Version:" + LogicConnectors.PLUGIN_VERSION;
	htmlContent = htmlContent + "<div id='titel_setup_content'>";
	htmlContent = htmlContent + "<div id='TOOLBAR-TITEL'>"+titeltxt+"</div>";
	htmlContent = htmlContent + "</div>";
	
	htmlContent = htmlContent + "<div id='titel_setup_content'>";
	htmlContent = htmlContent + "<div id='TOOLBAR-TITEL'>Food Timer setup</div>";
	htmlContent = htmlContent + "<div id='TOOLBAR'>";
	htmlContent = htmlContent + "	<input type='image' onclick='STREAM_SAVEFOOD();' src='img/savef.png' width='32' height='32' alt='save' />";
	htmlContent = htmlContent + "</div>";
	htmlContent = htmlContent + "<table border='0' width='100%' cellpadding='0' cellspacing='2'>";
	htmlContent = htmlContent + "<tr>";
	htmlContent = htmlContent + "<td><label for='spinner'>Power:</label></td>";
	htmlContent = htmlContent + "<td><input id='FOODPOWER' name='valuespinner'></td>";
	htmlContent = htmlContent + "</tr>";
	htmlContent = htmlContent + "<tr>";
	htmlContent = htmlContent + "<td><label for='spinner'>Time :</label></td>";
	htmlContent = htmlContent + "<td><input id='FOODTIME' name='valuespinner'></td>";
	htmlContent = htmlContent + "</tr>";
	htmlContent = htmlContent + "</table>";
	htmlContent = htmlContent + "</div>";
	
	htmlContent = htmlContent + "<div id='titel_setup_content'>";
	htmlContent = htmlContent + "<div id='TOOLBAR-TITEL'>connetor setup</div>";
	htmlContent = htmlContent + "<div id='TOOLBAR'>";
	htmlContent = htmlContent + "	<input type='image' onclick='led_SAVECURVE();' src='img/savef.png' width='32' height='32' alt='save' />";
	htmlContent = htmlContent + "</div>";
	htmlContent = htmlContent + "<p><table border='1' width='100%' cellpadding='0' cellspacing='3'>";
	htmlContent = htmlContent + "<tr>";
	htmlContent = htmlContent + "<td>Connector</td>";
	htmlContent = htmlContent + "<td>Type</td>";
	htmlContent = htmlContent + "<td>Dir</td>";
	htmlContent = htmlContent + "<td>Adapter Port</td>";
	htmlContent = htmlContent + "<td>Type</td>";
	htmlContent = htmlContent + "</tr>";
	htmlContent = htmlContent + "</div>";
	var Connectors =  LogicConnectors.CONNECTORS;
	for (var Connector in Connectors) {
		htmlContent = htmlContent + "<tr>";
		htmlContent = htmlContent + "<td>"+Connectors[Connector].DESCRIPTION+"</td>";
		// Hardware Type IO,DA,AD
		if (Connectors[Connector].HWTYPE=="0")									// IO img
			htmlContent = htmlContent + "<td><img src='img/IO.png' alt='sd' width='48' height='16'/></td>";
		if (Connectors[Connector].HWTYPE=="1")									// DA img
			htmlContent = htmlContent + "<td><img src='img/DA.png' alt='sd' width='48' height='16'/></td>";
		if (Connectors[Connector].HWTYPE == "2" ) 								// AD img
			htmlContent = htmlContent + "<td><img src='img/AD.png' alt='sd' width='16' height='16'/></td>";
		// Dir IN,OUT,BI
		if (Connectors[Connector].DIR=="1")										  // Output img
			htmlContent = htmlContent + "<td><img src='img/dout.png' alt='sd' width='48' height='16'/></td>";
		if (Connectors[Connector].DIR=="0")										  // Input img
			htmlContent = htmlContent + "<td><img src='img/din.png' alt='sd' width='48' height='16'/></td>";
		if (Connectors[Connector].DIR != "0" && Connectors[Connector].DIR != "1") // Error img
			htmlContent = htmlContent + "<td><img src='img/del.png' alt='sd' width='16' height='16'/></td>";
		
		htmlContent = htmlContent + "<td>"+"<input type='image' onclick='CONNECTOR();' src='img/lightbulb.png' alt='sd' width='16' height='16'/>"+"</td>";
		htmlContent = htmlContent + "<td></td>";
		htmlContent = htmlContent + "</tr>";
	}
	htmlContent = htmlContent + "</table>";
	htmlContent = htmlContent + "</div>";
	htmlContent = htmlContent + "</div>";
	
	
	//var myJSONText1 = JSON.stringify(LogicConnectors);
	//alert (myJSONText1);
	
	para = '"PLUGINNAME":"I2C Adapter"';
	var myJSONText2 = JSON.stringify(getMarelabData('COMMAND=GET_CONNECTOR_FOR_PLUGIN&PARAMETER=' + para));
	//alert (myJSONText2);
	
	ConnectorDialog(titeltxt,htmlContent); 
	var spinner1 = $( "#FOODPOWER" ).spinner();
	var spinner2 = $( "#FOODTIME" ).spinner();
	
	$('#FOODPOWER').val(foodpower);
	$('#FOODTIME').val(foodtime);
	
	//var spinner1 = $( "#FOODPOWER" ).spinner({ max: 50 , min:0});
	//var spinner2 = $( "#FOODTIME" ).spinner({ max: 600 , min:30});
}

// MOVES the LED Light Down
function foodtimer(){
	para = '"STREAM":"FOOD"';
	getMarelabData('COMMAND=STREAM&PARAMETER=' + para);
}
// STOPS THE LED LIGHT MOVE
function streamstop(){
	para = '"STREAM":"STOP"';
	getMarelabData('COMMAND=LEDLIGHT&PARAMETER=' + para);	
}


function GenerateLedDropDown(){
	var html;
	html = '<select id="RED_LED" size="1">';
	for (i=0;i<keyLeds.length;i++){
		html = html + '<option>' + keyLeds[i]+ '</option>';
	}
	html = html + '</select>';	
	$("#redleddropdown").empty();
	$("#redleddropdown").append(html);
	
	html = '<select id="GREEN_LED" size="1">';
	for (i=0;i<keyLeds.length;i++){
		html = html + '<option>' + keyLeds[i]+ '</option>';
	}
	html = html + '</select>';	
	$("#greenleddropdown").empty();
	$("#greenleddropdown").append(html);
	
	html = '<select id="BLUE_LED" size="1">';
	for (i=0;i<keyLeds.length;i++){
		html = html + '<option>' + keyLeds[i]+ '</option>';
	}
	html = html + '</select>';	
	$("#blueleddropdown").empty();
	$("#blueleddropdown").append(html);
}

function myListener (obj)
{
    //cl(RGraph.Registry.Get('chart.contextmenu').__shape__);
    selected_item = RGraph.Registry.Get('chart.contextmenu').__shape__.index;
    alert("ITEM:" +selected_item);
}



var OpenRGBDialog = function() {
	//alert("rgb");
	$('#colorpicker').farbtastic('#color');
	$('#dialogRGB' ).dialog();
}
/////////////////////////////////////////////////////////////////////////////////
// COLOR PICKER
/////////////////////////////////////////////////////////////////////////////////
jQuery.fn.farbtastic = function (callback) {
	  $.farbtastic(this, callback);
	  return this;
	};

	jQuery.farbtastic = function (container, callback) {
	  var container = $(container).get(0);
	  return container.farbtastic || (container.farbtastic = new jQuery._farbtastic(container, callback));
	}

	jQuery._farbtastic = function (container, callback) {
	  // Store farbtastic object
	  var fb = this;

	  // Insert markup
	  $(container).html('<div class="farbtastic"><div class="color"></div><div class="wheel"></div><div class="overlay"></div><div class="h-marker marker"></div><div class="sl-marker marker"></div></div>');
	  var e = $('.farbtastic', container);
	  fb.wheel = $('.wheel', container).get(0);
	  // Dimensions
	  fb.radius = 84;
	  fb.square = 100;
	  fb.width = 194;

	  // Fix background PNGs in IE6
	  if (navigator.appVersion.match(/MSIE [0-6]\./)) {
	    $('*', e).each(function () {
	      if (this.currentStyle.backgroundImage != 'none') {
	        var image = this.currentStyle.backgroundImage;
	        image = this.currentStyle.backgroundImage.substring(5, image.length - 2);
	        $(this).css({
	          'backgroundImage': 'none',
	          'filter': "progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=crop, src='" + image + "')"
	        });
	      }
	    });
	  }

	  /**
	   * Link to the given element(s) or callback.
	   */
	  fb.linkTo = function (callback) {
	    // Unbind previous nodes
	    if (typeof fb.callback == 'object') {
	      $(fb.callback).unbind('keyup', fb.updateValue);
	    }

	    // Reset color
	    fb.color = null;

	    // Bind callback or elements
	    if (typeof callback == 'function') {
	      fb.callback = callback;
	    }
	    else if (typeof callback == 'object' || typeof callback == 'string') {
	      fb.callback = $(callback);
	      fb.callback.bind('keyup', fb.updateValue);
	      if (fb.callback.get(0).value) {
	        fb.setColor(fb.callback.get(0).value);
	      }
	    }
	    return this;
	  }
	  fb.updateValue = function (event) {
	    if (this.value && this.value != fb.color) {
	      fb.setColor(this.value);
	    }
	  }

	  /**
	   * Change color with HTML syntax #123456
	   */
	  fb.setColor = function (color) {
	    var unpack = fb.unpack(color);
	    if (fb.color != color && unpack) {
	      fb.color = color;
	      fb.rgb = unpack;
	      fb.hsl = fb.RGBToHSL(fb.rgb);
	      fb.updateDisplay();
	    }
	    return this;
	  }

	  /**
	   * Change color with HSL triplet [0..1, 0..1, 0..1]
	   */
	  fb.setHSL = function (hsl) {
	    fb.hsl = hsl;
	    fb.rgb = fb.HSLToRGB(hsl);
	    fb.color = fb.pack(fb.rgb);
	    fb.updateDisplay();
	    return this;
	  }

	  /////////////////////////////////////////////////////

	  /**
	   * Retrieve the coordinates of the given event relative to the center
	   * of the widget.
	   */
	  fb.widgetCoords = function (event) {
	    var x, y;
	    var el = event.target || event.srcElement;
	    var reference = fb.wheel;

	    if (typeof event.offsetX != 'undefined') {
	      // Use offset coordinates and find common offsetParent
	      var pos = { x: event.offsetX, y: event.offsetY };

	      // Send the coordinates upwards through the offsetParent chain.
	      var e = el;
	      while (e) {
	        e.mouseX = pos.x;
	        e.mouseY = pos.y;
	        pos.x += e.offsetLeft;
	        pos.y += e.offsetTop;
	        e = e.offsetParent;
	      }

	      // Look for the coordinates starting from the wheel widget.
	      var e = reference;
	      var offset = { x: 0, y: 0 }
	      while (e) {
	        if (typeof e.mouseX != 'undefined') {
	          x = e.mouseX - offset.x;
	          y = e.mouseY - offset.y;
	          break;
	        }
	        offset.x += e.offsetLeft;
	        offset.y += e.offsetTop;
	        e = e.offsetParent;
	      }

	      // Reset stored coordinates
	      e = el;
	      while (e) {
	        e.mouseX = undefined;
	        e.mouseY = undefined;
	        e = e.offsetParent;
	      }
	    }
	    else {
	      // Use absolute coordinates
	      var pos = fb.absolutePosition(reference);
	      x = (event.pageX || 0*(event.clientX + $('html').get(0).scrollLeft)) - pos.x;
	      y = (event.pageY || 0*(event.clientY + $('html').get(0).scrollTop)) - pos.y;
	    }
	    // Subtract distance to middle
	    return { x: x - fb.width / 2, y: y - fb.width / 2 };
	  }

	  /**
	   * Mousedown handler
	   */
	  fb.mousedown = function (event) {
	    // Capture mouse
	    if (!document.dragging) {
	      $(document).bind('mousemove', fb.mousemove).bind('mouseup', fb.mouseup);
	      document.dragging = true;
	    }

	    // Check which area is being dragged
	    var pos = fb.widgetCoords(event);
	    fb.circleDrag = Math.max(Math.abs(pos.x), Math.abs(pos.y)) * 2 > fb.square;

	    // Process
	    fb.mousemove(event);
	    return false;
	  }

	  /**
	   * Mousemove handler
	   */
	  fb.mousemove = function (event) {
	    // Get coordinates relative to color picker center
	    var pos = fb.widgetCoords(event);

	    // Set new HSL parameters
	    if (fb.circleDrag) {
	      var hue = Math.atan2(pos.x, -pos.y) / 6.28;
	      if (hue < 0) hue += 1;
	      fb.setHSL([hue, fb.hsl[1], fb.hsl[2]]);
	    }
	    else {
	      var sat = Math.max(0, Math.min(1, -(pos.x / fb.square) + .5));
	      var lum = Math.max(0, Math.min(1, -(pos.y / fb.square) + .5));
	      fb.setHSL([fb.hsl[0], sat, lum]);
	    }
	    return false;
	  }

	  /**
	   * Mouseup handler
	   */
	  fb.mouseup = function () {
	    // Uncapture mouse
	    $(document).unbind('mousemove', fb.mousemove);
	    $(document).unbind('mouseup', fb.mouseup);
	    document.dragging = false;
	  }

	  /**
	   * Update the markers and styles
	   */
	  fb.updateDisplay = function () {
	    // Markers
	    var angle = fb.hsl[0] * 6.28;
	    $('.h-marker', e).css({
	      left: Math.round(Math.sin(angle) * fb.radius + fb.width / 2) + 'px',
	      top: Math.round(-Math.cos(angle) * fb.radius + fb.width / 2) + 'px'
	    });

	    $('.sl-marker', e).css({
	      left: Math.round(fb.square * (.5 - fb.hsl[1]) + fb.width / 2) + 'px',
	      top: Math.round(fb.square * (.5 - fb.hsl[2]) + fb.width / 2) + 'px'
	    });

	    // Saturation/Luminance gradient
	    $('.color', e).css('backgroundColor', fb.pack(fb.HSLToRGB([fb.hsl[0], 1, 0.5])));

	    // Linked elements or callback
	    if (typeof fb.callback == 'object') {
	      // Set background/foreground color
	      $(fb.callback).css({
	        backgroundColor: fb.color,
	        color: fb.hsl[2] > 0.5 ? '#000' : '#fff'
	      });

	      // Change linked value
	      $(fb.callback).each(function() {
	        if (this.value && this.value != fb.color) {
	          this.value = fb.color;
	        }
	      });
	    }
	    else if (typeof fb.callback == 'function') {
	      fb.callback.call(fb, fb.color);
	    }
	  }

	  /**
	   * Get absolute position of element
	   */
	  fb.absolutePosition = function (el) {
	    var r = { x: el.offsetLeft, y: el.offsetTop };
	    // Resolve relative to offsetParent
	    if (el.offsetParent) {
	      var tmp = fb.absolutePosition(el.offsetParent);
	      r.x += tmp.x;
	      r.y += tmp.y;
	    }
	    return r;
	  };

	  /* Various color utility functions */
	  fb.pack = function (rgb) {
	    var r = Math.round(rgb[0] * 255);
	    var g = Math.round(rgb[1] * 255);
	    var b = Math.round(rgb[2] * 255);
	    return '#' + (r < 16 ? '0' : '') + r.toString(16) +
	           (g < 16 ? '0' : '') + g.toString(16) +
	           (b < 16 ? '0' : '') + b.toString(16);
	  }

	  fb.unpack = function (color) {
	    if (color.length == 7) {
	      return [parseInt('0x' + color.substring(1, 3)) / 255,
	        parseInt('0x' + color.substring(3, 5)) / 255,
	        parseInt('0x' + color.substring(5, 7)) / 255];
	    }
	    else if (color.length == 4) {
	      return [parseInt('0x' + color.substring(1, 2)) / 15,
	        parseInt('0x' + color.substring(2, 3)) / 15,
	        parseInt('0x' + color.substring(3, 4)) / 15];
	    }
	  }

	  fb.HSLToRGB = function (hsl) {
	    var m1, m2, r, g, b;
	    var h = hsl[0], s = hsl[1], l = hsl[2];
	    m2 = (l <= 0.5) ? l * (s + 1) : l + s - l*s;
	    m1 = l * 2 - m2;
	    return [this.hueToRGB(m1, m2, h+0.33333),
	        this.hueToRGB(m1, m2, h),
	        this.hueToRGB(m1, m2, h-0.33333)];
	  }

	  fb.hueToRGB = function (m1, m2, h) {
	    h = (h < 0) ? h + 1 : ((h > 1) ? h - 1 : h);
	    if (h * 6 < 1) return m1 + (m2 - m1) * h * 6;
	    if (h * 2 < 1) return m2;
	    if (h * 3 < 2) return m1 + (m2 - m1) * (0.66666 - h) * 6;
	    return m1;
	  }

	  fb.RGBToHSL = function (rgb) {
	    var min, max, delta, h, s, l;
	    var r = rgb[0], g = rgb[1], b = rgb[2];
	    min = Math.min(r, Math.min(g, b));
	    max = Math.max(r, Math.max(g, b));
	    delta = max - min;
	    l = (min + max) / 2;
	    s = 0;
	    if (l > 0 && l < 1) {
	      s = delta / (l < 0.5 ? (2 * l) : (2 - 2 * l));
	    }
	    h = 0;
	    if (delta > 0) {
	      if (max == r && max != g) h += (g - b) / delta;
	      if (max == g && max != b) h += (2 + (b - r) / delta);
	      if (max == b && max != r) h += (4 + (r - g) / delta);
	      h /= 6;
	    }
	    return [h, s, l];
	  }

	  // Install mousedown handler (the others are set on the document on-demand)
	  $('*', e).mousedown(fb.mousedown);

	    // Init color
	  fb.setColor('#000000');

	  // Set linked elements/callback
	  if (callback) {
	    fb.linkTo(callback);
	  }
	}