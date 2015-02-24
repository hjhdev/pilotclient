/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "testblackcoremain.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackCoreTest;

//! Starter for test cases
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CBlackCoreTestMain::unitMain(argc, argv);
    Q_UNUSED(a);

    // bye
    return 0;
}
