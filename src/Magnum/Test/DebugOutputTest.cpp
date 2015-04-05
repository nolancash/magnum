/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/DebugOutput.h"

namespace Magnum { namespace Test {

struct DebugOutputTest: TestSuite::Tester {
    explicit DebugOutputTest();

    void debugSource();
    void debugType();
    void debugSeverity();

    void debugMessageSource();
    void debugMessageType();

    void debugGroupSource();
};

DebugOutputTest::DebugOutputTest() {
    addTests({&DebugOutputTest::debugSource,
              &DebugOutputTest::debugType,
              &DebugOutputTest::debugSeverity,

              &DebugOutputTest::debugMessageSource,
              &DebugOutputTest::debugMessageType,

              &DebugOutputTest::debugGroupSource});
}

void DebugOutputTest::debugSource() {
    std::ostringstream o;
    Debug(&o) << DebugOutput::Source::ShaderCompiler;
    CORRADE_COMPARE(o.str(), "DebugOutput::Source::ShaderCompiler\n");
}

void DebugOutputTest::debugType() {
    std::ostringstream o;
    Debug(&o) << DebugOutput::Type::PushGroup;
    CORRADE_COMPARE(o.str(), "DebugOutput::Type::PushGroup\n");
}

void DebugOutputTest::debugSeverity() {
    std::ostringstream o;
    Debug(&o) << DebugOutput::Severity::Notification;
    CORRADE_COMPARE(o.str(), "DebugOutput::Severity::Notification\n");
}

void DebugOutputTest::debugMessageSource() {
    std::ostringstream o;
    Debug(&o) << DebugMessage::Source::Application;
    CORRADE_COMPARE(o.str(), "DebugMessage::Source::Application\n");
}

void DebugOutputTest::debugMessageType() {
    std::ostringstream o;
    Debug(&o) << DebugMessage::Type::DeprecatedBehavior;
    CORRADE_COMPARE(o.str(), "DebugMessage::Type::DeprecatedBehavior\n");
}

void DebugOutputTest::debugGroupSource() {
    std::ostringstream o;
    Debug(&o) << DebugGroup::Source::ThirdParty;
    CORRADE_COMPARE(o.str(), "DebugGroup::Source::ThirdParty\n");
}

}}

CORRADE_TEST_MAIN(Magnum::Test::DebugOutputTest)