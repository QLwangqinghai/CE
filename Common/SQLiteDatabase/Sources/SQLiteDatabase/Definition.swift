//
//  Definition.swift
//  SqliteDatabase
//
//  Created by wangqinghai on 2017/12/25.
//  Copyright © 2017年 wangqinghai. All rights reserved.
//

import Foundation


public struct FieldType {
    public struct Integer {
        //1 byte
        public static let tinyInt = "TINYINT"

        //2 bytes
        public static let smallInt = "SMALLINT"
//        public static let int2 = "INT2"

        //3 bytes
        public static let mediumInt = "MEDIUMINT"
        
        //4 bytes
        public static let int = "INT"
//        public static let integer = "INTEGER"

        //8 bytes
        public static let bigInt = "BIGINT"
        

        public static let unsignedBigInt = "UNSIGNED BIG INT"
        public static let int8 = "INT8"
    }
    
    public struct Real {
        //4 bytes
        public static let float = "FLOAT"

        //8 bytes
        public static let double = "DOUBLE"
    }
    
    public struct Text {
        public static func character(_ count: UInt8) -> String {
            if count > 0 {
                return "CHARACTER(\(count))"
            } else {
                return "CHARACTER"
            }
        }
        
        public static func varChar(_ count: UInt8) -> String {
            if count > 0 {
                return "VARCHAR(\(count))"
            } else {
                return "VARCHAR"
            }
        }
        public static let varChar = "VARCHAR(255)"
        public static let text = "TEXT"
        public static let clob = "CLOB"
    }

}

public struct Definition {
    /*
     
     https://www.cnblogs.com/stephen-liu74/archive/2012/01/18/2325258.html
     声明类型    亲缘类型    应用规则
     INT
     INTEGER
     TINYINT
     SMALLINT
     MEDIUMINT
     BIGINT
     UNSIGNED BIG INT
     INT2
     INT8
     
     
     http://www.jb51.net/article/31946.htm
     bigint
     从 -2^63 (-9223372036854775808) 到 2^63-1 (9223372036854775807) 的整型数据（所有数字）。存储大小为 8 个字节。
     P.S. bigint已经有长度了，在mysql建表中的length，只是用于显示的位数
     int
     从 -2^31 (-2,147,483,648) 到 2^31 – 1 (2,147,483,647) 的整型数据（所有数字）。存储大小为 4 个字节。int 的 SQL-92 同义字为 integer。
     smallint
     从 -2^15 (-32,768) 到 2^15 – 1 (32,767) 的整型数据。存储大小为 2 个字节。
     tinyint
     从 0 到 255 的整型数据。存储大小为 1 字节。
     
     http://www.runoob.com/sqlite/sqlite-data-types.html
     
     smallint 16 位元的整数。
     integer 32 位元的整数。
     decimal(p,s) p 精确值和 s 大小的十进位整数，精确值p是指全部有几个数(digits)大小值，s是指小数点後有几位数。如果没有特别指定，则系统会设为 p=5; s=0 。
     float  32位元的实数。
     double  64位元的实数。
     char(n)  n 长度的字串，n不能超过 254。
     varchar(n) 长度不固定且其最大长度为 n 的字串，n不能超过 4000。
     graphic(n) 和 char(n) 一样，不过其单位是两个字元 double-bytes， n不能超过127。这个形态是为了支援两个字元长度的字体，例如中文字。
     vargraphic(n) 可变长度且其最大长度为 n 的双字元字串，n不能超过 2000
     date  包含了 年份、月份、日期。
     time  包含了 小时、分钟、秒。
     timestamp 包含了 年、月、日、时、分、秒、千分之一秒。
     
     datetime 包含日期时间格式，必须写成'2010-08-05'不能写为'2010-8-5'，否则在读取时会产生错误！
     */
    
    
//    1、CHAR。CHAR存储定长数据很方便，CHAR字段上的索引效率级高，比如定义char(10)，那么不论你存储的数据是否达到了10个字节，都要占去10个字节的空间,不足的自动用空格填充。
//
//    2、VARCHAR。存储变长数据，但存储效率没有CHAR高。如果一个字段可能的值是不固定长度的，我们只知道它不可能超过10个字符，把它定义为 VARCHAR(10)是最合算的。VARCHAR类型的实际长度是它的值的实际长度+1。为什么“+1”呢？这一个字节用于保存实际使用了多大的长度。从空间上考虑，用varchar合适；从效率上考虑，用char合适，关键是根据实际情况找到权衡点。
//
//    3、TEXT。text存储可变长度的非Unicode数据，最大长度为2^31-1(2,147,483,647)个字符。
//
//    4、NCHAR、NVARCHAR、NTEXT。这三种从名字上看比前面三种多了个“N”。它表示存储的是Unicode数据类型的字符。我们知道字符中，英文字符只需要一个字节存储就足够了，但汉字众多，需要两个字节存储，英文与汉字同时存在时容易造成混乱，Unicode字符集就是为了解决字符集这种不兼容的问题而产生的，它所有的字符都用两个字节表示，即英文字符也是用两个字节表示。nchar、nvarchar的长度是在1到4000之间。和char、varchar比较起来，nchar、nvarchar则最多存储4000个字符，不论是英文还是汉字；而char、varchar最多能存储8000个英文，4000个汉字。可以看出使用nchar、nvarchar数据类型时不用担心输入的字符是英文还是汉字，较为方便，但在存储英文时数量上有些损失。
//
//    所以一般来说，如果含有中文字符，用nchar/nvarchar，如果纯英文和数字，用char/varchar。
    
    
    
}
