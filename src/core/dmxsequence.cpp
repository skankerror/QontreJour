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

#include "dmxsequence.h"
#include <QDebug>

DmxSequence::DmxSequence(QObject *parent)
  : QObject{parent}
{

}

DmxScene *DmxSequence::getSelectedScene()
{
  if ((m_selectedScene > -1)
      && (m_selectedScene < m_L_dmxScene.size()))
    return m_L_dmxScene.at(m_selectedScene);
  else
    return nullptr;
}

DmxScene *DmxSequence::getNextScene()
{
  if ((m_selectedScene > -1)
      && (m_selectedScene < m_L_dmxScene.size()))
    if (m_selectedScene == m_L_dmxScene.size() - 1) // it's last, get first
      return m_L_dmxScene.first();
    else
      return m_L_dmxScene.at(m_selectedScene + 1);
  else
    return nullptr;
}

DmxScene *DmxSequence::getScene(const int t_step)
{
  if ((t_step > -1)
      && (t_step < m_L_dmxScene.size()))
    return m_L_dmxScene.at(t_step);
  else
    return nullptr;
}

void DmxSequence::addScene(DmxScene *t_scene)
{
  // we check if the pointer isn't null and if the output is not in the list.
  if(t_scene && !m_L_dmxScene.contains(t_scene))
  {
    m_L_dmxScene.append(t_scene);
//    emit L_dmxValueChanged();
  }
  else
    qWarning() << "cant add scene";


}

void DmxSequence::removeScene(DmxScene *t_scene)
{
  // we check if the pointer isn't null ...
  if(t_scene)
  {
    int index = m_L_dmxScene.indexOf(t_scene);
//    and if the output is in the list.
    if(index)
    {
      m_L_dmxScene.removeAt(index);
//      emit L_dmxValueChanged();
      //  m_L_dmxValue.squeeze(); // this freeis unused memory
//      return true;
    }
  }
  else
    qWarning() << "pointer is null or scene is not in the list."
               << "can't remove scene";

}

void DmxSequence::removeScene(int t_step)
{
  if ((t_step > 0)
      && (t_step < m_L_dmxScene.size()))
    m_L_dmxScene.removeAt(t_step);
  else
    qWarning() << "pointer is null or scene is not in the list."
               << "can't remove scene";

}
