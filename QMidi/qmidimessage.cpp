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

#include "qmidimessage.h"
#include <QDebug>

QMidiMessage::QMidiMessage(QObject *parent) :
  QObject(parent),
  m_status(UNKNOWN),
  m_channel(1),
  m_pitch(0),
  m_velocity(0),
  m_control(0),
  m_value(0),
  m_progID(0),
  m_pressure(0),
  m_seqID(0),
  m_CCFristDataByte(CCFIRSTDATABYTE_ERROR),
  m_deltaTime(0)
{
  m_pitchBend.first = 0;
  m_pitchBend.second = 0;
  m_pitchBend.first = 0;
  m_pitchBend.second = 0;
  m_sysExData.clear();
  m_rawMessage.clear();
}

QMidiMessage::~QMidiMessage()
{}

QMidiMessage::QMidiMessage(QMidiMessage &t_other,
                           QObject *parent) :
  QObject(parent),
  m_status(t_other.status()),
  m_channel(t_other.channel()),
  m_pitch(t_other.pitch()),
  m_velocity(t_other.velocity()),
  m_control(t_other.control()),
  m_value(t_other.value()),
  m_progID(t_other.progID()),
  m_pitchBend(t_other.pitchBend()),
  m_pressure(t_other.pressure()),
  m_songPos(t_other.songPos()),
  m_seqID(t_other.seqID()),
  m_CCFristDataByte(t_other.CCFristDataByte()),
  m_deltaTime(t_other.deltaTime()),
  m_sysExData(t_other.sysExData()),
  m_rawMessage(t_other.rawMessage())
{}

std::vector<unsigned char> QMidiMessage::rawMessage()
{
  // we make raw messages from other args
  makeRawMessage();
  return m_rawMessage;
}

void QMidiMessage::setRawMessage(std::vector<unsigned char> &t_rawMessage)
{
  m_rawMessage = t_rawMessage;
  // we populate args
  parseRawMessage();
}

void QMidiMessage::clear()
{
  m_status = UNKNOWN;
  m_channel = 1;
  m_pitch = 0;
  m_velocity = 0;
  m_control = 0;
  m_value = 0;
  m_progID = 0;
  m_pitchBend.first = 0;
  m_pitchBend.second = 0;
  m_pressure = 0;
  m_songPos.first = 0;
  m_songPos.second = 0;
  m_seqID = 0;
  m_CCFristDataByte = CCFIRSTDATABYTE_ERROR;
  m_deltaTime = 0;
  m_sysExData.clear();
  m_rawMessage.clear();
}

void QMidiMessage::makeRawMessage()
{
  m_rawMessage.clear();
  switch(m_status)
  {
  case NOTE_ON :
    m_rawMessage.push_back(NOTE_ON + m_channel - 1);
    m_rawMessage.push_back(m_pitch);
    m_rawMessage.push_back(m_velocity);
    break;
  case NOTE_OFF :
    m_rawMessage.push_back(NOTE_OFF + m_channel - 1);
    m_rawMessage.push_back(m_pitch);
    m_rawMessage.push_back(0);
    break;
  case PROG_CHANGE :
    m_rawMessage.push_back(PROG_CHANGE + m_channel - 1);
    m_rawMessage.push_back(m_control);
    m_rawMessage.push_back(m_value);
    break;
  case PITCH_BEND :
    m_rawMessage.push_back(PITCH_BEND + m_channel - 1);
    m_rawMessage.push_back(m_pitchBend.first);
    m_rawMessage.push_back(m_pitchBend.second);
    break;
  case AFTERTOUCH :
    m_rawMessage.push_back(AFTERTOUCH + m_channel - 1);
    m_rawMessage.push_back(m_pressure);
    break;
  case POLY_AFTERTOUCH :
    m_rawMessage.push_back(POLY_AFTERTOUCH + m_channel - 1);
    m_rawMessage.push_back(m_pitch);
    m_rawMessage.push_back(m_pressure);
    break;
  case CONTROL_CHANGE :
  {
    m_rawMessage.push_back(CONTROL_CHANGE + m_channel - 1);
//    if (m_channel > 120) // Channel Mode Message
//    {
//      m_chModStatus = static_cast<ChannelModeStatus>(m_channel);
//      m_rawMessage.push_back(m_chModStatus);
//      if (m_chModStatus == MONO_ON)
//        m_rawMessage.push_back(m_channelNumber);
//      else
//        m_rawMessage.push_back(m_value);
//    }
//    else // it's real control change message
//    {
//      m_rawMessage.push_back(m_control);
//      m_rawMessage.push_back(m_value);
//    }
    break;
  }
  case MIDI_CLOCK :
    m_rawMessage.push_back(MIDI_CLOCK);
    break;
  case START :
    m_rawMessage.push_back(START);
    break;
  case STOP :
    m_rawMessage.push_back(STOP);
    break;
  case CONTINUE :
    m_rawMessage.push_back(CONTINUE);
    break;
  case ACTIVE_SENSING :
    m_rawMessage.push_back(ACTIVE_SENSING);
    break;
  case SYSTEM_RESET :
    m_rawMessage.push_back(SYSTEM_RESET);
    break;
  case SONG_POS_PTR :
    m_rawMessage.push_back(SONG_POS_PTR);
    m_rawMessage.push_back(m_songPos.first);
    m_rawMessage.push_back(m_songPos.second);
    break;
  case SONG_SELECT :
    m_rawMessage.push_back(SONG_SELECT);
    m_rawMessage.push_back(m_seqID);
    break;
  case TUNE_REQUEST :
    m_rawMessage.push_back(TUNE_REQUEST);
    break;
  case SYSEX : // TODO: develop sysex ?
    m_rawMessage = m_sysExData;
    if(m_sysExData.back() != EOX)
      m_rawMessage.push_back(EOX);
    break;
  default :
    break;
  }
  qDebug() << m_rawMessage;
}

void QMidiMessage::parseRawMessage()
{
  int rawMessageSize = m_rawMessage.size();


  if (rawMessageSize == 0)
  {
    qDebug() << " empty raw message, impossible to parse. ";
    return;
  }

  int statusByte = m_rawMessage.at(0);
  // first we need to determinate if we have a channel
  if (statusByte > CHANNEL_VOICE_INFORMATION_LIMIT) // we have no channel
    m_status = static_cast<MidiStatus>(statusByte);

  else // we do have channel information
  {
    // we isolate first 4 bits by pushing others to 0
    unsigned char statusMask = STATUS_MASK; // 11110000 mask
    m_status = static_cast<MidiStatus>(m_rawMessage.at(0) & statusMask);

    // we get last 4 bits for channel
    unsigned char channelMask = CHANNEL_MASK; // 00001111
    m_channel = (m_rawMessage.at(0) & channelMask) + 1;
  }

  switch(m_status)
  {
  case NOTE_ON :
    m_pitch = m_rawMessage.at(1);
    m_velocity = m_rawMessage.at(2);
    break;
  case NOTE_OFF :
    m_pitch = m_rawMessage.at(1);
    m_velocity = m_rawMessage.at(2); // = 0 ?
    break;
  case PROG_CHANGE :
    m_progID = m_rawMessage.at(1);
    break;
  case PITCH_BEND :
    m_pitchBend.first = m_rawMessage.at(1);
    m_pitchBend.second = m_rawMessage.at(2);
    break;
  case AFTERTOUCH :
    m_pressure = m_rawMessage.at(1);
    break;
  case POLY_AFTERTOUCH :
    m_pitch = m_rawMessage.at(1);
    m_pressure = m_rawMessage.at(2);
    break;
  case CONTROL_CHANGE :
    m_control = m_rawMessage.at(1);
//    if (m_control > CONTROL_CHANGE_CONTROLER_ID_LIMIT) // channel mode message
//    {
//      m_chModStatus = static_cast<ChannelModeStatus>(m_channel);
//      if (m_chModStatus == MONO_ON)
//        m_channelNumber = m_rawMessage.at(2);
//    }
//    m_value = m_rawMessage.at(2);
    break;
  case SONG_POS_PTR :
    m_songPos.first = m_rawMessage.at(1);
    m_songPos.second = m_rawMessage.at(2);
    break;
  case SONG_SELECT :
    m_seqID = m_rawMessage.at(1);
    break;
  case SYSEX :
    m_sysExData = m_rawMessage;
    break;
  // nothing to do for these cases
  case MIDI_CLOCK :
  case START :
  case STOP :
  case CONTINUE :
  case ACTIVE_SENSING :
  case SYSTEM_RESET :
  case TUNE_REQUEST :
  default :
    break;
  }

}

