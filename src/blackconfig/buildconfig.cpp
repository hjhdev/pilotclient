/* Copyright (C) 2013
 * swift Project Community/Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "buildconfig.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>
#include <QStringList>
#include <QStringBuilder>
#include <QtGlobal>
#include <QSysInfo>

namespace BlackConfig
{
    const QString &CBuildConfig::swiftGuiExecutableName()
    {
        static const QString s("swiftguistd");
        return s;
    }

    const QString &CBuildConfig::swiftCoreExecutableName()
    {
        static const QString s("swiftcore");
        return s;
    }

    const QString &CBuildConfig::swiftDataExecutableName()
    {
        static const QString s("swiftdata");
        return s;
    }

    bool CBuildConfig::isKnownExecutableName(const QString &executable)
    {
        return executable == CBuildConfig::swiftCoreExecutableName() ||
               executable == CBuildConfig::swiftDataExecutableName() ||
               executable == CBuildConfig::swiftGuiExecutableName();
    }

    bool CBuildConfig::isRunningOnWindows10()
    {
#ifdef Q_OS_WIN
        // QSysInfo::WindowsVersion only available on Win platforms
        if (!CBuildConfig::isRunningOnWindowsNtPlatform()) { return false; }
        return (QSysInfo::WindowsVersion == QSysInfo::WV_10_0);
#else
        return false;
#endif
    }

    const QString &CBuildConfig::getPlatformString()
    {
        static const QString p([]
        {
            if (CBuildConfig::isRunningOnLinuxPlatform()) return QString("Linux");
            if (CBuildConfig::isRunningOnMacOSPlatform()) return QString("MacOS");
            if (CBuildConfig::isRunningOnWindowsNtPlatform())
            {
                if (CBuildConfig::buildWordSize() == 32) return QString("Win32");
                if (CBuildConfig::buildWordSize() == 64) return QString("Win64");
            };
            return QString("unknown");
        }());
        return p;
    }

    namespace Private
    {
        bool isLocalDeveloperBuildImpl()
        {
            if (!CBuildConfig::isDebugBuild()) { return false; }
            const QString p = QCoreApplication::applicationDirPath().toLower();

            // guessing, feel free to add path checks
            if (p.contains("build")) { return true; }
            if (p.contains("msvc")) { return true; }
            return false;
        }
    }

    bool CBuildConfig::isLocalDeveloperDebugBuild()
    {
        static const bool devBuild = Private::isLocalDeveloperBuildImpl();
        return devBuild;
    }

    bool CBuildConfig::isLifetimeExpired()
    {
        if (!getEol().isValid()) { return true; }
        return QDateTime::currentDateTime() > getEol();
    }

    const QString boolToYesNo(bool v)
    {
        return v ? QStringLiteral("yes") : QStringLiteral("no");
    }

    const QString &CBuildConfig::compiledWithInfo(bool shortVersion)
    {
        if (shortVersion)
        {
            static QString infoShort;
            if (infoShort.isEmpty())
            {
                QStringList sl;
                if (CBuildConfig::isCompiledWithBlackCore())     { sl << "BlackCore"; }
                if (CBuildConfig::isCompiledWithBlackSound())    { sl << "BlackSound"; }
                if (CBuildConfig::isCompiledWithBlackInput())    { sl << "BlackInput"; }
                if (CBuildConfig::isCompiledWithGui())           { sl << "BlackGui"; }
                if (CBuildConfig::isCompiledWithFs9Support())    { sl << "FS9"; }
                if (CBuildConfig::isCompiledWithFsxSupport())    { sl << "FSX"; }
                if (CBuildConfig::isCompiledWithXPlaneSupport()) { sl << "XPlane"; }
                if (CBuildConfig::isCompiledWithP3DSupport())    { sl << "P3D"; }
                if (CBuildConfig::isCompiledWithFGSupport())     { sl << "FG"; }
                infoShort = sl.join(", ");
                if (infoShort.isEmpty()) { infoShort = "<none>"; }
            }
            return infoShort;
        }
        else
        {
            static QString infoLong;
            if (infoLong.isEmpty())
            {
                infoLong = infoLong.append("BlackCore: ").append(boolToYesNo(isCompiledWithBlackCore()));
                infoLong = infoLong.append(" BlackInput: ").append(boolToYesNo(isCompiledWithBlackInput()));
                infoLong = infoLong.append(" BlackSound: ").append(boolToYesNo(isCompiledWithBlackSound()));
                infoLong = infoLong.append(" GUI: ").append(boolToYesNo(isCompiledWithGui()));

                infoLong = infoLong.append(" FS9: ").append(boolToYesNo(isCompiledWithFs9Support()));
                infoLong = infoLong.append(" FSX: ").append(boolToYesNo(isCompiledWithFsxSupport()));
                infoLong = infoLong.append(" P3D: ").append(boolToYesNo(isCompiledWithP3DSupport()));
                infoLong = infoLong.append(" XPlane: ").append(boolToYesNo(isCompiledWithXPlaneSupport()));
                infoLong = infoLong.append(" FG: ").append(boolToYesNo(isCompiledWithFGSupport()));
            }
            return infoLong;
        }
    }

    const QString &CBuildConfig::buildDateAndTime()
    {
        // http://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
        static const QString buildDateAndTime = QString(__DATE__ " "  __TIME__).simplified();
        return buildDateAndTime;
    }

    const QVersionNumber &CBuildConfig::getVersion()
    {
        static const QVersionNumber v { versionMajor(), versionMinor(), versionMicro(), lastCommitTimestampAsVersionSegment(lastCommitTimestamp()) };
        return v;
    }

    const QString &CBuildConfig::getVersionString()
    {
        static const QString s(getVersion().toString());
        return s;
    }

    const QString &CBuildConfig::getVersionStringPlatform()
    {
        static const QString s = getPlatformString() % u' ' % getVersionString();
        return s;
    }

    namespace Private
    {
        const QDateTime buildTimestampImpl()
        {
            // Mar 27 2017 20:17:06 (needs to be on english locale, otherwise fails - e.g.
            QDateTime dt = QLocale(QLocale::English).toDateTime(CBuildConfig::buildDateAndTime().simplified(), "MMM d yyyy hh:mm:ss");
            dt.setUtcOffset(0);
            return dt;
        }
    }

    const QDateTime &CBuildConfig::buildTimestamp()
    {
        // Mar 27 2017 20:17:06
        static const QDateTime dt = Private::buildTimestampImpl();
        return dt;
    }

    int CBuildConfig::lastCommitTimestampAsVersionSegment(const QDateTime &lastCommitTimestamp)
    {
        if (lastCommitTimestamp.isValid())
        {
            const QString bts = lastCommitTimestamp.toString("yyyyMMddHHmm");
            bool ok;
            const long long lctsll = bts.toLongLong(&ok); // at least 64bit
            if (!ok) { return 0; }
            // now we have to converto int
            // max 2147483647 (2^31 - 1)
            //      1MMddHHmm (years since 2010)
            const long long yearOffset = 201000000000;
            const int lctsInt = static_cast<int>(lctsll - yearOffset);
            return lctsInt;
        }
        return 0; // intentionally 0 => 0.7.3.0 <-
    }

    int CBuildConfig::buildTimestampAsVersionSegment(const QDateTime &buildTimestamp)
    {
        if (buildTimestamp.isValid())
        {
            const QString bts = buildTimestamp.toString("yyyyMMddHHmm");
            bool ok;
            const long long btsll = bts.toLongLong(&ok); // at least 64bit
            if (!ok) { return 0; }
            // now we have to convert to int, otherwise we would fail 2021
            // max 2147483647 (2^31 - 1)
            //     yyMMddHHmm (years since 2010)
            //                           yyyyMMddHHmm
            const long long yearOffset = 201000000000;
            const int btsInt = static_cast<int>(btsll - yearOffset);
            return btsInt;
        }
        return 0; // intentionally 0 => 0.7.3.0 <-
    }

    const QStringList &CBuildConfig::getBuildAbiParts()
    {
        static const QStringList parts = QSysInfo::buildAbi().split('-');
        return parts;
    }

    namespace Private
    {
        int buildWordSizeImpl()
        {
            if (CBuildConfig::getBuildAbiParts().length() < 3) { return -1; }
            const QString abiWs = CBuildConfig::getBuildAbiParts()[2];
            if (abiWs.contains("32")) { return 32; }
            if (abiWs.contains("64")) { return 64; }
            return -1;
        }
    }

    int CBuildConfig::buildWordSize()
    {
        static const int bws = Private::buildWordSizeImpl();
        return bws;
    }

    bool CBuildConfig::supportFlightGear()
    {
        // for testing set to "true", never commit with TRUE before FG is completed
        // this will be removed when FG is fully productive
        return true; // && isLocalDeveloperDebugBuild();
    }
} // ns

//! \endcond
