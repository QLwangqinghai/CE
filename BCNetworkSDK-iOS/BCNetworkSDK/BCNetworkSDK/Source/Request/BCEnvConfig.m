//
//  BCEnvConfig.m
//  BCNetworkSDK
//
//  Created by 王青海 on 2018/7/24.
//  Copyright © 2018年 王青海. All rights reserved.
//

#import "BCEnvConfig.h"
#import "BCLog.h"

//@implementation BCHTTPFieldInfo
//
//- (instancetype)init {
//    self = [super init];
//    if (self) {
//        _timeoutInterval = 60;
//        _networkServiceType = NSURLNetworkServiceTypeDefault;
//        _HTTPShouldUsePipelining = false;
//        _HTTPShouldHandleCookies = true;
//        _cachePolicy = NSURLRequestUseProtocolCachePolicy;
//        _allowsCellularAccess = true;
//    }
//    return self;
//}
//
//
//- (NSMutableDictionary *)allFields {
//    NSArray *keyPaths = @[NSStringFromSelector(@selector(allowsCellularAccess)), NSStringFromSelector(@selector(cachePolicy)), NSStringFromSelector(@selector(HTTPShouldHandleCookies)), NSStringFromSelector(@selector(HTTPShouldUsePipelining)), NSStringFromSelector(@selector(networkServiceType)), NSStringFromSelector(@selector(timeoutInterval))];
//    NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithCapacity:10];
//    for (NSString *keyPath in keyPaths) {
//        [dict setObject:[self valueForKeyPath:keyPath] forKey:keyPath];
//    }
//    return dict;
//}
//
//+ (NSDictionary *)defaultAllFields {
//    static NSDictionary* __defaultAllFields = nil;
//    
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        __defaultAllFields = [[[[BCHTTPFieldInfo alloc] init] allFields] copy];
//    });
//    
//    return __defaultAllFields;
//}
//
//@end

@interface BCEnvConfig ()
{
    NSData *_seedData;
}
@property (nonatomic, copy) NSString *name;
@property (nonatomic, strong) NSMutableDictionary *URLStringInfoStore;

@end
@implementation BCEnvConfig

- (instancetype)init {
    BCLogWarn(@"BCEnvConfig init error, used initWithName:appID:host:seed: !!!");

    return nil;
}

- (instancetype)initWithName:(NSString *)name {
    return [self initWithName:name appID:nil host:nil seed:nil];
}


- (instancetype)initWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed {
    if (name.length <= 0) {
        BCLogWarn(@"BCEnvConfig initWithName error, name must be valid!!!");
        return nil;
    }
    
    self = [super init];
    if (self) {
        _name = [name copy];
        _appID = [appID copy];
        _host = [host copy];
        self.seed = seed;

        _URLStringInfoStore = [NSMutableDictionary dictionary];
    }
    return self;
}

- (NSDictionary *)URLStringInfo {
    return [self.URLStringInfoStore copy];
}

- (NSString *)debugDescription {
    return [NSString stringWithFormat:@"<%@: %p> {name:%@, host:%@, seed:%@, info:%@}", [self class], self, self.name, self.host, self.seed, self.init];
}

- (void)setSeed:(NSString *)seed {
    if (seed) {
        _seed = [seed copy];
        _seedData = [_seed dataUsingEncoding:NSUTF8StringEncoding];
    } else {
        _seed = nil;
        _seedData = nil;
    }
}

- (NSData *)seedData {
    return _seedData;
}


- (NSString *)urlStringForKey:(id <NSCopying>)key {
    if (nil == key) {
        return nil;
    }
    return self.URLStringInfoStore[key];
}

- (void)setUrlString:(NSString *)urlString forKey:(id <NSCopying>)key {
    if (nil == key) {
        return;
    }
    if (nil == urlString) {
        [self.URLStringInfoStore removeObjectForKey:key];
    } else {
        self.URLStringInfoStore[key] = urlString;
    }
}


- (void)removeAllUrl {
    [self.URLStringInfoStore removeAllObjects];
}

- (void)addUrlsFrom:(NSDictionary *)info {
    if (info) {
        [self.URLStringInfoStore setValuesForKeysWithDictionary:info];
    }
}

+ (instancetype)configWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed {
    return [self configWithName:name appID:appID host:host seed:seed builder:nil];
}

+ (instancetype)configWithName:(NSString *)name appID:(NSString *)appID host:(NSString *)host seed:(NSString *)seed builder:(void(^)(BCEnvConfig *config))builder {
    BCEnvConfig *config = [[self alloc] initWithName:name appID:appID host:host seed:seed];
    if (builder) {
        builder(config);
    }
    return config;
}

+ (instancetype)configWithName:(NSString *)name builder:(void(^)(BCEnvConfig *config))builder {
    return [self configWithName:name appID:nil host:nil seed:nil builder:builder];
}


@end





