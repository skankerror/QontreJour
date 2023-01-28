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

#ifndef DMXVALUESLIDER_H
#define DMXVALUESLIDER_H

#include <QSlider>
#include "../core/dmxvalue.h"
#include "../core/dmxchannelgroup.h"

class DmxValueSlider
    : public QSlider
{

  Q_OBJECT

public:

  explicit DmxValueSlider(QWidget *parent = nullptr);

  DmxValueSlider(DmxValue *t_dmxValue,
                 QWidget *parent = nullptr);

  virtual ~DmxValueSlider();

  DmxValue *getDmxValue() const{ return m_dmxValue; }
  void setDmxValue(DmxValue *t_dmxValue){ m_dmxValue = t_dmxValue; }

protected slots:

  virtual void updateLevel(int t_level);
  virtual void onValueLevelChanged(DmxValue::SignalSenderType t_type,
                                   quint8 t_level);

protected :

  DmxValue *m_dmxValue;
};

/*********************************************************************/

class SubmasterSlider
    : public DmxValueSlider
{

  Q_OBJECT

public :

  explicit SubmasterSlider(QWidget *parent = nullptr);

  explicit SubmasterSlider(DmxChannelGroup *t_dmxChannelGroup,
                           QWidget *parent = nullptr);

  ~SubmasterSlider();

  DmxChannelGroup *getchannelGroup() const{ return m_channelGroup; }

public slots:

  void setChannelGroup(DmxChannelGroup *t_channelGroup){ m_channelGroup = t_channelGroup; }

protected slots:

  void updateLevel(int t_level) override;

private :

  DmxChannelGroup *m_channelGroup;

};



#endif // DMXVALUESLIDER_H
