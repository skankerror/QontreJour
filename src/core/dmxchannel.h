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

#ifndef DMXCHANNEL_H
#define DMXCHANNEL_H

#include <QObject>
#include <QList>
#include "dmxoutput.h"

class DmxChannel
    : public QObject
{

  Q_OBJECT

public:

  explicit DmxChannel(QObject *parent = nullptr);
  ~DmxChannel();

  // getters
  int getLevel() const{ return m_level; }
  QList<DmxOutput *> getL_dmxOutput() const { return m_L_dmxOutput; }
  DmxOutput* getL_dmxOutputAt(int t_index);


  //setters
  void setLevel(const int t_level);


  void addDmxOutput(DmxOutput *t_dmxOutput);
  void addDmxOutputs(const QList<DmxOutput *> t_L_dmxOutput);
  bool removeDmxOutput(const DmxOutput *t_dmxOutput);
  bool removeDmxOutputs(const QList<DmxOutput *> t_L_dmxOutput);
  void clear();

  int getOutputsCount() const { return m_L_dmxOutput.size(); }


signals:

  void L_dmxOutputChanged();
  void levelChanged(int);

private:

  QList<DmxOutput *> m_L_dmxOutput;
  int m_level;

};

#endif // DMXCHANNEL_H
