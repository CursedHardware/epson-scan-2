//
//  FKErrorUtil.h
//  FileFormatKit
//
//  Created by Yumoto Atsushi on 2014/10/08.
//  Copyright (c) 2014年 epson. All rights reserved.
//
#import "FileFormatKit.h"

static NSString* const kESImageFormatKitErrorDomain = @"ESImageFormatKit";

inline static FKErrorCode  FKErrorCodeFromNSError(NSError** error,FKErrorCode defaultCode)
{
    if (error) {
        return [*error code];
    }
    
    return defaultCode;
}

inline static BOOL NSErrorFromFKErrorCode(FKErrorCode errorCode,NSError** outError)
{
    if(outError) {
        
        if (errorCode == kFKNoError) {
            *outError = nil;
        }else{
            
            *outError = [NSError errorWithDomain:kESImageFormatKitErrorDomain
                                            code:errorCode
                                        userInfo:nil];
        }
        
        return YES;
    }
    
    
    return NO;
}
