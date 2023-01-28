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

#include "dmxvalueslider.h"

DmxValueSlider::DmxValueSlider(QWidget *parent)
  :QSlider(parent)
{
  setMinimum(0);
  setMaximum(255);
}

DmxValueSlider::DmxValueSlider(DmxValue *t_dmxValue,
                               QWidget *parent)
  : QSlider(parent),
    m_dmxValue(t_dmxValue)
{
  setMinimum(0);
  setMaximum(255);

  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));

  connect(m_dmxValue,
          SIGNAL(levelChanged(DmxValue::SignalSenderType,quint8)),
          this,
          SLOT(onValueLevelChanged(DmxValue::SignalSenderType,quint8)));

}

DmxValueSlider::~DmxValueSlider()
{}

void DmxValueSlider::unMoveSlider(quint8 t_level)
{
  // we disconnect to avoid connecting loop
  disconnect(this,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(updateLevel(int)));

  this->setValue(t_level);

  // we reconnect
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));
}

void DmxValueSlider::updateLevel(int t_level)
{
  if (t_level < 0) t_level = 0;
  if (t_level > 255) t_level = 255;
  if (m_dmxValue->getLevel() == t_level) return;
  m_dmxValue->setLevel(DmxValue::DirectChannelEditSender,
                       t_level);
}

void DmxValueSlider::onValueLevelChanged(DmxValue::SignalSenderType t_type,
                                         quint8 t_level)
{
  Q_UNUSED(t_type)

  // we disconnect to avoid connecting loop
  disconnect(this,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(updateLevel(int)));

  this->setValue(t_level);

  // we reconnect
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));


}

/**************************************************************/

SubmasterSlider::SubmasterSlider(QWidget *parent)
  : DmxValueSlider(parent)
{}

SubmasterSlider::SubmasterSlider(DmxChannelGroup *t_dmxChannelGroup,
                                 QWidget *parent)
  : DmxValueSlider(parent),
    m_channelGroup(t_dmxChannelGroup)
{
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));

  connect(m_channelGroup,
          SIGNAL(levelChanged(DmxValue::SignalSenderType,quint8)),
          this,
          SLOT(onValueLevelChanged(DmxValue::SignalSenderType,quint8)));

}

SubmasterSlider::~SubmasterSlider()
{}

void SubmasterSlider::updateLevel(int t_level)
{
  if (t_level < 0) t_level = 0;
  if (t_level > 255) t_level = 255;
  if (m_channelGroup->getLevel() == t_level) return;
  m_channelGroup->setLevel(DmxValue::SubmasterSliderSender,
                           t_level);

}
