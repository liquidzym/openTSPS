﻿package com.lab.connection  {		import flash.events.IEventDispatcher;		/**	 * This interface defines methods that should be implemented by the connection class	 */	// need to extend IEventDispatcher because connection classes extend EventDispatcher	public interface ITSPSConnection extends IEventDispatcher {		function connect(host:String = "127.0.0.1", port:int = 3335):void;		function get connected():Boolean;	}	}