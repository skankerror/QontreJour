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

#include "dmxscene.h"
#include <QDebug>

/****************************** RootScene ****************************/

RootScene::RootScene(ValueType t_type,
                     DmxValue *t_parent)
  : DmxValue(t_type,
             t_parent)
{}

RootScene::~RootScene()
{
  m_L_childScene.clear();
  m_L_childScene.squeeze();
}

/****************************** DmxScene *****************************/

DmxScene::DmxScene(ValueType t_type,
                   RootScene *t_parent)
  : DmxValue(t_type,
             t_parent),
    m_sequence(t_parent)
{}

DmxScene::~DmxScene()
{}



