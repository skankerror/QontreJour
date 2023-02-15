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

#include "dmxengine.h"
#include <QChar>

DmxEngine *DmxEngine::instance()
{
  static DmxEngine inst;
  return &inst;
}

DmxEngine::~DmxEngine()
{
  delete m_dmxPatch;
  m_dmxPatch = nullptr;
}

void DmxEngine::straightPatch(RootValue *t_rootChannel,
                              RootValue *t_rootOutput)
{

}

void DmxEngine::straightPatch(RootValue *t_rootChannel,
                              QList<RootValue *> t_L_rootOutput)
{

}

void DmxEngine::clearPatch(RootValue *t_rootChannel)
{
  m_dmxPatch->clearPatch();
  // TODO : clear connections with root channel
}

DmxEngine::DmxEngine(QObject *parent)
  : QObject(parent),
    m_dmxPatch(new DmxPatch())
{}

/*************************** Uid_Id /************************************/

Uid_Id::Uid_Id(QString &t_string)
{
  if (t_string.size() > 3
      && t_string.at(0).isDigit())
  {
    auto tempFloat = t_string.toFloat();
    m_universeID = (uid)tempFloat;
    // TODO : implement this fucking string method
//    for (qsizetype i = 0;
//         i < t_string.size();
//         i++)
//    {
//      auto value = t_string.at(i);
//      if (value.isDigit())

  }
}

/******************************** DmxPatch *******************************/

void DmxPatch::clearPatch()
{
  m_MM_patch.clear();
}

void DmxPatch::clearChannel(id t_channelID)
{

}

void DmxPatch::addOutputToChannel(id t_channelID, Uid_Id t_outputUid_Id)
{

}

void DmxPatch::addOutputListToChannel(id t_channelId, QList<Uid_Id> t_L_outputUid_Id)
{

}

void DmxPatch::removeOutputFromChannel(id t_channelID, Uid_Id t_outputUid_Id)
{

}

void DmxPatch::removeOutputListFromChannel(id t_channelID, QList<Uid_Id> t_L_outputUid_Id)
{

}


