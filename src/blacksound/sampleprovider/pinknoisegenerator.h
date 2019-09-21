/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef PINKNOISEGENERATOR_H
#define PINKNOISEGENERATOR_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"

#include <QRandomGenerator>
#include <QVector>

#include <array>

namespace BlackSound
{
    namespace SampleProvider
    {
        //! Pink noise generator
        class BLACKSOUND_EXPORT CPinkNoiseGenerator : public ISampleProvider
        {
            Q_OBJECT

        public:
            //! Noise generator
            CPinkNoiseGenerator(QObject *parent = nullptr) : ISampleProvider(parent) {}

            virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

            void setGain(double gain) { m_gain = gain; }

        private:
            QRandomGenerator random;
            std::array<double, 7> pinkNoiseBuffer = {0};
            double m_gain = 0.0;
        };
    }
}

#endif // guard
