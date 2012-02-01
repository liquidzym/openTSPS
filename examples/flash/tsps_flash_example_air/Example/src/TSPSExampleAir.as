﻿package {	import app.CustomPerson;		import com.lab.TSPS;	import com.lab.TSPSPerson;	import com.lab.connection.ITSPSConnection;	import com.lab.connection.OSCConnection;	import com.lab.connection.TCPConnection;	import com.lab.events.TSPSEvent;		import flash.display.Sprite;	import flash.display.Stage;	import flash.events.Event;	import flash.text.TextField;	import flash.utils.setTimeout;		[SWF(width="1024", height="768", frameRate="30", backgroundColor="#FFFFFF")]	public class TSPSExampleAir extends Sprite {				// change the following port values if necessary for your application		private static const PORT_TCP:uint = 3335;		private static const PORT_OSC:uint = 3333;				private var tspsServer:TSPS;				private var stageWidth:Number;		private var stageHeight:Number;				private var customPeople:Array;				private var currentColor:uint = 0;				// use the ITSPSConnection as the type so you can assign either		// assign either a TCPConnection or OSCConnection object to this property		private var connection:ITSPSConnection;				// define either a TCP or OSC connection		// the connection type is specified in the communication tab in the openTSPS application		// change this type to TCP_CONNECTION OR OSC_CONNECTION		private var connectionType:String = TSPS.TCP_CONNECTION;		//private var connectionType:String = TSPS.OSC_CONNECTION;				private var port:uint;				public function TSPSExampleAir() {						// make the application window visible			// alternatively you could set this in TSPSExampleAir-app.xml			this.stage.nativeWindow.visible = true;						// store variables of stage width + height			// TSPS sends out coordinates from 0 - 1, so you need to scale them to			// the size that you want (usually the stage width + height)			// - for this app, there is a MC on the stage called activeArea_mc that			// 	 we're using for the active area			stageWidth = activeArea_mc.width; //stage.stageWidth;			stageHeight = activeArea_mc.height; //stage.stageHeight;						// build array of your custom people objects			customPeople = new Array();						// create a new connection object			if (connectionType == TSPS.TCP_CONNECTION) {				connection = new TCPConnection();				port = PORT_TCP;			}			else if (connectionType == TSPS.OSC_CONNECTION) {				// In order to send OSC data from openTSPS to Flash, a udp to tcp bridge is required				// We recommend Memo's UDP-TCP Bridge: http://www.msavisuals.com/udp_tcp_bridge				connection = new OSCConnection();				port = PORT_OSC;			}						// start tsps server, passing a reference to the connection			tspsServer = new TSPS(connection);									//listen for connection information			connection.addEventListener(Event.CONNECT, connectHandler);			connection.addEventListener(Event.CLOSE, closeHandler);						// - this defaults to host as "localhost" and port as 3335 for TCP or 3333 for OSC,			//   so you can just call connect() if all is default; otherwise, you can call			//	 connect (host, port)			connection.connect("localhost", port);						//listen for three key TSPS events: person entered, person moved, person left			// - person entered is only called the first time a person appears			// - person moved gets called each time a person gets updated			// - person left gets called when a person dissapears (when they are no longer tracked)			tspsServer.addEventListener( TSPSEvent.PERSON_ENTERED, personEntered);			tspsServer.addEventListener( TSPSEvent.PERSON_MOVED, personMoved);			tspsServer.addEventListener( TSPSEvent.PERSON_LEFT, personLeft);						numberOfPeople_mc.number_txt.text = String(customPeople.length);			connected_mc.visible = true;		}						/************************************************************************		 CONNECTION EVENTS					 ************************************************************************/				private function connectHandler(e:Event):void {			trace ("connected");			removeChild(connected_mc);		}				private function closeHandler(e:Event):void {			trace ("closed!");			addChild(connected_mc);		}				/************************************************************************		 PEOPLE EVENT CATCHERS		 - add these to listen for specific things from TSPS					 ************************************************************************/				private function personEntered(e:TSPSEvent):void {			// create a new custom person + pass it the person parameter of the TSPSEvent			// and the height and width of the viewable area for the custom person			var newCustomPerson:CustomPerson = new CustomPerson( e.person, stageWidth, stageHeight );			newCustomPerson.setBackground(currentColor);			if (currentColor++ > 20) currentColor = 0;						// add the custom person to the viewable area in your application			activeArea_mc.addChild(newCustomPerson);			customPeople.push( newCustomPerson );						personEntered_mc.gotoAndPlay("active");			numberOfPeople_mc.number_txt.text = customPeople.length;		}				private function personMoved(e:TSPSEvent):void {			// don't do any updating; we are listening for the person's update in the CustomPerson class			personUpdated_mc.gotoAndPlay("active");			numberOfPeople_mc.number_txt.text = customPeople.length;		}				private function personLeft(e:TSPSEvent):void {			// find the person that left by id and remove from array			for (var i:int = customPeople.length-1; i>=0; i--){				if (customPeople[i].person.id == e.person.id){ 					activeArea_mc.removeChild(customPeople[i]);					customPeople.splice( i, 1 );					break;								}			}			personLeft_mc.gotoAndPlay("active");			numberOfPeople_mc.number_txt.text = customPeople.length;		}	}}