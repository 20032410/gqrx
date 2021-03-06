/* -*- c++ -*- */
/*
 * Copyright 2011 Alexandru Csete OZ9AEC.
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
#include <QString>
#include <QChar>
#include <QTime>
#include <QDebug>
#include "arissattlm.h"
#include "ui_arissattlm.h"
#include "tlm/arissat/ss_types_common.h"

extern "C" {
    #include "tlm/arissat/scale_therm.h"
    #include "tlm/arissat/scale_psu.h"
    #include "tlm/arissat/scale_ppt.h"
}


ArissatTlm::ArissatTlm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArissatTlm),
    deg(QChar(0x00b0))
{
    ui->setupUi(this);

    createPptLabels();
}

ArissatTlm::~ArissatTlm()
{
    delete ui;
}


/*! \brief Create labels for PPT telemetry and add them to the grid layout. */
void ArissatTlm::createPptLabels()
{
    // PPT telemetry labels
    for (int i = 0; i < PPT_COUNT; i++) {
        // Panel energy
        pptEnergy[i] = new QLabel("0", this);
        pptEnergy[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptEnergy[i], 1, 1+i, 1, 1);
        // Panel voltage
        pptVolt[i] = new QLabel("0.000 V", this);
        pptVolt[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptVolt[i], 2, 1+i, 1, 1);
        // Panel current
        pptAmp[i] = new QLabel("0.000 A", this);
        pptAmp[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptAmp[i], 3, 1+i, 1, 1);
        // Panel temperature
        pptTempPanel[i] = new QLabel(QString("0 %1C").arg(deg), this);
        pptTempPanel[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptTempPanel[i], 4, 1+i, 1, 1);
        // Inductor temperature
        pptTempInd[i] = new QLabel(QString("0 %1C").arg(deg), this);
        pptTempInd[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptTempInd[i], 5, 1+i, 1, 1);
        // Diode temperature
        pptTempDiode[i] = new QLabel(QString("0 %1C").arg(deg), this);
        pptTempDiode[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptTempDiode[i], 6, 1+i, 1, 1);
        // FET temperature
        pptTempFet[i] = new QLabel(QString("0 %1C").arg(deg), this);
        pptTempFet[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptTempFet[i], 7, 1+i, 1, 1);
        // Solar panel PWM current setpoint
        pptPwm[i] = new QLabel("0.000 A", this);
        pptPwm[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptPwm[i], 8, 1+i, 1, 1);
        // Telemetry age
        pptAge[i] = new QLabel("0", this);
        pptAge[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptAge[i], 9, 1+i, 1, 1);
        // Corrupt packet count
        pptCorrupt[i] = new QLabel("0", this);
        pptCorrupt[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->pptGridLayout->addWidget(pptCorrupt[i], 10, 1+i, 1, 1);
    }

}

/*! \brief Process new data. */
void ArissatTlm::processData(QByteArray &data)
{
    QByteArray ba(data);
    ss_telem_t tlm;

    // check first byte: 'T'
    if (!ba.startsWith('T')) {
        qDebug() << "Data does not start with 'T'" << ba[0];
        return;
    }
    if (ba.size() != 371) {
        qDebug() << "Data length is not 371 bytes";
        return;
    }

    // extract the frame counter
    memcpy(&frameCounter, ba.data()+1, 4);

    // now copy the ss_telem_t structure
    memcpy(&tlm, ba.data()+27, 340);

    updateMissionData(tlm);
    updateIhuTemps(tlm);
    updateBattery(tlm);
    updatePower(tlm);
    updatePpt(tlm);
}


/*! \brief Display main mission data. */
void ArissatTlm::updateMissionData(ss_telem_t &tlm)
{
    // MET
    quint32 days = tlm.mission_time / 86400;
    QTime   t0(0,0), met;
    int met_sec = (int) (tlm.mission_time - days*86400);
    met = t0.addSecs(met_sec);
    ui->metLabel->setText(QString("MET:  %1d %2 ").arg(days).arg(met.toString("HH:mm:ss")));

    // telemetry frame counter
    ui->frameCountLabel->setText(QString::number(frameCounter));

    // Mission mode
    // TODO: colors, see http://www.qtcentre.org/threads/195-Setting-text-color-on-QLabel
    switch (tlm.mission_mode) {
    case EMERGENCY_PWR:
        ui->modeLabel->setText(tr("Emergency Power"));
        break;
    case LOW_PWR:
        ui->modeLabel->setText(tr("Low Power"));
        break;
    case HIGH_PWR:
        ui->modeLabel->setText(tr("High Power"));
        break;
    case TXINHIBIT_PWR:
        ui->modeLabel->setText(tr("TX Inhibit"));
        break;
    default:
        ui->modeLabel->setText(tr("Invalid mode"));
        break;
    }
}


/*! \brief Display IHU temperatures. */
void ArissatTlm::updateIhuTemps(ss_telem_t &tlm)
{
    ui->tempRfMod->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.rf)).arg(deg));
    ui->tempIhuEnc->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.ihu_enclosure)).arg(deg));
    ui->tempCtl->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.control_panel)).arg(deg));
    ui->tempExp->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.experiment)).arg(deg));
    ui->tempCamBottom->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.bottom_cam)).arg(deg));
    ui->tempCamTop->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.top_cam)).arg(deg));
    ui->tempBattery->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.battery)).arg(deg));
    ui->tempIhu->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.ihu_pcb)).arg(deg));
    ui->tempPsu->setText(QString("%1 %2C").arg((int)scale_thermistor_C(tlm.ihu_temps.psu_pcb)).arg(deg));
}


/*! \brief Show battery telemetry. */
void ArissatTlm::updateBattery(ss_telem_t &tlm)
{
    // Battery status
    if (scale_psu_i_batt(tlm.power.batt_status.i_raw, tlm.power.batt_status.ref2p5_raw) > 0)
        ui->battState->setText(tr("Charging"));
    else
        ui->battState->setText(tr("Discharging"));

    // Battery voltage
    ui->battVolt->setText(QString("%1 V").arg(scale_psu_v_batt(tlm.power.batt_status.v_raw, tlm.power.batt_status.ref2p5_raw), 7, 'f', 3));

    // Battery current
    ui->battCurrent->setText(QString("%1 A").arg(scale_psu_i_batt(tlm.power.batt_status.i_raw, tlm.power.batt_status.ref2p5_raw), 6, 'f', 3));

    // Vdd - 2.5V engineering
    ui->battVdd->setText(QString("%1 V").arg(scale_psu_vdd(tlm.power.batt_status.ref2p5_raw), 6, 'f', 3));

    // Vdd - 2.5V ref raw
    ui->battVref->setText(QString("0x%1").arg(tlm.power.batt_status.ref2p5_raw, 0, 16));

    // Net charge
    ui->battCharge->setText(QString("%1 C").arg(scale_psu_c_net_batt_s48(tlm.power.batt_status.c_battery_net_raw, tlm.power.batt_status.ref2p5_raw), 7, 'f', 0));
}

/*! \brief Show power telemetry. */
void ArissatTlm::updatePower(ss_telem_t &tlm)
{
    double amp;

    // IHU
    if (tlm.power.ihu.status)
        ui->ihuStatus->setText(tr("ON"));
    else
        ui->ihuStatus->setText(tr("OFF"));
    amp = scale_psu_i_ihu(tlm.power.ihu.i_raw_lsb + ((tlm.power.ihu.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->ihuAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));

    // SDX
    if (tlm.power.sdx.status)
        ui->sdxStatus->setText(tr("ON"));
    else
        ui->sdxStatus->setText(tr("OFF"));
    amp = scale_psu_i_sdx(tlm.power.sdx.i_raw_lsb + ((tlm.power.sdx.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->sdxAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));

    // Experiment
    if (tlm.power.experiment.status)
        ui->expStatus->setText(tr("ON"));
    else
        ui->expStatus->setText(tr("OFF"));
    amp = scale_psu_i_experiment(tlm.power.experiment.i_raw_lsb + ((tlm.power.experiment.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->expAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));

    // Camera
    ui->camStatus->setText(QString("%1%2%3%4").
                           arg(tlm.power.camera.status1 ? '1' : '-').
                           arg(tlm.power.camera.status2 ? '1' : '-').
                           arg(tlm.power.camera.status3 ? '1' : '-').
                           arg(tlm.power.camera.status4 ? '1' : '-'));
    amp = scale_psu_i_camera(tlm.power.camera.i_raw_lsb + ((tlm.power.camera.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->camAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));

    // 5 volt
    if (tlm.power.ps5v.status)
        ui->psu5vStatus->setText(tr("ON"));
    else
        ui->psu5vStatus->setText(tr("OFF"));
    amp = scale_psu_i_5v(tlm.power.ps5v.i_raw_lsb + ((tlm.power.ps5v.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->psu5vAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));

    // 8 volt
    if (tlm.power.ps8v.status)
        ui->psu8vStatus->setText(tr("ON"));
    else
        ui->psu8vStatus->setText(tr("OFF"));
    amp = scale_psu_i_8v(tlm.power.ps8v.i_raw_lsb + ((tlm.power.ps8v.i_raw_msb & 0x0f) << 8),
                             tlm.power.batt_status.ref2p5_raw);
    ui->psu8vAmp->setText(QString("%1 A").arg(amp, 6, 'f', 3));
}


/*! \brief Update PPT telemetry. */
void ArissatTlm::updatePpt(ss_telem_t &tlm)
{
    quint8 lsb, msb;

    for (int i = 0; i < PPT_COUNT; i++) {
        pptEnergy[i]->setText(QString::number(U48TOU64(tlm.ppt_status[i].sp_energy_osc_raw)));
        pptVolt[i]->setText(QString("%1 V").arg(scale_ppt_sp_voltage(tlm.ppt_status[i].sp_voltage_raw), 7, 'f', 3));
        pptAmp[i]->setText(QString("%1 A").arg(scale_ppt_sp_current(tlm.ppt_status[i].sp_current_adc_raw), 7, 'f', 3));
        pptPwm[i]->setText(QString("%1 A").arg(scale_ppt_pwm_setpoint(tlm.ppt_status[i].osc_ccp_current_setpt), 7, 'f', 3));
        pptAge[i]->setText(QString::number((int)tlm.ppt_status[i].aged));
        pptCorrupt[i]->setText(QString::number((int)tlm.ppt_status[i].corrupt));

        // Solar panel temperature
        msb = (tlm.ppt_status[i].sp_temp_raw_msb_diode_temp_raw_msb & 0xF0) << 4;
        lsb = tlm.ppt_status[i].sp_temp_raw_lsb;
        pptTempPanel[i]->setText(QString("%1 %2C").
                                 arg((int)scale_thermistor_C(msb+lsb)).
                                 arg(deg));
        // Inductor temperature
        msb = (tlm.ppt_status[i].ind_temp_raw_msb_fet_temp_raw_msb  & 0xF0) << 4;
        lsb = tlm.ppt_status[i].ind_temp_raw_lsb;
        pptTempInd[i]->setText(QString("%1 %2C").
                               arg((int)scale_thermistor_C(msb+lsb)).
                               arg(deg));
        // Diode temperature
        msb = (tlm.ppt_status[i].sp_temp_raw_msb_diode_temp_raw_msb  & 0x0F) << 4;
        lsb = tlm.ppt_status[i].diode_temp_raw_lsb;
        pptTempDiode[i]->setText(QString("%1 %2C").
                               arg((int)scale_thermistor_C(msb+lsb)).
                               arg(deg));
        // FET temperature
        msb = (tlm.ppt_status[i].ind_temp_raw_msb_fet_temp_raw_msb  & 0x0F) << 4;
        lsb = tlm.ppt_status[i].fet_temp_raw_lsb;
        pptTempFet[i]->setText(QString("%1 %2C").
                               arg((int)scale_thermistor_C(msb+lsb)).
                               arg(deg));
    }
}
