/*
 * (c) 2021 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
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

#include "qmidiin.h"
#include <QDebug>

QMidiIn::QMidiIn(QObject *parent,
                 QMidiApi t_api,
                 const QString &t_clientName,
                 unsigned int t_queueSizeLimit):
  QObject(parent),
  m_rtMidiIn(new RtMidiIn((RtMidi::Api)t_api,
                          t_clientName.toStdString().c_str(),
                          t_queueSizeLimit)),
  m_currentID(QMIDIIN_COUNT++),
  m_isPortOpen(false),
  m_ignoreSysex(false),
  m_ignoreTime(false),
  m_ignoreSense(false)
{
  // by default no ignored messages
  setIgnoredTypes(m_ignoreSysex, m_ignoreTime, m_ignoreSense);
}

QMidiIn::~QMidiIn()
{
//  RtMidiIn::~RtMidiIn();
  delete m_rtMidiIn;
}

void QMidiIn::callBack(double t_deltatime,
                       std::vector<unsigned char> *t_unMessage,
                       void *t_userData)
{
  auto aMidiIn = (QMidiIn*)t_userData;

  // TODO: check thread / parent object ??
  // maybe pass the vector and create qmessage in qmidiIO ?
  auto aMessage = new QMidiMessage(/*aMidiIn*/);
  aMessage->setRawMessage(*t_unMessage);
  aMessage->setDeltaTime(t_deltatime);

  emit aMidiIn->sigRecieveMessage(aMessage);

}

int QMidiIn::portCount()
{
  return m_rtMidiIn->getPortCount();
}

QStringList QMidiIn::portNames()
{
  int aPortCount = portCount();
  QStringList stringList;
  for(int i = 0; i < aPortCount; i++)
  {
    QString string = QString::fromStdString(m_rtMidiIn->getPortName(i));
    stringList.append(string);
  }
  return stringList;
}

void QMidiIn::setIgnoredTypes(bool t_ignoreSysex,
                              bool t_ignoreTime,
                              bool t_ignoreSense)
{
  m_rtMidiIn->ignoreTypes(t_ignoreSysex,
              t_ignoreTime,
              t_ignoreSense);
  m_ignoreSysex = t_ignoreSysex;
  m_ignoreTime = t_ignoreTime;
  m_ignoreSense = t_ignoreSense;
}

void QMidiIn::connectMidiIn(int t_portNumber)
{
  if (m_rtMidiIn->isPortOpen())
    disconnectMidiIn();

  m_rtMidiIn->openPort(t_portNumber);

  if (m_rtMidiIn->isPortOpen())
  {
    m_isPortOpen = true;
    m_rtMidiIn->setCallback(&callBack, this);
  }
  else
    qDebug() << "Midi in " << m_currentID << " not opened";
}

void QMidiIn::connectMidiIn(QString &t_portName)
{
  for(unsigned int i = 0; i < m_rtMidiIn->getPortCount(); i++)
  {
    if(t_portName == QString::fromStdString(m_rtMidiIn->getPortName(i)))
    {
      if (m_rtMidiIn->isPortOpen())
        disconnectMidiIn();
      m_rtMidiIn->openPort(i);
    }
    if (m_rtMidiIn->isPortOpen())
      m_isPortOpen = true;
    else
      qDebug() << "Midi in " << m_currentID << " not opened";
  }
}


void QMidiIn::connectVirtualMidiIn()
{
  m_rtMidiIn->openVirtualPort("QMidiIn virtual port");
}

void QMidiIn::disconnectMidiIn()
{
  if (m_rtMidiIn->isPortOpen())
  {
    m_isPortOpen = false;
    m_rtMidiIn->closePort();
    m_rtMidiIn->cancelCallback();
  }
  else qDebug() << "Port in was not opened...";
}


