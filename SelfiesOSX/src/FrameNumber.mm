#include "FrameNumber.h"
#import <Foundation/Foundation.h>

void FrameNumber::saveFrameNumber ( int num ) {
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setInteger:num forKey:@"Frame Number"];
    [prefs synchronize];
}

int FrameNumber::loadFrameNumber () {
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSInteger myInt = [prefs integerForKey:@"Frame Number"];
    int intValue  = myInt;
    return intValue;
}
