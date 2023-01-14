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

#ifndef QMIDIMESSAGE_H
#define QMIDIMESSAGE_H

#include <QObject>
#include <vector>
#include "qmidi.h"


/*! \brief MIDI message class
 *
 *  Class to handle all MIDI events.
*/
class QMidiMessage :
    public QObject
{

  Q_OBJECT

public:

  /*! \brief QMidiMessage constructor.
   *  \param parent a QObject parent
   */
  explicit QMidiMessage(QObject *parent = nullptr);

  /*! \brief QMidiMessage destructor */
  ~QMidiMessage();

  /*! \brief QMidiMessage copy constructor
   *  \param t_other object to copy
   *  \param parent QObject parent
   */
  QMidiMessage(QMidiMessage &t_other,
               QObject *parent = nullptr);

  // getters
  MidiStatus status() const{ return m_status; }
  unsigned int channel() const{ return m_channel; }
  unsigned int pitch() const{ return m_pitch; }
  unsigned int velocity() const{ return m_velocity; }
  unsigned int control() const{ return m_control; }
  unsigned int value() const{ return m_value; }
  unsigned int progID() const{ return m_progID; }
  QPair<unsigned int, unsigned int> pitchBend() const{ return m_pitchBend; }
  unsigned int pressure() const{ return m_pressure; }
  QPair<unsigned int, unsigned int> songPos() const{ return m_songPos; }
  unsigned int seqID() const{ return m_seqID; }
  CCFirstDataByte CCFristDataByte() const { return m_CCFristDataByte; }
  unsigned int channelNumber() const{ return m_channelNumber; }
  double deltaTime() const{ return m_deltaTime; }
  std::vector<unsigned char> sysExData() const{ return m_sysExData; }
  std::vector<unsigned char> rawMessage();

  // setters
  void setStatus(const MidiStatus t_status){ m_status = t_status; }
  void setChannel(const unsigned int t_channel){ m_channel = t_channel; }
  void setPitch(const unsigned int t_pitch){ m_pitch = t_pitch; }
  void setVelocity(const unsigned int t_velocity){ m_velocity = t_velocity; }
  void setControl(const unsigned int t_control){ m_control = t_control; }
  void setValue(const unsigned int t_value){ m_value = t_value; }
  void setProgID(const unsigned int t_progID){ m_progID = t_progID; }
  void setPitchBend(const QPair<unsigned int, unsigned int> t_pitchBend){ m_pitchBend = t_pitchBend; }
  void setPressure(const unsigned int t_pressure){ m_pressure = t_pressure; }
  void setSongPos(const QPair<unsigned int, unsigned int> t_songPos){ m_songPos = t_songPos; }
  void setSeqID(const unsigned int t_seqID){ m_seqID = t_seqID; }
  void setCCFristDataByte(CCFirstDataByte t_CCFristDataByte){ m_CCFristDataByte = t_CCFristDataByte; }
  void setChannelNumber(unsigned int t_channelNumber){ m_channelNumber = t_channelNumber; }
  void setDeltaTime(const double t_deltaTime){ m_deltaTime = t_deltaTime; }
  void setSysExData(std::vector<unsigned char> &t_sysExData){ m_sysExData = t_sysExData; }
  void setRawMessage(std::vector<unsigned char> &t_rawMessage);

private:

  void clear();

  // method to set raw message from args
  void makeRawMessage();

  // method to set args from raw message
  void parseRawMessage();


private:

  MidiStatus m_status;
  unsigned int m_channel;
  unsigned int m_pitch;
  unsigned int m_velocity;
  unsigned int m_control;
  unsigned int m_value;
  unsigned int m_progID;
  QPair<unsigned int, unsigned int> m_pitchBend;
  unsigned int m_pressure;
  QPair<unsigned int, unsigned int> m_songPos;
  unsigned int m_seqID;

  CCFirstDataByte m_CCFristDataByte;
  unsigned int m_channelNumber;

  double m_deltaTime;
  std::vector<unsigned char> m_sysExData;
  std::vector<unsigned char> m_rawMessage;

};
#endif // QMIDIMESSAGE_H
