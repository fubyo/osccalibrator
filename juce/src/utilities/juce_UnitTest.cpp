/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_UnitTest.h"
#include "../threads/juce_ScopedLock.h"


//==============================================================================
UnitTest::UnitTest (const String& name_)
    : name (name_), runner (0)
{
    getAllTests().add (this);
}

UnitTest::~UnitTest()
{
    getAllTests().removeValue (this);
}

Array<UnitTest*>& UnitTest::getAllTests()
{
    static Array<UnitTest*> tests;
    return tests;
}

void UnitTest::initialise()  {}
void UnitTest::shutdown()   {}

void UnitTest::performTest (UnitTestRunner* const runner_)
{
    jassert (runner_ != 0);
    runner = runner_;

    initialise();
    runTest();
    shutdown();
}

void UnitTest::logMessage (const String& message)
{
    runner->logMessage (message);
}

void UnitTest::beginTest (const String& testName)
{
    runner->beginNewTest (this, testName);
}

void UnitTest::expect (const bool result, const String& failureMessage)
{
    if (result)
        runner->addPass();
    else
        runner->addFail (failureMessage);
}

//==============================================================================
UnitTestRunner::UnitTestRunner()
    : currentTest (0), assertOnFailure (false)
{
}

UnitTestRunner::~UnitTestRunner()
{
}

int UnitTestRunner::getNumResults() const throw()
{
    return results.size();
}

const UnitTestRunner::TestResult* UnitTestRunner::getResult (int index) const throw()
{
    return results [index];
}

void UnitTestRunner::resultsUpdated()
{
}

void UnitTestRunner::runTests (const Array<UnitTest*>& tests, const bool assertOnFailure_)
{
    results.clear();
    assertOnFailure = assertOnFailure_;
    resultsUpdated();

    for (int i = 0; i < tests.size(); ++i)
    {
        try
        {
            tests.getUnchecked(i)->performTest (this);
        }
        catch (...)
        {
            addFail ("An unhandled exception was thrown!");
        }
    }

    endTest();
}

void UnitTestRunner::runAllTests (const bool assertOnFailure_)
{
    runTests (UnitTest::getAllTests(), assertOnFailure_);
}

void UnitTestRunner::logMessage (const String& message)
{
    Logger::writeToLog (message);
}

void UnitTestRunner::beginNewTest (UnitTest* const test, const String& subCategory)
{
    endTest();
    currentTest = test;

    TestResult* const r = new TestResult();
    r->unitTestName = test->getName();
    r->subcategoryName = subCategory;
    r->passes = 0;
    r->failures = 0;
    results.add (r);

    logMessage ("-----------------------------------------------------------------");
    logMessage ("Starting test: " + r->unitTestName + " / " + subCategory + "...");

    resultsUpdated();
}

void UnitTestRunner::endTest()
{
    if (results.size() > 0)
    {
        TestResult* const r = results.getLast();

        if (r->failures > 0)
        {
            String m ("FAILED!!");
            m << r->failures << (r->failures == 1 ? "test" : "tests")
              << " failed, out of a total of " << (r->passes + r->failures);

            logMessage (String::empty);
            logMessage (m);
            logMessage (String::empty);
        }
        else
        {
            logMessage ("All tests completed successfully");
        }
    }
}

void UnitTestRunner::addPass()
{
    {
        const ScopedLock sl (results.getLock());

        TestResult* const r = results.getLast();
        jassert (r != 0); // You need to call UnitTest::beginTest() before performing any tests!

        r->passes++;

        String message ("Test ");
        message << (r->failures + r->passes) << " passed";
        logMessage (message);
    }

    resultsUpdated();
}

void UnitTestRunner::addFail (const String& failureMessage)
{
    {
        const ScopedLock sl (results.getLock());

        TestResult* const r = results.getLast();
        jassert (r != 0); // You need to call UnitTest::beginTest() before performing any tests!

        r->failures++;

        String message ("!!! Test ");
        message << (r->failures + r->passes) << " failed";

        if (failureMessage.isNotEmpty())
            message << ": " << failureMessage;

        r->messages.add (message);

        logMessage (message);
    }

    resultsUpdated();

    if (assertOnFailure) { jassertfalse }
}

END_JUCE_NAMESPACE
