/*
File:   assert.h
Author: Taylor Robbins
Date:   11\26\2022
*/

#ifndef _ASSERT_H
#define _ASSERT_H

#define MyBreak() //TODO: Route this somewhere!

#if DEBUG_BUILD
#define MyDebugBreak() MyBreak()
#else
#define MyDebugBreak() //nothing
#endif

//TODO: Add a callback function of some sort for debug purposes
#define AssertMsg(Expression, message) do { if (!(Expression)) { MyBreak(); } } while(0)
#define AssertMsg_(Expression, message) do { if (!(Expression)) { MyBreak(); } } while(0)

#define Assert(Expression)  AssertMsg((Expression), nullptr)
#define Assert_(Expression) AssertMsg_((Expression), nullptr)

//Assertions that only happen in debug builds
#if DEBUG_BUILD
#define DebugAssert(Expression)              Assert((Expression))
#define DebugAssert_(Expression)             Assert_((Expression))
#define DebugAssertMsg(Expression, message)  AssertMsg((Expression), (message))
#define DebugAssertMsg_(Expression, message) AssertMsg_((Expression), (message))
#define DebugAssertAndUnused(Expression, unusedVarInRelease)              DebugAssert(Expression)
#define DebugAssertAndUnused_(Expression, unusedVarInRelease)             DebugAssert_(Expression)
#define DebugAssertAndUnusedMsg(Expression, unusedVarInRelease, message)  DebugAssertMsg((Expression), message)
#define DebugAssertAndUnusedMsg_(Expression, unusedVarInRelease, message) DebugAssertMsg_((Expression), message)
#else
#define DebugAssert(Expression)              //null
#define DebugAssert_(Expression)             //null
#define DebugAssertMsg(Expression, message)  //null
#define DebugAssertMsg_(Expression, message) //null
#define DebugAssertAndUnused(Expression, unusedVarInRelease)              UNUSED(unusedVarInRelease)
#define DebugAssertAndUnused_(Expression, unusedVarInRelease)             UNUSED(unusedVarInRelease)
#define DebugAssertAndUnusedMsg(Expression, unusedVarInRelease, message)  UNUSED(unusedVarInRelease)
#define DebugAssertAndUnusedMsg_(Expression, unusedVarInRelease, message) UNUSED(unusedVarInRelease)
#endif

#ifndef ASSERTIONS_ENABLED
#error ASSERTIONS_ENABLED is not defined!
#endif

//Compiled out precondition if statement
#if ASSERTIONS_ENABLED
#define AssertIf(Precondition, Expression)  if (Precondition) { Assert(Expression);  }
#define AssertIf_(Precondition, Expression) if (Precondition) { Assert_(Expression); }
#define AssertIfMsg(Precondition, Expression, message)  if (Precondition) { AssertMsg(Expression, message);  }
#define AssertIfMsg_(Precondition, Expression, message) if (Precondition) { AssertMsg_(Expression, message); }
#else
#define AssertIf(Precondition, Expression)              //nothing
#define AssertIf_(Precondition, Expression)             //nothing
#define AssertIfMsg(Precondition, Expression, message)  //nothing
#define AssertIfMsg_(Precondition, Expression, message) //nothing
#endif

//Quick-hand for something != nullptr && something != nullptr && ...
#define NotNull(variable)                                    Assert((variable) != nullptr)
#define NotNull2(variable1, variable2)                       Assert((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3(variable1, variable2, variable3)            Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4(variable1, variable2, variable3, variable4) Assert((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNull_(variable)                                    Assert_((variable) != nullptr)
#define NotNull2_(variable1, variable2)                       Assert_((variable1) != nullptr && (variable2) != nullptr)
#define NotNull3_(variable1, variable2, variable3)            Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr)
#define NotNull4_(variable1, variable2, variable3, variable4) Assert_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr)

#define NotNullMsg(variable, message)                                    AssertMsg((variable) != nullptr, (message))
#define NotNull2Msg(variable1, variable2, message)                       AssertMsg((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg(variable1, variable2, variable3, message)            AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg(variable1, variable2, variable3, variable4, message) AssertMsg((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))

#define NotNullMsg_(variable, message)                                    AssertMsg_((variable) != nullptr, (message))
#define NotNull2Msg_(variable1, variable2, message)                       AssertMsg_((variable1) != nullptr && (variable2) != nullptr, (message))
#define NotNull3Msg_(variable1, variable2, variable3, message)            AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr, (message))
#define NotNull4Msg_(variable1, variable2, variable3, variable4, message) AssertMsg_((variable1) != nullptr && (variable2) != nullptr && (variable3) != nullptr && (variable4) != nullptr, (message))

#define Unimplemented()  AssertMsg(false, "This code path is unimplemented!")
#define Unimplemented_() AssertMsg_(false, "This code path is unimplemented!")

#endif //  _ASSERT_H
