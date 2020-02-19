import XCTest

import SQLite3CoreTests

var tests = [XCTestCaseEntry]()
tests += SQLite3CoreTests.allTests()
XCTMain(tests)
