//
//  SITPArray.h
//  SITPNumber
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#ifndef SITPArray_h
#define SITPArray_h

#include <stdio.h>

/*
 contentOnly
 Float32 Float64 Time HTime
 SITPDataSubtypeCodeByte16 = 0x0,
 SITPDataSubtypeCodeByte32 = 0x1,
 SITPDataSubtypeCodeByte48 = 0x2,
 SITPDataSubtypeCodeByte64 = 0x3,
 
 
 4b encodeType
 
 Int SInt
 encodeType == 0, 4b(1 ~ 8)
 encodeType == 1, 1b(0) 3b(1 ~ 4)

 Bool
 encodeType == 0, 1b(0)  3b 0 ~ 7

 Data
 SITPDataSubtypeCodeNormal = 0x16,
 SITPDataSubtypeCodeJsonMap = 0x17,
 encodeType = 0, 4b(0) (count=4B, lengths[count], remain(content))
 encodeType = 1, 2b count 2b header

 
 
 String Message
 encodeType = 0, 4b(0) (count=4B, lengths[count], remain(content))
 encodeType = 1, 2b count 2b header
 */


#endif /* SITPArray_h */
