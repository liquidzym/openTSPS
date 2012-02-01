﻿package com.lab.connection {	import flash.net.Socket;	import flash.utils.ByteArray;	import flash.events.Event;	import flash.events.ProgressEvent;	import flash.events.SecurityErrorEvent;	import com.lab.events.SocketMessageEvent;	import flash.utils.Endian;		/**	 * A Class to establish a socket connection and receive TCP data from openTSPS	 *	 * @author The Lab at Rockwell Group. Modified by Jeff Hoefs.	 */ 	public class TCPSocket extends Socket {					public function TCPSocket() {			super();						addEventListener(SecurityErrorEvent.SECURITY_ERROR, securityErrorHandler);			addEventListener(ProgressEvent.SOCKET_DATA, socketDataHandler, false, 0, true);				}		        private function securityErrorHandler(event:SecurityErrorEvent):void {        	trace("securityErrorHandler: " + event);        }	    private function socketDataHandler(event:ProgressEvent):void {				var fullString:String = super.readUTFBytes(super.bytesAvailable);			//remove [/TCP] message			fullString = fullString.substring(0, fullString.length-6);			var sendArray:Array = new Array();			var peopleArray:Array = fullString.split("\\");						for (var h:uint=0; h<peopleArray.length; h++){					if (peopleArray[h].length > 0){					//split on ;					var rawArray:Array = peopleArray[h].split(";");					//TSPS/personMoved/;id:1751,age:2;centroid:0.514:0.890;velocity:0:0;boundingrect:0.8:0.00208333:0.198437:0.635417;opticalflow:0:0;					var sendObject:Object = new Object();					sendObject.address = rawArray.shift();								for (var i:uint = 0; i<rawArray.length; i++){						var messageArray:Array = rawArray[i].split("/");												//if there are multiple values						if (messageArray.length > 1){							var splitData:Array = new Array();							if (messageArray[1].indexOf(",") != -1){															sendObject[ messageArray[0] ] = new Array();								var valueArray:Array = messageArray[1].split(",");								for (var j:uint = 0; j<valueArray.length; j++){																var data:Array = valueArray[j].split(":");									var obj:Object = new Object();																for (var k:uint = 0; k<data.length; k++){										splitData = data[k].split(">");										if (splitData.length > 1) obj[splitData[0]] = splitData[1];									}									sendObject[messageArray[0]].push(obj);								}												} else {																		var dataArray:Array = messageArray[1].split(":");								if (dataArray.length > 1){									sendObject[messageArray[0]] = new Object();									for (var kk:uint = 0; kk<dataArray.length; kk++){										splitData = dataArray[kk].split(">");										sendObject[messageArray[0]][splitData[0]] = splitData[1];									}														} else {									sendObject[messageArray[0]] = messageArray[1];												}							}						}					}					sendArray.push(sendObject);				}			}			dispatchEvent(new SocketMessageEvent(SocketMessageEvent.MESSAGE_RECEIVED,sendArray));	    }	}}