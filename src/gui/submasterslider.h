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

#ifndef SUBMASTERSLIDER_H
#define SUBMASTERSLIDER_H

#include <QSlider>
#include "../core/dmxchannelgroup.h"

class SubMasterSlider
    : public QSlider
{

  Q_OBJECT

public:

  SubMasterSlider(QWidget *parent = nullptr);
  SubMasterSlider(DmxChannelGroup *t_dmxChannelGroup,
                  QWidget *parent = nullptr);


  DmxChannelGroup *getDmxChannelGroup() const { return m_dmxChannelGroup; }
  void setDmxChannelGroup(DmxChannelGroup *t_dmxChannelGroup) { m_dmxChannelGroup = t_dmxChannelGroup; }

private:

  DmxChannelGroup* m_dmxChannelGroup;
};

#endif // SUBMASTERSLIDER_H
