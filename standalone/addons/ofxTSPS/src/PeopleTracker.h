/***************************************************************************
 *
 *  PeopleTracker.h
 *  Rockwell LAB + IDEO LAB peopleVision project
 *
 *  Based in part on code created by NUI Group Dev Team A on 3/1/09.
 *  Copyright 2009 NUI Group/Inc. All rights reserved.       
 *  Version : 1.20
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer as
 * the first lines of this file unmodified.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NUI GROUP ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BEN WOODHOUSE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 * Web: http://code.google.com/p/peoplevision/
 *
 * (...)
 *
 ***************************************************************************/

/**
 * 11/23/2009
 * Refactored by James George (http://www.jamesgeorge.org)
 * integrated aspects of persistent person tracking and motion detection
 * from SNIFF project (http://www.gravitytrap.com/sniff/)
 * Added:
 *	added getters/setters for non-gui configuration
 *	instance persistent blob tracking
 *	delegate call backs
 *	finished tuio and optical flow implementation
 */

#ifndef _OFXPEOPLETRACKER_H
#define _OFXPEOPLETRACKER_H

#include "Person.h"
#include "View.h"
#include "ofxCv.h"
//#include "ofxOpenCv.h"
#include "CPUImageFilter.h"
//#include "ofxCvOpticalFlowLK.h"

//haar setting
#define CV_HAAR_FIND_MODE CV_HAAR_FIND_BIGGEST_OBJECT

//dependent libraries
//#include "ofxCvHaarTracker.h"
#include "Settings.h"
#include "GuiManager.h"
#include "TUIOSender.h"
#include "OscSender.h"
#include "TCPSender.h"
#include "WebSocketSender.h"
//#include "ofxCvBlobTracker.h"
#include "Scene.h"
#include "Utils.h"

#define DRAW_MODE_NORMAL				0
#define DRAW_MODE_GUI					1
#define DRAW_MODE_CAMERA_CALIBRATE		2
#define DRAW_MODE_FULLSCREEN			3

#define CAMERA_SOURCE_VIEW				0
#define ADJUSTED_CAMERA_VIEW			1
#define REFERENCE_BACKGROUND_VIEW		2
#define PROCESSED_VIEW					3
#define DATA_VIEW						4

//layout vars
#define spacing 10
#define GUI_WIDTH 340

#define TSPS_HOME_PAGE "http://lab.rockwellgroup.com/work/TSPS"
#define TSPS_SOURCE_PAGE "https://github.com/labatrockwell/openTSPS"

namespace ofxTSPS {

    //interface for listener of people events
    class PersonListener {
    public:
        //called when the person enters the system
        virtual void personEntered( Person* person, Scene* scene ) = 0;
        //called each time the centroid moves (a lot)
        virtual void personMoved( Person* person, Scene* scene ) = 0;
        //called one frame before the person is removed from the list to let you clean up
        virtual void personWillLeave( Person* person, Scene* scene ) = 0;
        //called every frame no matter what.
        virtual void personUpdated(Person* person, Scene* scene) = 0;
    };



    class PeopleTracker {//: public ofxCvBlobListener {
        public:
            //set up and update
        
            void setup(int w, int h);				//Call during setup with camera width & height
            void update(const ofImage& image);		//Call with sequential camera images
            //void update(ofImage image); //Call with sequential camera images
            void mousePressed(ofMouseEventArgs &e);	
            
            //communication
            
            void setupTuio(string ip, int port);
            void setupOsc(string ip, int port);
            void setupTcp(int port);
            void setupWebSocket(int port);
            void setListener(PersonListener* delegate);
        
            //tracking metrics
            
            void enableHaarFeatures(bool doHaar);
            void enableOpticalFlow(bool doOpticalFlow);
        
            // background learning and management
        
            void relearnBackground(); //one shot relearn trigger
            void enableBackgroundRelearnSmart(bool doSmartLearn);//auto-relearns if there are too many blobs in the scene
            void enableBackgroundReleaernProgressive(bool doProgressive); //relearns over time using progessive frame averagering
            void setRelearnRate(float relearnRate);
            
            //image control
        
            void setThreshold(float thresholdAmount);
            void setMinBlobSize(float minBlobSize); // % of screen
            void setMaxBlobSize(float maxBlobSize); // % of screen
        
            void enableSmooth(bool doSmooth);
            void setSmoothAmount(int smoothAmount);
        
            void enableHighpass(bool doHighpass);
            void setHighpassBlurAmount(int highpassBlurAmount);
            void setHighpassNoiseAmount(int highpassNoiseAmount);

            void enableAmplify(bool doAmp);
            void setAmplifyAmount(int amplifyAmount);
                                   
            // filter controls

            //haar
            void setHaarXMLFile(string haarFile);
            void setHaarExpandArea(float haarExpandAmount); //makes the haar rect +area bigger
    //		void setMinHaarArea(float minArea);
    //		void setMaxHaarArea(float maxArea);
            void useHaarAsCentroid(bool useHaarCenter);
        
            //blobs
            void enableFindHoles(bool findHoles);
            void trackDarkBlobs();
            void trackLightBlobs();
            int getDrawMode();
        
            //GUI extension
            void addSlider(string name, int* value, int min, int max);
            void addSlider(string name, float* value, float min, float max);
            void addToggle(string name, bool* value);
        
            //drawing methods
        
            void setDrawMode(int mode);
            void draw();
            void draw(int x, int y);
            void draw(int x, int y, int mode);
        
            //callbacks for blob tracking
        
            Person* personAtIndex(int i);
            Person* getTrackedPerson(int pid);
            int totalPeople();
        
            void blobOn( int x, int y, int id, int order );
            void blobMoved( int x, int y, int id, int order );
            void blobOff( int x, int y, int id, int order );
        
            // layout functions
            void setActiveView( int activeIndex );
            void setActiveDimensions ( int actWidth, int actHeight);	
            bool loadFont(string fontName, int fontSize);
        
            //JG this is so we can access video grabber settings through the default interface
            void setVideoGrabber(ofBaseVideo* grabber, tspsInputType inputType);

            // for accessing Optical Flow in specific regions
            //			  and accessing the threshold set in the GUI
            ofPoint getOpticalFlowInRegion(float x, float y, float w, float h);
            
            // for accessing the OSC sender whose parameters are adjusted in the GUI
            OscSender* getOSCsender(); 
        
            // Websockets 
            WebSocketSender * getWebSocketServer();
            
            // for getting a color version of the adjusted view image
            // NOTE:  only works if the adjusted view is currently in color	and not grayscale
            //        (this parameter can be set in the GUI under the 'views' tab)
            ofImage& getAdjustedImageInColor();	
            
            // for accessing which view is the current view
            bool inCameraView();
            bool inAdjustedView();
            bool inBackgroundView();
            bool inDifferencingView();
            bool inDataView();
            
            // GUI getters
            bool useKinect();	
        
        protected:
        
            void trackPeople();
            void updateSettings();
            bool isTrackingPerson(int pid);
                
            map<unsigned int,Person*> trackedPeople;
            Scene scene;
            
            ofImage cameraImage, warpedImage, backgroundImage, differencedImage;
            //ofxCv::RunningBackground backgroundDifferencer;
            
            //more specific CV images for processing
            CPUImageFilter		grayDiff;
            //ofxCvShortImage		floatBgImg;
            
            //coord warping for cropping the camera image
            //ofxCvCoordWarping coordWarp;
        
            // blob tracking things
            
            ofxCv::ContourFinder    contourFinder;
            //ofxCvContourFinder 	contourFinder;
            //ofxCvBlobTracker persistentTracker;
            int drawMode;
            
            //filter variables
            
            //float fLearnRate;
            float width, height;
            
            // haar
            string lastHaarFile;
            //ofxLABCvHaarFinder	 haarFinder;
            //ofxCvHaarTracker    haarTracker;
            
            // optical flow
            //ofxCvOpticalFlowLK	opticalFlow;
            
            // switches for filters
            Settings *p_Settings;
        
            // communication
            
            TUIOSender tuioClient;
            bool bTuioEnabled;
            PersonListener* eventListener;
            OscSender oscClient;
            bool bOscEnabled;
            TCPSender tcpClient;
            bool bTcpEnabled;
            WebSocketSender webSocketServer;
            bool bWebSocketsEnabled;
        
            //gui
            GuiManager gui;
            bool guiLoaded;
            float activeHeight, activeWidth;
        
            // layout functions
            ofTrueTypeFont font;
            bool bFontLoaded;
            
            //drawing + activating active video
        
            void updateViewRectangles();
            void drawBlobs( float width, float height );
            bool isInsideRect(float x, float y, ofRectangle rect);
            int activeViewIndex;
            
            ofRectangle activeView;
            View cameraView;
            View adjustedView;
            View bgView;
            View processedView;
            View dataView;
    };
};

#endif
