
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Type encoding's type.
 */
typedef NS_OPTIONS(NSUInteger, BCEncodingType) {
    BCEncodingTypeMask       = 0xFF, ///< mask of type value
    BCEncodingTypeUnknown    = 0, ///< unknown
    BCEncodingTypeVoid       = 1, ///< void
    BCEncodingTypeBool       = 2, ///< bool
    BCEncodingTypeInt8       = 3, ///< char / BOOL
    BCEncodingTypeUInt8      = 4, ///< unsigned char
    BCEncodingTypeInt16      = 5, ///< short
    BCEncodingTypeUInt16     = 6, ///< unsigned short
    BCEncodingTypeInt32      = 7, ///< int
    BCEncodingTypeUInt32     = 8, ///< unsigned int
    BCEncodingTypeInt64      = 9, ///< long long
    BCEncodingTypeUInt64     = 10, ///< unsigned long long
    BCEncodingTypeFloat      = 11, ///< float
    BCEncodingTypeDouble     = 12, ///< double
    BCEncodingTypeLongDouble = 13, ///< long double
    BCEncodingTypeObject     = 14, ///< id
    BCEncodingTypeClass      = 15, ///< Class
    BCEncodingTypeSEL        = 16, ///< SEL
    BCEncodingTypeBlock      = 17, ///< block
    BCEncodingTypePointer    = 18, ///< void*
    BCEncodingTypeStruct     = 19, ///< struct
    BCEncodingTypeUnion      = 20, ///< union
    BCEncodingTypeCString    = 21, ///< char*
    BCEncodingTypeCArray     = 22, ///< char[10] (for example)
    
    BCEncodingTypeQualifierMask   = 0xFF00,   ///< mask of qualifier
    BCEncodingTypeQualifierConst  = 1 << 8,  ///< const
    BCEncodingTypeQualifierIn     = 1 << 9,  ///< in
    BCEncodingTypeQualifierInout  = 1 << 10, ///< inout
    BCEncodingTypeQualifierOut    = 1 << 11, ///< out
    BCEncodingTypeQualifierBycopy = 1 << 12, ///< bycopy
    BCEncodingTypeQualifierByref  = 1 << 13, ///< byref
    BCEncodingTypeQualifierOneway = 1 << 14, ///< oneway
    
    BCEncodingTypePropertyMask         = 0xFF0000, ///< mask of property
    BCEncodingTypePropertyReadonly     = 1 << 16, ///< readonly
    BCEncodingTypePropertyCopy         = 1 << 17, ///< copy
    BCEncodingTypePropertyRetain       = 1 << 18, ///< retain
    BCEncodingTypePropertyNonatomic    = 1 << 19, ///< nonatomic
    BCEncodingTypePropertyWeak         = 1 << 20, ///< weak
    BCEncodingTypePropertyCustomGetter = 1 << 21, ///< getter=
    BCEncodingTypePropertyCustomSetter = 1 << 22, ///< setter=
    BCEncodingTypePropertyDynamic      = 1 << 23, ///< @dynamic
};

/**
 Get the type from a Type-Encoding string.
 
 @discussion See also:
 https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtTypeEncodings.html
 https://developer.apple.com/library/mac/documentation/Cocoa/Conceptual/ObjCRuntimeGuide/Articles/ocrtPropertyIntrospection.html
 
 @param typeEncoding  A Type-Encoding string.
 @return The encoding type.
 */
BCEncodingType BCEncodingGetType(const char *typeEncoding);


/**
 Instance variable information.
 */
@interface BCClassIvarInfo : NSObject
@property (nonatomic, assign, readonly) Ivar ivar;              ///< ivar opaque struct
@property (nonatomic, strong, readonly) NSString *name;         ///< Ivar's name
@property (nonatomic, assign, readonly) ptrdiff_t offset;       ///< Ivar's offset
@property (nonatomic, strong, readonly) NSString *typeEncoding; ///< Ivar's type encoding
@property (nonatomic, assign, readonly) BCEncodingType type;    ///< Ivar's type

/**
 Creates and returns an ivar info object.
 
 @param ivar ivar opaque struct
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithIvar:(Ivar)ivar;
@end


/**
 Method information.
 */
@interface BCClassMethodInfo : NSObject
@property (nonatomic, assign, readonly) Method method;                  ///< method opaque struct
@property (nonatomic, strong, readonly) NSString *name;                 ///< method name
@property (nonatomic, assign, readonly) SEL sel;                        ///< method's selector
@property (nonatomic, assign, readonly) IMP imp;                        ///< method's implementation
@property (nonatomic, strong, readonly) NSString *typeEncoding;         ///< method's parameter and return types
@property (nonatomic, strong, readonly) NSString *returnTypeEncoding;   ///< return value's type
@property (nullable, nonatomic, strong, readonly) NSArray<NSString *> *argumentTypeEncodings; ///< array of arguments' type

/**
 Creates and returns a method info object.
 
 @param method method opaque struct
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithMethod:(Method)method;
@end


/**
 Property information.
 */
@interface BCClassPropertyInfo : NSObject
@property (nonatomic, assign, readonly) objc_property_t property; ///< property's opaque struct
@property (nonatomic, strong, readonly) NSString *name;           ///< property's name
@property (nonatomic, assign, readonly) BCEncodingType type;      ///< property's type
@property (nonatomic, strong, readonly) NSString *typeEncoding;   ///< property's encoding value
@property (nonatomic, strong, readonly) NSString *ivarName;       ///< property's ivar name
@property (nullable, nonatomic, assign, readonly) Class cls;      ///< may be nil
@property (nullable, nonatomic, strong, readonly) NSArray<NSString *> *protocols; ///< may nil
@property (nonatomic, assign, readonly) SEL getter;               ///< getter (nonnull)
@property (nonatomic, assign, readonly) SEL setter;               ///< setter (nonnull)

/**
 Creates and returns a property info object.
 
 @param property property opaque struct
 @return A new object, or nil if an error occurs.
 */
- (instancetype)initWithProperty:(objc_property_t)property;
@end


/**
 Class information for a class.
 */
@interface BCClassInfo : NSObject
@property (nonatomic, assign, readonly) Class cls; ///< class object
@property (nullable, nonatomic, assign, readonly) Class superCls; ///< super class object
@property (nullable, nonatomic, assign, readonly) Class metaCls;  ///< class's meta class object
@property (nonatomic, readonly) BOOL isMeta; ///< whether this class is meta class
@property (nonatomic, strong, readonly) NSString *name; ///< class name
@property (nullable, nonatomic, strong, readonly) BCClassInfo *superClassInfo; ///< super class's class info
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, BCClassIvarInfo *> *ivarInfos; ///< ivars
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, BCClassMethodInfo *> *methodInfos; ///< methods
@property (nullable, nonatomic, strong, readonly) NSDictionary<NSString *, BCClassPropertyInfo *> *propertyInfos; ///< properties

/**
 If the class is changed (for example: you add a method to this class with
 'class_addMethod()'), you should call this method to refresh the class info cache.
 
 After called this method, `needUpdate` will returns `YES`, and you should call 
 'classInfoWithClass' or 'classInfoWithClassName' to get the updated class info.
 */
- (void)setNeedUpdate;

/**
 If this method returns `YES`, you should stop using this instance and call
 `classInfoWithClass` or `classInfoWithClassName` to get the updated class info.
 
 @return Whether this class info need update.
 */
- (BOOL)needUpdate;

/**
 Get the class info of a specified Class.
 
 @discussion This method will cache the class info and super-class info
 at the first access to the Class. This method is thread-safe.
 
 @param cls A class.
 @return A class info, or nil if an error occurs.
 */
+ (nullable instancetype)classInfoWithClass:(Class)cls;

/**
 Get the class info of a specified Class.
 
 @discussion This method will cache the class info and super-class info
 at the first access to the Class. This method is thread-safe.
 
 @param className A class name.
 @return A class info, or nil if an error occurs.
 */
+ (nullable instancetype)classInfoWithClassName:(NSString *)className;

@end

NS_ASSUME_NONNULL_END
