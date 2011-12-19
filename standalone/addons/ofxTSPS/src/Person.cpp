/*
 *  Person.cpp
 *  peopleTracker_000
 *
 *  Adapted by James George on 11/21/09.
 *  Copyright 2009 (http://www.jamesgeorge.org) All rights reserved.
 *
 */
#include "Person.h"

using namespace ofxCv;
using namespace cv;

#define MAX_HAAR_GHOSTFRAMES 15 //how long before we say it's really gone

namespace ofxTSPS {
    
    Person::Person(int id, int index, ofxCv::ContourFinder& _contourFinder)
    : contourFinder(_contourFinder), 
      pid(id),
      oid(index),
      age(0),
      hasHaar(false),
      haarRect(ofRectangle(0,0,0,0)),
      opticalFlowVectorAccumulation(ofPoint(0,0)),
      framesWithoutHaar(0),
      customAttributes(NULL),
      depth(0)
    {
        centroid = (toOf(contourFinder.getCentroid(index)));
        update(false);
    }

    Person::~Person()
    {
        if(customAttributes != NULL){
            free(customAttributes);
        }
    }

    void Person::updateIndex( int index ){
        oid = index;
    };

    void Person::update(bool dampen)
    {
        //oid = contourFinder.getTracker().getIndexFromLabel(pid);
        ofPoint c = toOf(contourFinder.getCentroid(oid));
        
        if(dampen){
            centroid = (centroid * .7) + ( c * .3);
        }
        else{
            centroid = c;
            //centroid = blob.centroid;
        }
        
        velocity	 = c - centroid;
		contour		 = contourFinder.getPolyline(oid);
        area		 = contourFinder.getContourArea(oid);//blob.area;
        boundingRect = toOf(contourFinder.getBoundingRect(oid));//blob.boundingRect;
        
        simpleContour = contour;
        simpleContour.simplify(2.0f);
        float simplifyAmount = 2.5f;
        while (simpleContour.size() > 100){
            simpleContour.simplify(simplifyAmount);
            simplifyAmount += .5f;
        }
        
        age++;
    }


    void Person::setHaarRect(ofRectangle _haarRect)
    {
        haarRect = _haarRect;
        hasHaar = true;
        framesWithoutHaar = 0;
    }

    ofRectangle Person::getHaarRect()
    {
        if(!hasHaar){
            printf("Person: accessing Haar rect without Haar!\n");
            return ofRectangle(0,0,0,0);
        }
        return haarRect;
    }

    bool Person::hasHaarRect()
    {
        return hasHaar;
    }

    void Person::noHaarThisFrame()
    {
        //temp remove keep haar
        hasHaar = false;
        return;
        
        //if we had a haar but it vanished move it by the movement of the blob
        if(framesWithoutHaar++ > MAX_HAAR_GHOSTFRAMES){
            hasHaar = false;
        }
        if(hasHaar){
            haarRect.x += velocity.x;
            haarRect.y += velocity.y;
        }
    }

    ofRectangle Person::getBoundingRectNormalized(float videoWidth, float videoHeight)
    {
            return ofRectangle(boundingRect.x/videoWidth,
                               boundingRect.y/videoHeight,
                               boundingRect.width/videoWidth,
                               boundingRect.height/videoHeight);
    }


    ofRectangle Person::getHaarRectNormalized(float videoWidth, float videoHeight){
        return ofRectangle(haarRect.x/videoWidth,
                           haarRect.y/videoHeight,
                           haarRect.width/videoWidth,
                           haarRect.height/videoHeight);
    };

    ofPoint Person::getCentroidNormalized(float videoWidth, float videoHeight)
    {
        return ofPoint(centroid.x / videoWidth, centroid.y / videoHeight);
    }

    ofPoint Person::getHaarCentroidNormalized(float videoWidth, float videoHeight)
    {
        ofRectangle haarRect = getHaarRect();
        return ofPoint( (haarRect.x + haarRect.width/2) / videoWidth, (haarRect.y + haarRect.height/2) / videoHeight );
    }

    #pragma mark get as string

    /***************************************************************
     GET PERSON STRING (so we don't have to write this so many times)
     ***************************************************************/

    string Person::getJSON( string type, ofPoint centroid, int cameraWidth, int cameraHeight, bool bSendContours ){
        
        //construct a JSON object
        
        stringstream message;
        message<<"{";
        message<<"\"type\":\""<<type<<"\",";
        message<<"\"id\":"<<pid<<",";
        message<<"\"age\":"<<age<<",";
        message<<"\"depth\":"<<depth<<",";
        message<<"\"centroid\":{"<<"\"x\":"<<centroid.x<<",\"y\":"<<centroid.y<<"},"; //pass in normalized centroid
        message<<"\"velocity\":{"<<"\"x\":"<<ofToString(velocity.x, 3)<<",\"y\":"<<ofToString(velocity.y, 3)<<"},";
        
        ofRectangle scaledRect = getBoundingRectNormalized(cameraWidth,cameraHeight);
        message<<"\"boundingrect\":{"<<"\"x\":"<<scaledRect.x<<",\"y\":"<<scaledRect.y<<",\"width\":"<<scaledRect.width<<",\"height\":"<<scaledRect.height<<"},";
        
        message<<"\"opticalflow\":{"<<"\"x\":"<<opticalFlowVectorAccumulation.x<<",\"y\":"<<opticalFlowVectorAccumulation.y<<"},";
        ofRectangle scaledHaar = getHaarRectNormalized(cameraWidth,cameraHeight);
        message<<"\"haarrect\":{"<<"\"x\":"<<scaledHaar.x<<",\"y\":"<<scaledHaar.y<<",\"width\":"<<scaledHaar.width<<",\"height\":"<<scaledHaar.height<<"}";	
        
        if (bSendContours){
            message<<",";
            message<<"\"contours\":[";
            for (int i=0; i<simpleContour.size(); i++){
                message<<"{\"x\":"<<ofToString(simpleContour[i].x, 3)<<",\"y\":"<<ofToString(simpleContour[i].y, 3)<<"}";
                if (i+1 < simpleContour.size()) message<<",";
            };
            message<<"]";
        }
        message<<"}";
        return message.str();
    }
}

