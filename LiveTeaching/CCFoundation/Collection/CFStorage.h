///*	CFStorage.h
//	Copyright (c) 1999-2018, Apple Inc. and the Swift project authors
// 
//	Portions Copyright (c) 2014-2018, Apple Inc. and the Swift project authors
//	Licensed under Apache License v2.0 with Runtime Library Exception
//	See http://swift.org/LICENSE.txt for license information
//	See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//*/
///*!
//        @header CFStorage
//CFStorage stores an array of arbitrary-sized values. There are no callbacks;
//all that is provided about the values is the size, and the appropriate number
//of bytes are copied in and out of the CFStorage.
//
//CFStorage uses a balanced tree to store the values, and is most appropriate
//for situations where potentially a large number values (more than a hundred
//bytes' worth) will be stored and there will be a lot of editing (insertions and deletions).
//
//Getting to an item is O(log n), although caching the last result often reduces this
//to a constant time.
//
//The overhead of CFStorage is 48 bytes. There is no per item overhead; the 
//non-leaf nodes in the tree cost 20 bytes each, and the worst case extra
//capacity (unused space in the leaves) is 12%, typically much less.
//
//Because CFStorage does not necessarily use a single block of memory to store the values,
//when you ask for a value, you get back the pointer to the value and optionally
//the range of other values that are consecutive and thus reachable as if the
//storage was a single block.
//*/
//
//#if !defined(__COREFOUNDATION_CFSTORAGE__)
//#define __COREFOUNDATION_CFSTORAGE__ 1
//
//
//#include <stdlib.h>
//#include <stdio.h>
//
//
//typedef uint32_t CFStorageEnumerationOptionFlags;
//static CFStorageEnumerationOptionFlags kCFStorageEnumerationConcurrent = 0x1;
//
//struct __CCBalanceTreeStorage;
//typedef struct __CCBalanceTreeStorage * CCBalanceTreeStorageRef;
//typedef CCBalanceTreeStorageRef CFStorageRef;
//typedef long CCIndex;
//
//typedef struct {
//    CCIndex location;
//    CCIndex length;
//} CCRange;
//
///*!
//	@typedef CFStorageApplierFunction
//	Type of the callback function used by the apply functions of
//		CFStorage.
//	@param value The current value from the storage.
//	@param context The user-defined context parameter given to the apply
//		function.
//*/
//typedef void (*CFStorageApplierFunction)(const void *value, void *context);
//
///*!
//	@typedef CFStorageRangeApplierBlock
//	Type of the callback block used by the apply functions of
//	    CFStorage
//	@param val  A pointer to a range of values, numbering range.length
//	@param range The range of values.  This will always be a subrange of the range
//	    passed to the apply function.  Do not try to modify the contents of the vals pointer, because
//	    there is no guarantee it points into the contents of the CFStorage object.
//	 @param stop An "out" parameter that, if set to true from within the block, indicates that the enumeration may stop.
// 
//*/
//#if __BLOCKS__
//typedef void (^CFStorageApplierBlock)(const void *vals, CCRange range, _Bool * stop);
//#endif
//
//
///*!
//        @function CFStorageCreate
//        Creates a new mutable storage with elements of the given size.
//	@param valueSizeInBytes The size in bytes of each of the elements 
//		to be stored in the storage.  If this value is zero or
//		negative, the result is undefined.
//	@result A reference to the new CFStorage instance.
//*/
//CFStorageRef CFStorageCreate(uint32_t valueSizeInBytes);
//
///*!
//	@function CFStorageInsertValues
//	Allocates space for range.length values at location range.location.  Use
//	 CFStorageReplaceValues() to set those values.
//	@param storage The storage to which the values are to be inserted.
//		If this parameter is not a valid CFStorage, the behavior is undefined.
//	@param range The range of values within the storage to insert. The
// 		range location must be at least zero and not exceed the count of the storage.
// 		Values at indexes equal to or greater than the range location have their indexes
//  		increased by the length of the range.  Thus this creates a gap in the storage
//  		 equal to the length of the given range.  If the range length is negative, the
//   		behavior is undefined. The range may be empty (length 0),
//  		 in which case there is no effect.
//   		
//*/
//void CFStorageInsertValues(CFStorageRef storage, CCRange range);
//
///*!
//	@function CFStorageDeleteValues
//	Deletes the values of the storage in the specified range.
//	@param storage The storage from which the values are to be deleted.
//		If this parameter is not a valid CFStorage, the behavior is undefined.
//	@param range The range of values within the storage to delete. If the
//		range location or end point (defined by the location plus
//		length minus 1) are outside the index space of the storage (0
//		to N inclusive, where N is the count of the storage), the
//		behavior is undefined. If the range length is negative, the
//		behavior is undefined. The range may be empty (length 0),
//		in which case no values are deleted.
//*/
//void CFStorageDeleteValues(CFStorageRef storage, CCRange range);
//
///*!
//	@function CFStorageGetCount
//	Returns the number of values currently in the storage.
//	@param storage The storage to be queried. If this parameter is not a valid
//		CFStorage, the behavior is undefined.
//	@result The number of values in the storage.
//*/
//long CFStorageGetCount(CFStorageRef storage);
//
///*!
//	@function CFStorageGetValueAtIndex
//		Returns a pointer to the specified value.  The pointer is mutable and may be used to
//		get or set the value.  This is considered to be a mutating function, and so calling this
//		while accessing the CFStorage from another thread is undefined behavior,
// 		even if you do not set a value.  To access the CFStorage in a non-mutating
//  		manner, use the more efficient CFStorageGetConstValueAtIndex().
//	@param storage The storage to be queried. If this parameter is not a
//		valid CFStorage, the behavior is undefined.
//	@param idx The index of the value to retrieve. If the index is
//		outside the index space of the storage (0 to N-1 inclusive,
//		where N is the count of the storage), the behavior is
//		undefined.
//	@param validConsecutiveValueRange This parameter is a C pointer to a CFRange.
//		If NULL is specified, this argument is ignored; otherwise, the range
//		is set to the range of values that may be accessed via an offset from the result pointer.
//		The range location is set to the index of the lowest consecutive
//		value and the range length is set to the count of consecutive values.
//	@result The value with the given index in the storage.
//*/
//void * _Nullable CFStorageGetValueAtIndex(CFStorageRef storage, CCIndex idx, CCRange *validConsecutiveValueRange);
//
///*!
//	@function CFStorageGetConstValueAtIndex
//		Returns a pointer to the specified value.  The pointer is immutable and may
//		only be used to get the value.  This is not considered to be a mutating function,
//		so it is safe to call this concurrently with other non-mutating functions.  Furthermore,
// 		this is often more efficient than CFStorageGetValueAtIndex(), so it should be used
//  		in preference to that function when possible.
//	@param storage The storage to be queried. If this parameter is not a
//		valid CFStorage, the behavior is undefined.
//	@param idx The index of the value to retrieve. If the index is
//		outside the index space of the storage (0 to N-1 inclusive,
//		where N is the count of the storage), the behavior is
//		undefined.
//	@param validConsecutiveValueRange This parameter is a C pointer to a CFRange.
//		If NULL is specified, this argument is ignored; otherwise, the range
//		is set to the range of values that may be accessed via an offset from the result pointer.
//		The range location is set to the index of the lowest consecutive
//		value and the range length is set to the count of consecutive values.
//	@result The value with the given index in the storage.
//*/
//const void *CFStorageGetConstValueAtIndex(CFStorageRef storage, CCIndex idx, CCRange *validConsecutiveValueRange);
//
///*!
//        @function CFStorageGetValues
//	Fills the buffer with values from the storage.
//	@param storage The storage to be queried. If this parameter is not a
//		valid CFStorage, the behavior is undefined.
//	@param range The range of values within the storage to retrieve. If
//		the range location or end point (defined by the location
//		plus length minus 1) are outside the index space of the
//		storage (0 to N-1 inclusive, where N is the count of the
//		storage), the behavior is undefined. If the range length is
//		negative, the behavior is undefined. The range may be empty
//		(length 0), in which case no values are put into the buffer.
//	@param values A C array of to be filled with values from the storage. 
//                The values in the C array are ordered
//		in the same order in which they appear in the storage. If this
//		parameter is not a valid pointer to a C array of at least
//		range.length pointers, the behavior is undefined.
//*/
//void CFStorageGetValues(CFStorageRef storage, CCRange range, void *values);
//
///*!
//	@function CFStorageApplyFunction
//	Calls a function once for each value in the set.
//	@param storage The storage to be operated upon. If this parameter is not
//		a valid CFStorage, the behavior is undefined.
//	@param range The range of values within the storage to operate on. If the
//		range location or end point (defined by the location plus
//		length minus 1) are outside the index space of the storage (0
//		to N inclusive, where N is the count of the storage), the
//		behavior is undefined. If the range length is negative, the
//		behavior is undefined. The range may be empty (length 0),
//		in which case the no values are operated on.
//	@param applier The callback function to call once for each value in
//		the given storage. If this parameter is not a
//		pointer to a function of the correct prototype, the behavior
//		is undefined. If there are values in the storage which the
//		applier function does not expect or cannot properly apply
//		to, the behavior is undefined. 
//	@param context A pointer-sized user-defined value, which is passed
//		as the second parameter to the applier function, but is
//		otherwise unused by this function. If the context is not
//		what is expected by the applier function, the behavior is
//		undefined.
//*/
//void CFStorageApplyFunction(CFStorageRef storage, CCRange range, CFStorageApplierFunction applier, void *context);
//
///*!
//	@function CFStorageApplyBlock
//	Enumerates ranges of stored objects with a block.
// 	@param storage The storage to be operated upon. If this parameter is not
//		a valid CFStorage, the behavior is undefined.
//	 @param range The range of values within the storage to operate on. If the
//		 sum of the range location and length is larger than the
//		 count of the storage, the behavior is undefined.  If the
//		 range location or length is negative, the behavior is undefined.
//  	@param options Options controlling how the enumeration may proceed.
//  	@param applier The callback block.  The block is passed a pointer to
//		a buffer of contiguous objects in the storage, and the range of stored 
//		values represented by the buffer.  If the block modifies the 
// 		contents of the buffer, the behavior is undefined.  If the block modifies
//  		the contents of the CFStorage, the behavior is undefined.
// 
// */
//#if __BLOCKS__
//void CFStorageApplyBlock(CFStorageRef storage, CCRange range, CFStorageEnumerationOptionFlags options, CFStorageApplierBlock applier);
//#endif
//
//
///*!
//	@function CFStorageCreateWithSubrange
//	Returns a new CFStorage that contains a portion of an existing CFStorage.
// 	@param storage The storage to be operated upon. If this parameter is not
//		a valid CFStorage, the behavior is undefined.
//	 @param range The range of values within the storage to operate on. If the
//		 sum of the range location and length is larger than the
//		 count of the storage, the behavior is undefined.  If the
//		 range location or length is negative, the behavior is undefined.
// 	 @result A reference to a new CFStorage containing a byte-for-byte copy of
//		 the objects in the range.  This may use copy-on-write techniques
// 		 to allow efficient implementation.
// */
//CFStorageRef CFStorageCreateWithSubrange(CFStorageRef storage, CCRange range);
//
///*!
//        @function CFStorageReplaceValues
//	Replaces a range of values in the storage.
//	@param storage The storage from which the specified values are to be
//		removed. If this parameter is not a valid CFStorage,
//		the behavior is undefined.
//	@param range The range of values within the storage to replace. If the
//		range location or end point (defined by the location plus
//		length minus 1) are outside the index space of the storage (0
//		to N inclusive, where N is the count of the storage), the
//		behavior is undefined. If the range length is negative, the
//		behavior is undefined. The range may be empty (length 0),
//		in which case the new values are merely inserted at the
//		range location.
//	@param values A C array of the values to be copied into the storage. 
//		The new values in the storage are ordered in the same order 
//		in which they appear in this C array. This parameter may be NULL 
//		if the range length is 0.  This C array is not changed or freed by
//		this function. If this parameter is not a valid pointer to a C array of at least
//		range length pointers, the behavior is undefined.
//*/
//void CFStorageReplaceValues(CFStorageRef storage, CCRange range, const void *values);
//
///* Private stuff...
//*/
//CCIndex __CFStorageGetCapacity(CFStorageRef storage);
//CCIndex __CFStorageGetValueSize(CFStorageRef storage);
//void __CFStorageSetAlwaysFrozen(CFStorageRef storage, _Bool alwaysFrozen);
//
//
//#endif /* ! __COREFOUNDATION_CFSTORAGE__ */
//
