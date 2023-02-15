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
#include <QDebug>

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
  clearPatch(t_rootChannel);

  auto biggerSize = t_rootChannel->getL_ChildValueSize()
      < t_rootOutput->getL_ChildValueSize()
      ? t_rootChannel->getL_ChildValueSize()
      : t_rootOutput->getL_ChildValueSize();

  for (int i = 0;
       i < biggerSize;
       i++)
  {
    auto channel = t_rootChannel->getChildValue(i);
    auto channelCast = static_cast<DmxChannel *>(channel);
    auto output = t_rootOutput->getChildValue(i);
    auto outputCast = static_cast<DmxOutput *>(output);
    patchOutputToChannel(channelCast,
                         outputCast);
  }
}

void DmxEngine::straightPatch(RootValue *t_rootChannel,
                              QList<RootValue *> t_L_rootOutput)
{
  clearPatch(t_rootChannel);

  int totalOutputSize = 0;
  // for agregate all outputs and patch in the order
  QList<LeveledValue *> L_output;
  for (const auto &item
       : std::as_const(t_L_rootOutput))
  {
    totalOutputSize += item->getL_ChildValueSize();
    L_output += item->getL_childValue();
  }
  auto biggerSize = t_rootChannel->getL_ChildValueSize()
      < totalOutputSize
      ? t_rootChannel->getL_ChildValueSize()
      : totalOutputSize;

  for (int i = 0;
       i < biggerSize;
       i++)
  {
    auto channel = t_rootChannel->getChildValue(i);
    auto channelCast = static_cast<DmxChannel *>(channel);
    auto output = L_output.at(i);
    auto outputCast = static_cast<DmxOutput *>(output);
    patchOutputToChannel(channelCast,
                         outputCast);
  }
}

void DmxEngine::clearPatch(RootValue *t_rootChannel)
{
  m_dmxPatch->clearPatch();

  auto L_Channel = t_rootChannel->getL_childValue();
  for (const auto &item
       : std::as_const(L_Channel))
  {
    auto channel = static_cast<DmxChannel *>(item);
    auto L_output = channel->getL_controledOutput();
    for (const auto &it
         : std::as_const(L_output))
    {
      it->setChannelControler(nullptr);
    }
    channel->clearControledOutput();
  }
}

void DmxEngine::patchOutputToChannel(DmxChannel *t_channel,
                                     DmxOutput *t_output)
{
  auto channelID = t_channel->getID();
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->addOutputToChannel(channelID,
                                     outputUid_Id))
  {
    t_channel->addOutput(t_output);
    t_output->setChannelControler(t_channel);
  }
  else
  {
    qWarning() << "can't DmxEngine::patchOutputToChannel";
  }
}

void DmxEngine::patchOutputListToChannel(DmxChannel *t_channel,
                                         QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    patchOutputToChannel(t_channel,
                         item);
  }
}

void DmxEngine::unpatchOutputFromChannel(DmxChannel *t_channel,
                                         DmxOutput *t_output)
{
  auto channelID = t_channel->getID();
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->removeOutputFromChannel(channelID,
                                          outputUid_Id))
  {
    t_channel->removeOutput(t_output);
    t_output->setChannelControler(nullptr);
  }
  else
  {
    qWarning () << "can't DmxEngine::unpatchOutputFromChannel";
  }
}

void DmxEngine::unpatchOutputListFromChannel(DmxChannel *t_channel,
                                             QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    unpatchOutputFromChannel(t_channel,
                             item);
  }
}

void DmxEngine::unpatchOutput(DmxOutput *t_output)
{
  auto outputUid_Id = Uid_Id(t_output);
  if (m_dmxPatch->removeOutput(outputUid_Id))
  {
    auto channel = t_output->getChannelControler();
    channel->removeOutput(t_output);
    t_output->setChannelControler(nullptr);
  }
  else
  {
    qWarning () << "can't DmxEngine::unpatchOutput";
  }
}

void DmxEngine::unpatchOutputList(QList<DmxOutput *> t_L_output)
{
  for (const auto &item
       : std::as_const(t_L_output))
  {
    unpatchOutput(item);
  }
}

void DmxEngine::clearChannelPatch(DmxChannel *t_channel)
{
  if (m_dmxPatch->clearChannel(t_channel->getID()))
  {
    auto L_output = t_channel->getL_controledOutput();
    for (const auto &item
         : std::as_const(L_output))
    {
      item->setChannelControler(nullptr);
    }
    t_channel->clearControledOutput();
  }
  else
  {
    qWarning () << "can't DmxEngine::clearChannelPatch";
  }
}

void DmxEngine::clearChannelListPatch(QList<DmxChannel *> t_L_channel)
{
  for (const auto &item
       : std::as_const(t_L_channel))
  {
    clearChannelPatch(item);
  }
}

DmxEngine::DmxEngine(QObject *parent)
  : QObject(parent),
    m_dmxPatch(new DmxPatch())
{
//  clearPatch();
}

/*************************** Uid_Id /************************************/

Uid_Id::Uid_Id(DmxOutput *t_output)
{
  if (t_output)
  {
    m_universeID = t_output->getUniverseID();
    m_outputID = t_output->getID();
  }
}

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

bool DmxPatch::clearChannel(id t_channelID)
{
  return m_MM_patch.remove(t_channelID);
}

bool DmxPatch::addOutputToChannel(const id t_channelID,
                                  const Uid_Id t_outputUid_Id)
{
  // check if it's not yet in the key
  auto L_Uid_Id = m_MM_patch.values(t_channelID);
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    qWarning () << "output already in the patch map";
    return false;
  }

  // chack if it's not in another key
  L_Uid_Id = m_MM_patch.values();
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    removeOutput(t_outputUid_Id);
  }
  m_MM_patch.insert(t_channelID,
                    t_outputUid_Id);
  return true;
}

void DmxPatch::addOutputListToChannel(const id t_channelId,
                                      const QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    addOutputToChannel(t_channelId,
                       item);
  }
}

bool DmxPatch::removeOutput(Uid_Id t_outputUid_Id)
{
  // check if it's in the map
  auto L_Uid_Id = m_MM_patch.values();
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    auto i = m_MM_patch.constBegin();
    while (i != m_MM_patch.constEnd())
    {
      if (i.value() == t_outputUid_Id)
      {
        m_MM_patch.remove(i.key(), i.value());
        break;
      }
      ++i;
    }
    return true;
  }
  qWarning () << "can't remove output from channel";
  return false;
}

void DmxPatch::removeOutputList(QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    removeOutput(item);
  }

}

bool DmxPatch::removeOutputFromChannel(id t_channelID,
                                       Uid_Id t_outputUid_Id)
{
  // choper les values de la key, vérifier et remove
  auto L_Uid_Id = m_MM_patch.values(t_channelID);
  if (L_Uid_Id.contains(t_outputUid_Id))
  {
    m_MM_patch.remove(t_channelID,
                      t_outputUid_Id);
    return true;
  }
  qWarning () << "can't remove output from channel";
  return false;
}

void DmxPatch::removeOutputListFromChannel(id t_channelID,
                                           QList<Uid_Id> t_L_outputUid_Id)
{
  for (const auto &item
       : std::as_const(t_L_outputUid_Id))
  {
    removeOutputFromChannel(t_channelID,
                            item);
  }
}


