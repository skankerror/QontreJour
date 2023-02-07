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

#ifndef DMXSCENE_H
#define DMXSCENE_H

#include "dmxvalue.h"


class DmxScene :
    public DmxValue
{

  Q_OBJECT

public:

  explicit DmxScene(DmxValue::ValueType t_type /*= Scene*/,
                    DmxScene *t_parent = nullptr);

  virtual ~DmxScene();

  // getters
  QString getNotes() const{ return m_notes; }
  float getTimeIn() const{ return m_timeIn; }
  float getTimeOut() const{ return m_timeOut; }
  float getDelayIn() const{ return m_delayIn; }
  float getDelayOut() const{ return m_delayOut; }
  int getStepNumber() const;

  // setters
  void setNotes(const QString &t_notes){ m_notes = t_notes; }
  void setTimeIn(float t_timeIn){ m_timeIn = t_timeIn; }
  void setTimeOut(float t_timeOut){ m_timeOut = t_timeOut; }
  void setDelayIn(float t_delayIn){ m_delayIn = t_delayIn; }
  void setDelayOut(float t_delayOut){ m_delayOut = t_delayOut; }

public slots :

  void setStepNumber(int t_stepNumber){ m_stepNumber = t_stepNumber; }

signals:

protected :

  QString m_notes;
  float m_timeIn;
  float m_timeOut;
  float m_delayIn;
  float m_delayOut;
  int m_stepNumber;

};

#endif // DMXSCENE_H
