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
#include <QDebug>
#include <QtMath>

Interpreter::Interpreter(QObject *parent)
    : QObject{parent}
{}

void Interpreter::recieveData(KeypadButton t_button)
{
  if (t_button == KeypadButton::All)
  {
    clearAllSelections();
    m_lastSelectedChannelId = NO_ID;
    m_lastSelectedOutputUidId = NULL_UID_ID;
    emit selectAll();
    return;
  }

  if (t_button == KeypadButton::Clear)
  {
    clearAllSelections();
    m_lastSelectedChannelId = NO_ID;
    m_lastSelectedOutputUidId = NULL_UID_ID;
    emit clearChannelSelection();
    emit clearOutputSelection();
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
      clearAllSelections();
      L_channelsToSelect.append(m_lastSelectedChannelId);
      emit clearChannelSelection();
      emit addChannelSelection(L_channelsToSelect);
    }
    break;
  case KeypadButton::Output :
    if (calculateOutputUidId());
    {
      clearAllSelections();
      L_outputsToSelect.append(m_lastSelectedOutputUidId);
      emit clearOutputSelection();
      emit addOutputSelection(L_outputsToSelect);
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
        if (decay < 0 )
          for (id i = lastSelectedChannelId;
               i <= m_lastSelectedChannelId;
               i--)
            L_channelsToSelect.append(i);
        clearAllSelections();
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
          if (decay < 0 )
            for (id i = lastSelectedOutputUidId.getOutputID();
                 i <= m_lastSelectedOutputUidId.getOutputID();
                 i--)
              L_outputsToSelect.append(Uid_Id(lastSelectedOutputUidId.getUniverseID()
                                              , i));
          clearAllSelections();
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
        emit addChannelSelection(L_channelsToSelect);
      }
    }
    else if (!(m_lastSelectedOutputUidId == NULL_UID_ID))
    {
      if (calculateOutputUidId())
      {
        L_outputsToSelect.append(m_lastSelectedOutputUidId);
        clearAllSelections();
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
        clearAllSelections();
        emit removeChannelSelection(L_channelsToSelect);
      }
    }
    else if (!(m_lastSelectedOutputUidId == NULL_UID_ID))
    {
      if (calculateOutputUidId())
      {
        L_outputsToSelect.append(m_lastSelectedOutputUidId);
        clearAllSelections();
        emit removeOutputSelection(L_outputsToSelect);
      }
    }
    break;
  case KeypadButton::Arobase :
    emit setLevel(calculateDmx());
    break;
  case KeypadButton::Pluspc :
  case KeypadButton::Moinspc :
  case KeypadButton::Time :
  case KeypadButton::Timein :
  case KeypadButton::Timeout :
  case KeypadButton::Delayin :
  case KeypadButton::Delayout :
  case KeypadButton::Cue :
  case KeypadButton::Group :
  case KeypadButton::Record :
  case KeypadButton::Update :
  case KeypadButton::Delete :
  case KeypadButton::Patch :
  case KeypadButton::Unpatch :
  case KeypadButton::Step :
  case KeypadButton::Goto :
  case KeypadButton::Help :
  default : break;
  }
}

void Interpreter::clearAllSelections()
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
    qDebug() << m_lastSelectedChannelId;
  }
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
      qDebug() << "uid" << universeId;
    }
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
    qDebug() << outputId;
  }
  m_lastSelectedOutputUidId.setOutputID(outputId);

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
  return (level > 255) ?
             255 :  level;
}

bool Interpreter::calculateFloatTime()
{
  return true;
}


