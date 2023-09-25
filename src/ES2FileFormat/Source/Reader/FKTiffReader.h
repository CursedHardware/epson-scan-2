//
//  FKImageInputFromTiffFileStream.h
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 2014/07/25.
//  Copyright (c) 2014年 Sato Daisuke. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "FKReader.h"

@class FKTiffToRawUtil;
@interface FKTiffReader : FKReader
{
@private
    FKTiffToRawUtil* tifftoRawUtil_;
}


@end
