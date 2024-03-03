/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2022 by Admiral_Fish
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "Filter.hpp"
#include "ui_Filter.h"
#include <Core/Util/Utility.hpp>
#include <Forms/Controls/Controls.hpp>

Filter::Filter(QWidget *parent) : QWidget(parent), ui(new Ui::Filter)
{
    ui->setupUi(this);

    ui->comboBoxAbility->setup({ 255, 0, 1 });
    ui->comboBoxGender->setup({ 255, 0, 1 });
    ui->checkListHiddenPower->setup(Utility::getHiddenPowers());
    ui->checkListNature->setup(Utility::getNatures());
    ui->comboBoxShiny->setup({ 255, 1, 2, 3 });

    connect(ui->pushButtonEncounterSlotAny, &QPushButton::clicked, ui->checkListEncounterSlot, &CheckList::resetChecks);
    connect(ui->pushButtonHiddenPowerAny, &QPushButton::clicked, ui->checkListHiddenPower, &CheckList::resetChecks);
    connect(ui->pushButtonNatureAny, &QPushButton::clicked, ui->checkListNature, &CheckList::resetChecks);
}

Filter::~Filter()
{
    delete ui;
}

std::array<u8, 6> Filter::getMinIVs() const
{
    return ui->ivFilter->getLower();
}

std::array<u8, 6> Filter::getMaxIVs() const
{
    return ui->ivFilter->getUpper();
}

u8 Filter::getAbility() const
{
    return ui->comboBoxAbility->getCurrentByte();
}

u8 Filter::getGender() const
{
    return ui->comboBoxGender->getCurrentByte();
}

std::vector<bool> Filter::getEncounterSlots()
{
    return ui->checkListEncounterSlot->getChecked();
}

void Filter::setEncounterSlots(const std::vector<std::string> &encounterSlots) const
{
    ui->checkListEncounterSlot->setup(encounterSlots);
}

void Filter::toggleEncounterSlots(const std::vector<bool> &encounterSlots) const
{
    ui->checkListEncounterSlot->setChecks(encounterSlots);
}

void Filter::resetEncounterSlots() const
{
    ui->checkListEncounterSlot->resetChecks();
}

std::vector<bool> Filter::getHiddenPowers()
{
    return ui->checkListHiddenPower->getChecked();
}

std::vector<bool> Filter::getNatures()
{
    return ui->checkListNature->getChecked();
}

u8 Filter::getShiny() const
{
    return ui->comboBoxShiny->getCurrentByte();
}

void Filter::enableControls(Controls control)
{
    if ((control & Controls::IVs) != Controls::None)
    {
        ui->ivFilter->setVisible(true);
    }

    if ((control & Controls::Ability) != Controls::None)
    {
        ui->labelAbility->setVisible(true);
        ui->comboBoxAbility->setVisible(true);
    }

    if ((control & Controls::Gender) != Controls::None)
    {
        ui->labelGender->setVisible(true);
        ui->comboBoxGender->setVisible(true);
    }

    if ((control & Controls::EncounterSlots) != Controls::None)
    {
        ui->labelEncounterSlot->setVisible(true);
        ui->checkListEncounterSlot->setVisible(true);
        ui->pushButtonEncounterSlotAny->setVisible(true);
    }

    if ((control & Controls::HiddenPowers) != Controls::None)
    {
        ui->labelHiddenPower->setVisible(true);
        ui->checkListHiddenPower->setVisible(true);
        ui->pushButtonHiddenPowerAny->setVisible(true);
    }

    if ((control & Controls::Natures) != Controls::None)
    {
        ui->labelNature->setVisible(true);
        ui->checkListNature->setVisible(true);
        ui->pushButtonNatureAny->setVisible(true);
    }

    if ((control & Controls::Shiny) != Controls::None)
    {
        ui->labelShiny->setVisible(true);
        ui->comboBoxShiny->setVisible(true);
    }
}

void Filter::disableControls(Controls control)
{
    if ((control & Controls::IVs) != Controls::None)
    {
        ui->ivFilter->setVisible(false);
    }

    if ((control & Controls::Ability) != Controls::None)
    {
        ui->labelAbility->setVisible(false);
        ui->comboBoxAbility->setVisible(false);
    }

    if ((control & Controls::Gender) != Controls::None)
    {
        ui->labelGender->setVisible(false);
        ui->comboBoxGender->setVisible(false);
    }

    if ((control & Controls::EncounterSlots) != Controls::None)
    {
        ui->labelEncounterSlot->setVisible(false);
        ui->checkListEncounterSlot->setVisible(false);
        ui->pushButtonEncounterSlotAny->setVisible(false);
    }

    if ((control & Controls::HiddenPowers) != Controls::None)
    {
        ui->labelHiddenPower->setVisible(false);
        ui->checkListHiddenPower->setVisible(false);
        ui->pushButtonHiddenPowerAny->setVisible(false);
    }

    if ((control & Controls::Natures) != Controls::None)
    {
        ui->labelNature->setVisible(false);
        ui->checkListNature->setVisible(false);
        ui->pushButtonNatureAny->setVisible(false);
    }

    if ((control & Controls::Shiny) != Controls::None)
    {
        ui->labelShiny->setVisible(false);
        ui->comboBoxShiny->setVisible(false);
    }
}

void Filter::enableHiddenAbility()
{
    ui->comboBoxAbility->addItem("H", 2);
}
