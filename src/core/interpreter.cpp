/*
 * (c) 2023 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "interpreter.h"
#include "dmxmanager.h"
#include <QDebug>
#include <QtMath>

Interpreter::Interpreter(QObject *parent)
    : QObject{parent}
{}

void Interpreter::recieveData(KeypadButton t_button)
{
  if (t_button == KeypadButton::All)
  {
    clearValue();
    m_lastSelectedChannelId = NO_ID;
    m_lastSelectedOutputUidId = NULL_UID_ID;
    emit selectAll();
    return;
  }

  if (t_button == KeypadButton::Clear)
  {
    clearValue();
    m_lastSelectedChannelId = NO_ID;
    m_lastSelectedOutputUidId = NULL_UID_ID;
    emit clearChannelSelection();
    emit clearOutputSelection();
    return;
  }

  if (t_button == KeypadButton::Pluspc)
  {
    emit plusPercent();
    return;
  }

  if (t_button == KeypadButton::Moinspc)
  {
    emit moinsPercent();
    return;
  }

  if (t_button == KeypadButton::Record)
  {
    if (m_selectedCueId)
      emit recordNewCue(m_selectedCueId);
    else if (m_selectedGroupId != NO_ID)
      emit recordGroup(m_selectedGroupId);
    else
      emit recordNextCue();
    resetInterpreter();
    return;
  }

  if (!m_isValued
      && (t_button > KeypadButton::Dot)) // no value yet and it's not value
  {
    emit sendError(); // syntax error
    return;
  }

  // if it's a value
  if (t_button <= KeypadButton::Dot)
  {
    // we add to list
    m_L_digits.append(t_button);
    m_isValued = true;
    return;
  }

  // it's not a value.
  QList<id> L_channelsToSelect;
  QList<Uid_Id> L_outputsToSelect;
  switch (t_button)
  {
  case KeypadButton::Channel :
    if (calculateChannelId())
    {
      resetInterpreter();
      L_channelsToSelect.append(m_lastSelectedChannelId);
      emit clearChannelSelection();
      emit addChannelSelection(L_channelsToSelect);
    }
    break;
  case KeypadButton::Output :
    if (calculateOutputUidId());
    {
      resetInterpreter();
      L_outputsToSelect.append(m_lastSelectedOutputUidId);
      emit clearOutputSelection();
      emit addOutputSelection(L_outputsToSelect);
    }
    break;
  case KeypadButton::Cue :
    if (calculateCueId())
    {
      clearValue();
      clearGroup();
    }
    break;
  case KeypadButton::Group :
    if (calculateGroupId())
    {
      clearValue();
      clearCue();
    }
    break;
  case KeypadButton::Thru :
    if (m_lastSelectedChannelId != NO_ID)
    {
      id lastSelectedChannelId = m_lastSelectedChannelId;
      if (calculateChannelId())
      {
        int decay = m_lastSelectedChannelId - lastSelectedChannelId;
        if (decay > 0)
          for (id i = lastSelectedChannelId;
               i <= m_lastSelectedChannelId;
               i++)
            L_channelsToSelect.append(i);
        else if (decay < 0 )
          for (id i = lastSelectedChannelId;
               i <= m_lastSelectedChannelId;
               i--)
            L_channelsToSelect.append(i);
        resetInterpreter();
        emit addChannelSelection(L_channelsToSelect);
      }
    }
    else if (!(m_lastSelectedOutputUidId == NULL_UID_ID))
    {
      // déterminer si c'est le même univers
      Uid_Id lastSelectedOutputUidId = m_lastSelectedOutputUidId;
      if (calculateOutputUidId())
        if (lastSelectedOutputUidId.getUniverseID()
            == m_lastSelectedOutputUidId.getUniverseID())
        {
          int decay = lastSelectedOutputUidId.getOutputID()
                      - m_lastSelectedOutputUidId.getOutputID();
          if (decay > 0)
            for (id i = lastSelectedOutputUidId.getOutputID();
                 i <= m_lastSelectedOutputUidId.getOutputID();
                 i++)
              L_outputsToSelect.append(Uid_Id(lastSelectedOutputUidId.getUniverseID()
                                              , i));
          else if (decay < 0 )
            for (id i = lastSelectedOutputUidId.getOutputID();
                 i <= m_lastSelectedOutputUidId.getOutputID();
                 i--)
              L_outputsToSelect.append(Uid_Id(lastSelectedOutputUidId.getUniverseID()
                                              , i));
          resetInterpreter();
          emit addOutputSelection(L_outputsToSelect);
        }
    }
    break;
  case KeypadButton::Plus :
    if (m_lastSelectedChannelId != NO_ID)
    {
      if (calculateChannelId())
      {
        L_channelsToSelect.append(m_lastSelectedChannelId);
        resetInterpreter();
        emit addChannelSelection(L_channelsToSelect);
      }
    }
    else if (!(m_lastSelectedOutputUidId == NULL_UID_ID))
    {
      if (calculateOutputUidId())
      {
        L_outputsToSelect.append(m_lastSelectedOutputUidId);
        resetInterpreter();
        emit addOutputSelection(L_outputsToSelect);
      }
    }
    break;
  case KeypadButton::Moins :
    if (m_lastSelectedChannelId != NO_ID)
    {
      if (calculateChannelId())
      {
        L_channelsToSelect.append(m_lastSelectedChannelId);
        resetInterpreter();
        emit removeChannelSelection(L_channelsToSelect);
      }
    }
    else if (!(m_lastSelectedOutputUidId == NULL_UID_ID))
    {
      if (calculateOutputUidId())
      {
        L_outputsToSelect.append(m_lastSelectedOutputUidId);
        resetInterpreter();
        emit removeOutputSelection(L_outputsToSelect);
      }
    }
    break;
  case KeypadButton::ArobaseDmx :
    emit setLevel(calculateDmx());
    resetInterpreter();
    break;
  case KeypadButton::ArobasePercent :
    emit setLevel(percentToDmx(calculatePercent()));
    resetInterpreter();
    break;
  case KeypadButton::Time :
    if (calculateFloatTime())
    {
      emit setTimeIn(m_time);
      emit setTimeOut(m_time);
    }
    break;
  case KeypadButton::Timein :
    if (calculateFloatTime())
      emit setTimeIn(m_time);
    break;
  case KeypadButton::Timeout :
    if (calculateFloatTime())
      emit setTimeOut(m_time);
    break;
  case KeypadButton::Delayin :
    if (calculateFloatTime())
      emit setDelayIn(m_time);
    break;
  case KeypadButton::Delayout :
    if (calculateFloatTime())
      emit setDelayOut(m_time);
    break;
  case KeypadButton::Update :
    if (m_selectedCueId)
      emit updateCue(m_selectedCueId);
    else if (m_selectedGroupId != NO_ID)
      emit recordGroup(m_selectedGroupId);
    else
      emit updateCurrentCue();
    resetInterpreter();
    break;
  case KeypadButton::Goto :
    if (m_isValued)
    {
      if (calculateCueId())
        emit gotoCue(m_selectedCueId);
    }
    else if (m_selectedCueId)
      emit gotoCue(m_selectedCueId);
    else if (m_stepId != NO_ID)
      emit gotoStep(m_stepId);
    else
      emit sendError_NoValueSpecified();
    break;
  case KeypadButton::Step :
    if (calculateStepId())
      resetInterpreter();
    break;
  case KeypadButton::Delete :
    if (m_selectedCueId != NO_ID)
      emit deleteCue(m_selectedCueId);
    else if (m_stepId > 0)
      emit deleteStep(m_stepId);
    else if (m_selectedGroupId != NO_ID)
      emit deleteGroup(m_selectedGroupId);
    break;
  case KeypadButton::Patch :
  case KeypadButton::Unpatch :
  case KeypadButton::Help :
  default : break;
  }
}

void Interpreter::clearValue()
{
  m_isValued = false;
  m_L_digits.squeeze();
  m_L_digits.clear();
}

bool Interpreter::calculateChannelId()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }

  m_lastSelectedChannelId = 0;

  // find first dot
  auto i = m_L_digits.indexOf(KeypadButton::Dot);
  if (i > -1)
    m_L_digits.remove(i, m_L_digits.size() - i);

  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    m_lastSelectedChannelId += digit * qPow(10, i);
  }
  m_lastSelectedChannelId--; // human - machine translation
  qDebug() << m_lastSelectedChannelId;
  clearValue();
  return true;
}

bool Interpreter::calculateOutputUidId()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }

  m_lastSelectedOutputUidId = NULL_UID_ID;
  uid universeId = 0;

  // find first dot
  auto index = m_L_digits.indexOf(KeypadButton::Dot);

  if (index != -1) // a universe is precised
  {
    int count = m_L_digits.count(KeypadButton::Dot);
    if (count > 1) // there are other dots
    { // we erase all others dot and their following digits
      auto lastIndex = m_L_digits.lastIndexOf(KeypadButton::Dot);
      while (lastIndex != index)
      {
        m_L_digits.remove(lastIndex,
                          m_L_digits.size() - lastIndex);
      }
    }
    for (int i = 0;
         i < m_L_digits.size() - 1 - index;
         i++)
    {
      KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
      universeId += digit * qPow(10, i);
    }
    universeId--; // human - machine translation
    qDebug() << "uid" << universeId;
    // now we erase dot and following digits
    m_L_digits.remove(index,
                      m_L_digits.size() - index);

  }
  m_lastSelectedOutputUidId.setUniverseID(universeId);

  // now don't have any dots in list, only output id
  id outputId = 0;
  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    outputId += digit * qPow(10, i);
  }
  outputId--; // human - machine translation
  qDebug() << outputId;
  m_lastSelectedOutputUidId.setOutputID(outputId);

  clearValue();
  return true;
}

bool Interpreter::calculateGroupId()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }

  m_selectedGroupId = 0;

  // find first dot
  auto i = m_L_digits.indexOf(KeypadButton::Dot);
  if (i > -1)
    m_L_digits.remove(i, m_L_digits.size() - i);

  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    m_selectedGroupId += digit * qPow(10, i);
  }
  m_selectedGroupId--; // human - machine translation
  qDebug() << m_selectedGroupId;
  clearValue();
  return true;
}

bool Interpreter::calculateCueId()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }
  m_selectedCueId = 0.0f;

  // find first dot
  auto index = m_L_digits.indexOf(KeypadButton::Dot);
  if (index > -1) // il y a un point
  {
    int count = m_L_digits.count(KeypadButton::Dot);
    if (count > 1) // there are other dots
    { // we erase all others dot and their following digits
      auto lastIndex = m_L_digits.lastIndexOf(KeypadButton::Dot);
      while (lastIndex != index)
      {
        m_L_digits.remove(lastIndex,
                          m_L_digits.size() - lastIndex);
      }
    }
    for (int i = index + 1;
         i < m_L_digits.size();
         i++)
    {
      KeypadButton digit = m_L_digits.at(i);
      m_selectedCueId += digit * qPow(10, index - i);
    }
    // now we erase dot and following digits
    m_L_digits.remove(index,
                      m_L_digits.size() - index);
  }
  for (int i = 0; // no more dots
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    m_selectedCueId += digit * qPow(10, i);
  }
  qDebug() << m_selectedCueId;
  clearValue();
  return true;
}

dmx Interpreter::calculateDmx()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return NULL_DMX;
  }

  dmx level = NULL_DMX;

  // find first dot
  auto i = m_L_digits.indexOf(KeypadButton::Dot);
  if (i > -1)
    m_L_digits.remove(i, m_L_digits.size() - i);

  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    level += digit * qPow(10, i);
    qDebug() << level;
  }
  clearValue();
  return (level > 255) ?
             255 :  level;
}

percent Interpreter::calculatePercent()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return NULL_DMX;
  }

  percent level = NULL_DMX;

  // find first dot
  auto i = m_L_digits.indexOf(KeypadButton::Dot);
  if (i > -1)
    m_L_digits.remove(i, m_L_digits.size() - i);

  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    level += digit * qPow(10, i);
    qDebug() << level;
  }
  clearValue();
  return (level > 100) ?
             100 :  level;
}

bool Interpreter::calculateFloatTime()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }
  m_time = 0.0f;

  // find first dot
  auto index = m_L_digits.indexOf(KeypadButton::Dot);
  if (index > -1) // il y a un point
  {
    int count = m_L_digits.count(KeypadButton::Dot);
    if (count > 1) // there are other dots
    { // we erase all others dot and their following digits
      auto lastIndex = m_L_digits.lastIndexOf(KeypadButton::Dot);
      while (lastIndex != index)
      {
        m_L_digits.remove(lastIndex,
                          m_L_digits.size() - lastIndex);
      }
    }
    for (int i = index + 1;
         i < m_L_digits.size();
         i++)
    {
      KeypadButton digit = m_L_digits.at(i);
      m_time += digit * qPow(10, index - i);
    }
    // now we erase dot and following digits
    m_L_digits.remove(index,
                      m_L_digits.size() - index);
  }
  for (int i = 0; // no more dots
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    m_time += digit * qPow(10, i);
  }
  qDebug() << m_time;
  clearValue();
  return true;
}

bool Interpreter::calculateStepId()
{
  if (!m_isValued)
  {
    emit sendError_NoValueSpecified();
    return false;
  }
  // NOTE : if no value is truely specified this will be id 0 !
  m_stepId = 0;

  // find first dot
  auto i = m_L_digits.indexOf(KeypadButton::Dot);
  if (i > -1)
    m_L_digits.remove(i, m_L_digits.size() - i);

  for (int i = 0;
       i < m_L_digits.size();
       i++)
  {
    KeypadButton digit = m_L_digits.at(m_L_digits.size() - 1 - i);
    m_stepId += digit * qPow(10, i);
  }
  m_stepId--; // human - machine translation
  qDebug() << m_stepId;
  clearValue();
  clearCue();
  return true;
}

percent Interpreter::dmxToPercent(dmx t_dLevel)
{
  if (t_dLevel > 255)
    return 100;

  qreal pLevel_f = float(t_dLevel) / 2.55f;
  int floor = qFloor(pLevel_f);
  int ceil = qCeil(pLevel_f);
  return (pLevel_f > (float)floor + 0.5f) ?
             ceil : floor;
}

dmx Interpreter::percentToDmx(percent t_pLevel)
{
  if (t_pLevel > 100)
    return 255;

  qreal dLevel_f = (float)t_pLevel * 2.55f;
  int floor = qFloor(dLevel_f);
  int ceil = qCeil(dLevel_f);
  return (dLevel_f > (float)floor + 0.5f) ?
             ceil : floor;
}
