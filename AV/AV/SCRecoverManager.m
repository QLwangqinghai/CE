//
//  SCRecoverManager.m
//  AV
//
//  Created by vector on 2019/7/12.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "SCRecoverManager.h"

NSString * const SCFileExtendedAttributesKey = @"SCFileExtendedAttributes";
NSString * const SCFileExtendedAttributeRecoverTimestampKey = @"recoverTimestamp";

@interface SCRecoverManager ()

@property (nonatomic, copy, readonly) NSString * directoryPath;

@property (nonatomic, copy, readonly) NSString * playbackDirectoryPath;

//原始
@property (nonatomic, copy, readonly) NSString * playbackOriginalDirectoryPath;

//转码后的
@property (nonatomic, copy, readonly) NSString * playbackGeneratedVideoDirectoryPath;

//受保护的一级目录 名
@property (nonatomic, readonly) NSSet<NSString *> * protectedDirectoryNames;

@end
@implementation SCRecoverManager

- (instancetype)init {
    self = [super init];
    if (self) {
        NSString * libraryPath = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory,NSUserDomainMask,YES) firstObject];
        _directoryPath = [libraryPath stringByAppendingPathComponent:@"recover"];
        [[NSFileManager defaultManager] createDirectoryAtPath:_directoryPath withIntermediateDirectories:true attributes:NULL error:NULL];

        _playbackDirectoryPath = [_directoryPath stringByAppendingPathComponent:@"playback"];
        _playbackOriginalDirectoryPath = [_playbackDirectoryPath stringByAppendingPathComponent:@"original"];
        _playbackGeneratedVideoDirectoryPath = [_playbackDirectoryPath stringByAppendingPathComponent:@"generated"];

        [[NSFileManager defaultManager] createDirectoryAtPath:_playbackDirectoryPath withIntermediateDirectories:true attributes:NULL error:NULL];
        [[NSFileManager defaultManager] createDirectoryAtPath:_playbackOriginalDirectoryPath withIntermediateDirectories:true attributes:NULL error:NULL];
        [[NSFileManager defaultManager] createDirectoryAtPath:_playbackGeneratedVideoDirectoryPath withIntermediateDirectories:true attributes:NULL error:NULL];
        
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            [self clear];
        });
        
        NSMutableSet * protectedDirectoryNames = [NSMutableSet set];
        [protectedDirectoryNames addObject:@"playback"];
        _protectedDirectoryNames = [protectedDirectoryNames copy];
    }
    return self;
}

+ (instancetype)shared {
    static SCRecoverManager * __shared;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        __shared = [[SCRecoverManager alloc] init];
    });
    return __shared;
}

- (void)clearUnprotectedItems {
    NSFileManager * fileManager = [NSFileManager defaultManager];
    NSError * error;
    NSArray<NSString *> * itemList = [fileManager contentsOfDirectoryAtPath:self.directoryPath error:&error];
    if (error) {
        NSLog(@"[fileManager contentsOfDirectoryAtPath:%@] error:%@", self.directoryPath, error);
    }
    
    [itemList enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (![self.protectedDirectoryNames containsObject:obj]) {
            NSString * path = [self.directoryPath stringByAppendingPathComponent:obj];
            NSError * e = nil;
            [fileManager removeItemAtPath:path error:&e];
            if (e) {
                NSLog(@"removeItemAtPath:%@, error%@", path, e);
            }
        }
    }];

}

- (void)clearPlaybackOriginalItems {
    //暂时设定超时时间为7天
    NSString * directoryPath = self.playbackOriginalDirectoryPath;
    NSFileManager * fileManager = [NSFileManager defaultManager];
    NSError * error;
    NSArray<NSString *> * itemList = [fileManager contentsOfDirectoryAtPath:directoryPath error:&error];
    if (error) {
        NSLog(@"[fileManager contentsOfDirectoryAtPath:%@] error:%@", directoryPath, error);
    }
    NSTimeInterval nowTime = [NSDate date].timeIntervalSince1970;
    NSTimeInterval referenceTime = nowTime - 7 * 24 * 60 * 60;
    
    [itemList enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        NSString * path = [directoryPath stringByAppendingPathComponent:obj];
        NSDate * date = [SCRecoverManager getRecoverTimestampWithPath:path];
        if (nil == date || date.timeIntervalSince1970 < referenceTime) {
            NSError * e = nil;
            [fileManager removeItemAtPath:path error:&e];
            if (e) {
                NSLog(@"recover removeItemAtPath:%@, error%@", path, e);
            } else {
                NSLog(@"recover removeItemAtPath:%@, success", path);
            }
        }
    }];
    
}
- (void)clearPlaybackGeneratedItems {
    //暂时设定超时时间为7天
    NSString * directoryPath = self.playbackGeneratedVideoDirectoryPath;
    NSFileManager * fileManager = [NSFileManager defaultManager];
    NSError * error;
    NSArray<NSString *> * itemList = [fileManager contentsOfDirectoryAtPath:directoryPath error:&error];
    if (error) {
        NSLog(@"[fileManager contentsOfDirectoryAtPath:%@] error:%@", directoryPath, error);
    }
    NSTimeInterval nowTime = [NSDate date].timeIntervalSince1970;
    NSTimeInterval referenceTime = nowTime - 7 * 24 * 60 * 60;
    
    [itemList enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        NSString * path = [directoryPath stringByAppendingPathComponent:obj];
        NSDate * date = [SCRecoverManager getRecoverTimestampWithPath:path];
        if (nil == date || date.timeIntervalSince1970 < referenceTime) {
            NSError * e = nil;
            [fileManager removeItemAtPath:path error:&e];
            if (e) {
                NSLog(@"recover removeItemAtPath:%@, error%@", path, e);
            } else {
                NSLog(@"recover removeItemAtPath:%@, success", path);
            }
        }
    }];
}


- (void)clear {
    //应用启动清除一些垃圾文件或者超出回收期限的文件
    [self clearUnprotectedItems];
    [self clearPlaybackOriginalItems];
    [self clearPlaybackGeneratedItems];
}


//回收 课程回放的原始音视频文件
- (void)recoverPlaybackOriginalFileAtPath:(NSString *)path rename:(NSString *)fileName {
    if (path.length <= 0) {
        return;
    }
    if (fileName.length <= 0) {
        fileName = [path lastPathComponent];
    }
    NSString * toPath = [self.playbackOriginalDirectoryPath stringByAppendingPathComponent:fileName];
    NSError * error = nil;
    [[NSFileManager defaultManager] moveItemAtPath:path toPath:toPath error:&error];
    if (error) {
        NSLog(@"recoverPlaybackOriginalFileAtPath:%@ faild, error:%@", path, error);
    }
}

//回收 课程回放的原始音视频文件
- (void)recoverPlaybackGeneratedVideoAtPath:(NSString *)path rename:(NSString *)fileName {
    if (path.length <= 0) {
        return;
    }
    if (fileName.length <= 0) {
        fileName = [path lastPathComponent];
    }
    NSString * toPath = [self.playbackGeneratedVideoDirectoryPath stringByAppendingPathComponent:fileName];
    NSError * error = nil;
    [[NSFileManager defaultManager] moveItemAtPath:path toPath:toPath error:&error];
    if (error) {
        NSLog(@"recoverPlaybackOriginalFileAtPath:%@ faild, error:%@", path, error);
    } else {
        [SCRecoverManager setRecoverTimestampWithPath:toPath date:[NSDate date]];
    }
}

+ (BOOL)setRecoverTimestampWithPath:(NSString *)path date:(NSDate *)date {
    return [self setExtendedAttributesWithPath:path handle:^BOOL(NSDictionary<NSFileAttributeKey,id> *attributes, NSMutableDictionary<NSString *,id> *extended) {
        NSDate * rDate = date;
        if (nil == rDate) {
            rDate = [NSDate date];
        }
        NSDate * creationDate = attributes[NSFileCreationDate];
        int64_t timeInterval = 0;
        if (creationDate && [creationDate isKindOfClass:[NSDate class]]) {
            timeInterval = (int64_t)(creationDate.timeIntervalSince1970 * 1000 + 0.01);
        }
        int64_t recoverTimestamp = rDate.timeIntervalSince1970 * 1000 - timeInterval;
        extended[SCFileExtendedAttributeRecoverTimestampKey] = [NSNumber numberWithLong:recoverTimestamp];
        return false;
    }];
}
+ (NSDate *)getRecoverTimestampWithPath:(NSString *)path {
    return [self getExtendedAttributesWithPath:path handle:^id(NSDictionary<NSFileAttributeKey,id> *attributes, NSDictionary<NSString *,id> *extended) {
        NSNumber * recoverTimestampObj = extended[SCFileExtendedAttributeRecoverTimestampKey];
        if (recoverTimestampObj && [recoverTimestampObj isKindOfClass:[NSNumber class]]) {
            NSDate * creationDate = attributes[NSFileCreationDate];
            int64_t timeInterval = 0;
            if (creationDate && [creationDate isKindOfClass:[NSDate class]]) {
                timeInterval = (int64_t)(creationDate.timeIntervalSince1970 * 1000 + 0.01);
            }
            int64_t time = timeInterval + recoverTimestampObj.longValue;
            return [NSDate dateWithTimeIntervalSince1970:((NSTimeInterval)time)/1000];
        } else {
            return nil;
        }
    }];
}

+ (BOOL)setExtendedAttributesWithPath:(NSString *)path handle:(BOOL(^)(NSDictionary<NSFileAttributeKey, id> * attributes, NSMutableDictionary<NSString *, id> * extended))handle {
    NSError *error = NULL;
    NSMutableDictionary<NSString *, id> * extended = nil;
    
    NSDictionary<NSFileAttributeKey, id> * attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
    NSDictionary * tmpExtended = attributes[SCFileExtendedAttributesKey];
    if (tmpExtended && [tmpExtended isKindOfClass:[NSDictionary class]]) {
        extended = [tmpExtended mutableCopy];
    } else {
        extended = [NSMutableDictionary dictionary];
    }
    if (handle) {
        if (handle(attributes, extended)) {
            NSMutableDictionary<NSFileAttributeKey, id> * extendedAttributes = [NSMutableDictionary dictionary];
            extendedAttributes[SCFileExtendedAttributesKey] = extended;
            BOOL sucess = [[NSFileManager defaultManager] setAttributes:extendedAttributes ofItemAtPath:path error:&error];
            return sucess;
        }
    }
    
    return false;
}
+ (id)getExtendedAttributesWithPath:(NSString *)path handle:(id(^)(NSDictionary<NSFileAttributeKey, id> * attributes, NSDictionary<NSString *, id> * extended))handle {
    
    NSError *error = NULL;
    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
    if (!attributes) {
        return nil;
    }
    NSDictionary *extended = [attributes objectForKey:SCFileExtendedAttributesKey];
    if (extended && [extended isKindOfClass:[NSDictionary class]]) {
        if (handle) {
            return handle(attributes, extended);
        } else {
            return nil;
        }
    } else {
        return nil;
    }
}

+ (BOOL)setExtendedAttributesWithPath:(NSString *)path key:(NSString *)key value:(id)value {
    NSError *error = NULL;
    NSMutableDictionary<NSFileAttributeKey, id> * extended = nil;
    
    NSDictionary<NSFileAttributeKey, id> * attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
    NSDictionary * tmpExtended = attributes[SCFileExtendedAttributesKey];
    if (tmpExtended && [tmpExtended isKindOfClass:[NSDictionary class]]) {
        extended = [tmpExtended mutableCopy];
    } else {
        extended = [NSMutableDictionary dictionary];
    }
    extended[key] = value;
    
    NSMutableDictionary<NSFileAttributeKey, id> * extendedAttributes = [NSMutableDictionary dictionary];
    extendedAttributes[SCFileExtendedAttributesKey] = extended;
    
    BOOL sucess = [[NSFileManager defaultManager] setAttributes:extendedAttributes ofItemAtPath:path error:&error];
    return sucess;
}

//读取文件扩展属性
+ (NSDictionary *)extendedAttributesWithPath:(NSString *)path {
    if (path.length <= 0) {
        return nil;
    }
    NSError *error = NULL;
    NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
    if (!attributes) {
        return nil;
    }
    NSDictionary *tmpExtended = [attributes objectForKey:SCFileExtendedAttributesKey];
    if (tmpExtended && [tmpExtended isKindOfClass:[NSDictionary class]]) {
        return tmpExtended;
    } else {
        return nil;
    }
}

//读取文件扩展属性
+ (id)extendedAttributeWithPath:(NSString *)path key:(NSString *)key {
    if (key.length <= 0) {
        return nil;
    }
    return [self extendedAttributesWithPath:path][key];
}

@end
