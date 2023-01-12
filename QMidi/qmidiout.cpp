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

#include "qmidiout.h"
#include <QDebug>

QMidiOut::QMidiOut(QObject *parent,
                   QMidiApi t_api,
                   const QString &t_clientName) :
  QObject(parent),
  m_rtMidiOut(new RtMidiOut((RtMidi::Api)t_api,
                            t_clientName.toStdString().c_str())),
  m_currentID(QMIDIOUT_COUNT++)
{}

QMidiOut::~QMidiOut()
{
  delete m_rtMidiOut;
}

int QMidiOut::portCount()
{
  return m_rtMidiOut->getPortCount();
}

QStringList QMidiOut::portNames()
{
  int aPortCount = portCount();
  QStringList stringList;
  for(int i = 0; i < aPortCount; i++)
  {
    QString string = QString::fromStdString(m_rtMidiOut->getPortName(i));
    stringList.append(string);
  }
  return stringList;
}

void QMidiOut::sendQMidiMessage(QMidiMessage *t_message)
{
  auto rawMessage = t_message->rawMessage();
  sendRawMessage(rawMessage);
}

void QMidiOut::sendRawMessage(std::vector<unsigned char> t_rawMessage)
{
  m_rtMidiOut->sendMessage(&t_rawMessage);
}

void QMidiOut::sendNoteOn(unsigned int t_channel,
                          unsigned int t_pitch,
                          unsigned int t_velocity)
{
  // must check before
  auto message = new QMidiMessage(this);
  message->setStatus(NOTE_ON);
  message->setChannel(t_channel);
  message->setPitch(t_pitch);
  message->setVelocity(t_velocity);
  sendQMidiMessage(message);
}

void QMidiOut::sendNoteOff(unsigned int t_channel,
                           unsigned int t_pitch,
                           unsigned int t_velocity)
{
  // must check before
  auto message = new QMidiMessage(this);
  message->setStatus(NOTE_OFF);
  message->setChannel(t_channel);
  message->setPitch(t_pitch);
  message->setVelocity(t_velocity);
  sendQMidiMessage(message);
}

void QMidiOut::sendProgChange(unsigned int t_channel,
                              unsigned int t_progID)
{
  auto message = new QMidiMessage(this);
  message->setStatus(PROG_CHANGE);
  message->setChannel(t_channel);
  message->setProgID(t_progID);
  sendQMidiMessage(message);
}

void QMidiOut::sendPitchBend(unsigned int t_channel,
                             QPair<unsigned int, unsigned int> t_pitchBend)
{
  auto message = new QMidiMessage(this);
  message->setStatus(PITCH_BEND);
  message->setChannel(t_channel);
  message->setPitchBend(t_pitchBend);
  sendQMidiMessage(message);
}

void QMidiOut::sendAftertouch(unsigned int t_channel,
                              unsigned int t_pressure)
{
  auto message = new QMidiMessage(this);
  message->setStatus(AFTERTOUCH);
  message->setChannel(t_channel);
  message->setPressure(t_pressure);
  sendQMidiMessage(message);
}

void QMidiOut::sendPolyAftertouch(unsigned int t_channel,
                                  unsigned int t_pitch,
                                  unsigned int t_pressure)
{
  auto message = new QMidiMessage(this);
  message->setStatus(POLY_AFTERTOUCH);
  message->setChannel(t_channel);
  message->setPitch(t_pitch);
  message->setPressure(t_pressure);
  sendQMidiMessage(message);
}

void QMidiOut::sendControlChange(unsigned int t_channel,
                                 unsigned int t_control,
                                 unsigned int t_value)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
  message->setControl(t_control);
  message->setValue(t_value);
  sendQMidiMessage(message);
}

void QMidiOut::sendChannelModeReset(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(RESET);
  message->setCCFristDataByte(RESET);
  message->setValue(0);
  sendQMidiMessage(message);
}

void QMidiOut::sendLocalControlOn(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(LOCAL_CONTROL);
  message->setCCFristDataByte(LOCAL_CONTROL);
  message->setValue(LOCAL_CONTROL_ON);
  sendQMidiMessage(message);

}

void QMidiOut::sendLocalControlOff(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(LOCAL_CONTROL);
  message->setCCFristDataByte(LOCAL_CONTROL);
  message->setValue(LOCAL_CONTROL_OFF);
  sendQMidiMessage(message);

}

void QMidiOut::sendAllNotesOff(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(ALL_NOTES_OFF);
  message->setCCFristDataByte(ALL_NOTES_OFF);
  message->setValue(0);
  sendQMidiMessage(message);

}

void QMidiOut::sendOmniOff(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(OMNI_OFF);
  message->setCCFristDataByte(OMNI_OFF);
  message->setValue(0);
  sendQMidiMessage(message);

}

void QMidiOut::sendOmniOn(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(OMNI_ON);
  message->setCCFristDataByte(OMNI_ON);
  message->setValue(0);
  sendQMidiMessage(message);

}

void QMidiOut::sendMonoOn(unsigned int t_channel,
                          unsigned int t_channelNumber)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(MONO_ON);
  message->setCCFristDataByte(MONO_ON);
  message->setChannelNumber(t_channelNumber);
  sendQMidiMessage(message);

}

void QMidiOut::sendPolyOn(unsigned int t_channel)
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTROL_CHANGE);
  message->setChannel(t_channel);
//  message->setChModStatus(POLY_ON);
  message->setCCFristDataByte(POLY_ON);
  message->setValue(0);
  sendQMidiMessage(message);

}

void QMidiOut::sendMidiClock()
{
  auto message = new QMidiMessage(this);
  message->setStatus(MIDI_CLOCK);
  sendQMidiMessage(message);
}

void QMidiOut::sendStart()
{
  auto message = new QMidiMessage(this);
  message->setStatus(START);
  sendQMidiMessage(message);
}

void QMidiOut::sendStop()
{
  auto message = new QMidiMessage(this);
  message->setStatus(STOP);
  sendQMidiMessage(message);
}

void QMidiOut::sendContinue()
{
  auto message = new QMidiMessage(this);
  message->setStatus(CONTINUE);
  sendQMidiMessage(message);
}

void QMidiOut::sendActiveSensing()
{
  auto message = new QMidiMessage(this);
  message->setStatus(ACTIVE_SENSING);
  sendQMidiMessage(message);
}

void QMidiOut::sendSystemReset()
{
  auto message = new QMidiMessage(this);
  message->setStatus(SYSTEM_RESET);
  sendQMidiMessage(message);
}

void QMidiOut::sendSongPosPointer(QPair<unsigned int, unsigned int> t_songPos)
{
  auto message = new QMidiMessage(this);
  message->setStatus(SONG_POS_PTR);
  message->setSongPos(t_songPos);
  sendQMidiMessage(message);
}

void QMidiOut::sendSongSelect(unsigned int t_seqID)
{
  auto message = new QMidiMessage(this);
  message->setStatus(SONG_SELECT);
  message->setSeqID(t_seqID);
  sendQMidiMessage(message);
}

void QMidiOut::sendTuneRequest()
{
  auto message = new QMidiMessage(this);
  message->setStatus(TUNE_REQUEST);
  sendQMidiMessage(message);
}

void QMidiOut::sendSysEx(std::vector<unsigned char> *t_sysExData)
{
  auto message = new QMidiMessage(this);
  message->setStatus(SYSEX);
  message->setSysExData(*t_sysExData);
  sendQMidiMessage(message);
}


void QMidiOut::connectMidiOut(int t_portNumber)
{
  if (m_rtMidiOut->isPortOpen())
    disconnectMidiOut();

  m_rtMidiOut->openPort(t_portNumber);

  if (m_rtMidiOut->isPortOpen())
    m_isPortOpen = true;
  else
    qDebug() << "Midi out " << m_currentID << " not opened";

}

void QMidiOut::connectMidiOut(QString &t_portName)
{
  for(unsigned int i = 0; i < m_rtMidiOut->getPortCount(); i++)
  {
    if(t_portName == QString::fromStdString(m_rtMidiOut->getPortName(i)))
    {
      if (m_rtMidiOut->isPortOpen())
        disconnectMidiOut();
      m_rtMidiOut->openPort(i);
    }
    if (m_rtMidiOut->isPortOpen())
      m_isPortOpen = true;
    else
      qDebug() << "Midi out " << m_currentID << " not opened";
  }
}

void QMidiOut::connectVirtualMidiOut()
{
  m_rtMidiOut->openVirtualPort("QMidiOut virtual port");
}

void QMidiOut::disconnectMidiOut()
{
  if (m_rtMidiOut->isPortOpen())
  {
    m_isPortOpen = false;
    m_rtMidiOut->closePort();
  }
  else qDebug() << "Port out was not opened...";


}
