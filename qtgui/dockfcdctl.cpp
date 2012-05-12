/* -*- c++ -*- */
/*
 * Copyright 2011-2012 Alexandru Csete OZ9AEC.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <QDebug>
#include "dockfcdctl.h"
#include "ui_dockfcdctl.h"

DockFcdCtl::DockFcdCtl(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockFcdCtl)
{
    ui->setupUi(this);
}

DockFcdCtl::~DockFcdCtl()
{
    delete ui;
}


void DockFcdCtl::setLnbLo(double freq_mhz)
{
    ui->lnbSpinBox->setValue(freq_mhz);
}

double DockFcdCtl::lnbLo()
{
    return ui->lnbSpinBox->value();
}


/*! \brief Set new LNA gain.
 *  \param gain The new gain in the range is -5.0 .. 30.0 dB.
 */
void DockFcdCtl::setLnaGain(float gain)
{
    int index = -1;

    if (gain > 30.5)
        index = 0;
    else if (gain > 27.5)
        index = 1;
    else if (gain > 22.5)
        index = 2;
    else if (gain > 18.5)
        index = 3;
    else if (gain > 16.5)
        index = 4;
    else if (gain > 13.5)
        index = 5;
    else if (gain > 11.5)
        index = 6;
    else if (gain > 8.5)
        index = 7;
    else if (gain > 6.5)
        index = 8;
    else if (gain > 3.5)
        index = 9;
    else if (gain > 1.5)
        index = 10;
    else if (gain > -0.5)
        index = 11;
    else if (gain > -3.5)
        index = 12;
    else
        index = 13;

    ui->lnaComboBox->setCurrentIndex(index);
}

/*! \brief Get current LNA gain. */
float DockFcdCtl::lnaGain()
{
    QString strval = ui->lnaComboBox->currentText();
    float gain;
    bool ok;

    strval.remove(" dB");
    gain = strval.toFloat(&ok);

    if (ok)
    {
        return gain;
    }
    else
    {
        qDebug() << "Could not convert" << strval << "to float";
        return 0.0;
    }

}


/*! \brief Set new frequency correction.
 *  \param corr The new frequency correction in PPM.
 */
void DockFcdCtl::setFreqCorr(int corr)
{
    ui->freqCorrSpinBox->setValue(corr);
}


/*! \brief Get current frequency correction. */
int DockFcdCtl::freqCorr()
{
    return ui->freqCorrSpinBox->value();
}


/*! \brief Set new I/Q gain.
 *  \param gain The new gain in the range -1.0 ... +1.0
 */
void   DockFcdCtl::setIqGain(double gain)
{
    ui->iqGainSpinBox->setValue(gain);
}


/*! \brief Get current I/Q gain.
 *  \return The current I/Q gain in the range -1.0 ... +1.0
 */
double DockFcdCtl::iqGain()
{
    return ui->iqGainSpinBox->value();
}


/*! \brief Set new I/Q phase.
 *  \param gain The new phase in the range -1.0 ... +1.0
 */
void  DockFcdCtl::setIqPhase(double phase)
{
    ui->iqPhaseSpinBox->setValue(phase);
}


/*! \brief Get current I/Q phase.
 *  \return The current I/Q phase in the range -1.0 ... +1.0
 */
double DockFcdCtl::iqPhase()
{
    return ui->iqPhaseSpinBox->value();
}


/*! \brief LNB LO value has changed. */
void DockFcdCtl::on_lnbSpinBox_valueChanged(double value)
{
    emit lnbLoChanged(value);
}


void DockFcdCtl::on_lnaComboBox_activated(const QString value)
{
    QString strval = value;
    float gain;
    bool ok;

    strval.remove(" dB");
    gain = strval.toFloat(&ok);

    if (ok)
    {
        emit lnaGainChanged(gain);
    }
    else
    {
        qDebug() << "DockFcdCtl::on_lnaComboBox_activated : Could not convert" <<
                    value << "to number";
    }

}

/*! \brief Frequency correction changed.
 *  \param value The new frequency correction in ppm.
 */
void DockFcdCtl::on_freqCorrSpinBox_valueChanged(int value)
{
    emit freqCorrChanged(value);
}


/*! \brief I/Q gain changed.
 *  \param value The new I/Q gain
 */
void DockFcdCtl::on_iqGainSpinBox_valueChanged(double value)
{
    emit iqCorrChanged(value, ui->iqPhaseSpinBox->value());
}


/*! \brief I/Q phase changed.
 *  \param The new I/Q phase.
 */
void DockFcdCtl::on_iqPhaseSpinBox_valueChanged(double value)
{
    emit iqCorrChanged(ui->iqGainSpinBox->value(), value);
}

