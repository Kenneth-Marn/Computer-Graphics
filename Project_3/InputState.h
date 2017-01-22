

//
//  InputState.h
//  assignment2
//
//  Created by Zwe Khaing Marn on 14/05/2016.
//  Copyright © 2016 Zwe Khaing Marn. All rights reserved.
//

#ifndef InputState_h
#define InputState_h

struct InputState
{
    InputState(): lMousePressed(false),
    rMousePressed(false),
    prevX(0), prevY(0),
    prevZ(0),
    deltaX(0), deltaY(0) , deltaZ(0){}
    
    // Is the mouse button currently being held down?
    bool lMousePressed;
    bool rMousePressed;
    
    // Last known position of the cursor
    float prevX;
    float prevY;
    float prevZ;
    // Accumulated change in cursor position.
    float deltaX;
    float deltaY;
    float deltaZ;
    // Update cursor variables based on new position x,y
    void update(float x, float y,float z)
    {
        float xDiff = x - prevX;
        float yDiff = y - prevY;
        deltaX += xDiff;
        deltaY += yDiff;
        prevX = x;
        prevY = y;
        prevZ = z;
    };
    
    // Read off the accumulated motion and reset it
    void readDeltaAndReset(float *x, float *y,float *z)
    {
        *x = deltaX;
        *y = deltaY;
        *z = deltaZ;
        deltaX = 0;
        deltaY = 0;
    };
};

#endif /* InputState_h */


