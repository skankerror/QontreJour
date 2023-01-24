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

#ifndef QMIDI_H
#define QMIDI_H

/*! \file qmidi.h
 *  \brief enum, define, global var file
 *
 *  In this file stand all the QMidi enum, define and global variables
 */

/*!
 *  \brief a simple wrapper for RtMidi::Api
 */
enum QMidiApi
{
  Q_UNSPECIFIED,    /*!< Search for a working compiled API. */
  Q_MACOSX_CORE,    /*!< Macintosh OS-X CoreMIDI API. */
  Q_LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
  Q_UNIX_JACK,      /*!< The JACK Low-Latency MIDI Server API. */
  Q_WINDOWS_MM,     /*!< The Microsoft Multimedia MIDI API. */
  Q_RTMIDI_DUMMY,   /*!< A compilable but non-functional API. */
  Q_NUM_APIS        /*!< Number of values in this enum. */
};

/*!
 *  \brief exhaustive midi status enum
 *
 *  This enum holds all midi status
 */
enum MidiStatus
{
  UNKNOWN = 0x00,

  // channel voice messages
  NOTE_ON = 0x90, /*!< plus 2 data bytes, pitch and velocity */
  NOTE_OFF = 0x80, /*!< plus 2 data bytes, pitch and velocity ( = 0) */
  PROG_CHANGE = 0xC0, /*!< plus 1 data byte, program ID */
  PITCH_BEND = 0xE0, /*!< plus 2 data bytes (MSB, LSB) */
  AFTERTOUCH = 0xD0, /*!< 1 data byte, pressure */
  POLY_AFTERTOUCH = 0xA0, /*!< 2 data bytes, pitch, pressure */
  CONTROL_CHANGE = 0xB0, /*!< this handles channel mode message too */

  // system real time messages
  MIDI_CLOCK = 0xF8, /*!< system real time message for sequencer */
  START = 0xFA, /*!< system real time message for sequencer */
  STOP = 0xFC, /*!< system real time message for sequencer */
  CONTINUE = 0xFB, /*!< system real time message for sequencer */
  ACTIVE_SENSING = 0xFE, /*!< system real time message for sequencer */
  SYSTEM_RESET = 0xFF, /*!< system real time message for sequencer */

  // system common messages (for slaved seq)
  MIDI_TIMING_CODE = 0xF1, /*!< system common message for slaved sequencer */
  SONG_POS_PTR = 0xF2, /*!< system common message for slaved sequencer plus 2 data bytes */
  SONG_SELECT = 0xF3, /*!< system common message for slaved sequencer plus 1 data byte */
  TUNE_REQUEST = 0xF6, /*!< system common message for slaved sequencer */

  // system exclusive messages
  SYSEX = 0xF0, /*!< system exclusive message status */
  EOX = 0xF7 /*!< end of sysex */
};

/*!
 * \brief The Control Channel First Data Byte
 *
 * this is default profile according to MIDI 1.0 and 2.0
 * specification. see http://midi.org for more information.
 */
enum CCFirstDataByte
{
  BANK_SELECT = 0x00, /*!< value 0-127, used as MSB, reset : do not set */
  MODULATION_WHEEL = 0x01, /*!< value 0-127, used as MSB, reset : 0 */
  BREATH_CONTROLER = 0x02, /*!< value 0-127, used as MSB, reset : 0 */
  UNDEFINED_3 = 0x03, /*!< value 0-127, used as MSB, reset : Device specific */
  FOOT_CONTROLLER = 0x04, /*!< value 0-127, used as MSB, reset : 0 */
  PORTAMENTO_TIME = 0x05, /*!< value 0-127, used as MSB, reset : 0 */
  DATA_ENTRY = 0x06, /*!< value 0-127, used as MSB, reset : do not set */
  CHANNEL_VOLUME = 0x07, /*!< value 0-127, used as MSB, reset : do not set */
  BALANCE = 0x08, /*!< value 0-127, used as MSB, reset : 64 */
  UNDEFINED_9 = 0x09, /*!< value 0-127, used as MSB, reset : Device specific */
  PAN = 0x0A, /*!< value 0-127, used as MSB, reset : do not set */
  EXPRESSION_CONTROLLER = 0x0B, /*!< value 0-127, used as MSB, reset : 127 */
  EFFECT_CONTROL_1 = 0x0C, /*!< value 0-127, used as MSB, reset : 0 */
  EFFECT_CONTROL_2 = 0x0D, /*!< value 0-127, used as MSB, reset : 0 */
  UNDEFINED_14 = 0x0E, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_15 = 0x0F, /*!< value 0-127, used as MSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_1 = 0x10, /*!< value 0-127, used as MSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_2 = 0x11, /*!< value 0-127, used as MSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_3 = 0x12, /*!< value 0-127, used as MSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_4 = 0x13, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_20 = 0x14, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_21 = 0x15, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_22 = 0x16, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_23 = 0x17, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_24 = 0x18, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_25 = 0x19, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_26 = 0x1A, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_27 = 0x1B, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_28 = 0x1C, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_29 = 0x1D, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_30 = 0x1E, /*!< value 0-127, used as MSB, reset : Device specific */
  UNDEFINED_31 = 0x1F, /*!< value 0-127, used as MSB, reset : Device specific */
  LSB_FOR_BANK_SELECT = 0x20, /*!< value 0-127, used as LSB, reset : do not set */
  LSB_FOR_MODULATION_WHEEL = 0x21, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_BREATH_CONTROLLER = 0x22, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_UNDEFINED_3 = 0x23, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_FOOT_CONTROLLER = 0x24, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_PORTAMENTO_TIME = 0x25, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_DATA_ENTRY = 0x26, /*!< value 0-127, used as LSB, reset : do not set */
  LSB_FOR_CHANNEL_VOLUME = 0x27, /*!< value 0-127, used as LSB, reset : do not set */
  LSB_FOR_BALANCE = 0x28, /*!< value 0-127, used as LSB, reset : 64 */
  LSB_FOR_UNDEFINED_9 = 0x29, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_PAN = 0x2A, /*!< value 0-127, used as LSB, reset : do not set */
  LSB_FOR_EXPRESSION_CONTROLLER = 0x2B, /*!< value 0-127, used as LSB, reset : 127 */
  LSB_FOR_EFFECT_CONTROL_1 = 0x2C, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_EFFECT_CONTROL_2 = 0x2D, /*!< value 0-127, used as LSB, reset : 0 */
  LSB_FOR_UNDEFINED_14 = 0x2E, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_15 = 0x2F, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_GENERAL_PURPOSE_CONTROLLER_1 = 0x30, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_GENERAL_PURPOSE_CONTROLLER_2 = 0x31, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_GENERAL_PURPOSE_CONTROLLER_3 = 0x32, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_GENERAL_PURPOSE_CONTROLLER_4 = 0x33, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_20 = 0x34, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_21 = 0x35, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_22 = 0x36, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_23 = 0x37, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_24 = 0x38, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_25 = 0x39, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_26 = 0x3A, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_27 = 0x3B, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_28 = 0x3C, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_29 = 0x3D, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_30 = 0x3E, /*!< value 0-127, used as LSB, reset : Device specific */
  LSB_FOR_UNDEFINED_31 = 0x3F, /*!< value 0-127, used as LSB, reset : Device specific */
  DAMPER_PEDAL = 0x40, /*!< value <64 -> OFF, >63 -> ON, used as LSB, reset : Device specific */
  PORTAMENTO = 0x41, /*!< <64 -> OFF, >63 -> ON, used as LSB, reset : Device specific */
  SOSTENUTO = 0x42, /*!< <64 -> OFF, >63 -> ON, used as LSB, reset : Device specific */
  SOFT_PEDAL = 0x43, /*!< <64 -> OFF, >63 -> ON, used as LSB, reset : Device specific */
  LEGATO_FOOTSWITCH = 0x44, /*!< <64 -> NORMAL, >63 -> LEGATO, used as LSB, reset : Device specific */
  HOLD_2 = 0x45, /*!< <64 -> OFF, >63 -> ON, used as LSB, reset : Device specific */
  SOUND_CONTROLLER_1 = 0x46, /*!< default : Sound Variation, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_2 = 0x47, /*!< default : Timbre/Harmonic Intens., value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_3 = 0x48, /*!< default : Release Time, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_4 = 0x49, /*!< default : Attack Time, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_5 = 0x4A, /*!< default : Brightness, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_6 = 0x4B, /*!< default : Decay Time, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_7 = 0x4C, /*!< default : Vibrato Rate, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_8 = 0x4D, /*!< default : Vibrato Depth, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_9 = 0x4E, /*!< default : Vibrato Delay, value 0-127, used as LSB, reset : do not set */
  SOUND_CONTROLLER_10 = 0x4F, /*!< default : undefined, value 0-127, used as LSB, reset : do not set */
  GENERAL_PURPOSE_CONTROLLER_5 = 0x50, /*!< value 0-127, used as LSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_6 = 0x51, /*!< value 0-127, used as LSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_7 = 0x52, /*!< value 0-127, used as LSB, reset : Device specific */
  GENERAL_PURPOSE_CONTROLLER_8 = 0x53, /*!< value 0-127, used as LSB, reset : Device specific */
  PORTAMENTO_CONTROL = 0x54, /*!< value 0-127, used as LSB, reset : Device specific */
  UNDEFINED_85 = 0x55, /*!< reset : Device specific */
  UNDEFINED_86 = 0x56, /*!< reset : Device specific */
  UNDEFINED_87 = 0x57, /*!< reset : Device specific */
  HIGH_RESOLUTION_VELOCITY_PREFIX = 0x58, /*!< value 0-127, used as LSB, reset : do not set */
  UNDEFINED_89 = 0x59, /*!< reset : Device specific */
  UNDEFINED_90 = 0x5A, /*!< reset : Device specific */
  EFFECTS_1_DEPTH = 0x5B, /*!< default: Reverb Send Level, value 0-127, reset : do not set */
  EFFECTS_2_DEPTH = 0x5C, /*!< value : 0-127, reset : do not set */
  EFFECTS_3_DEPTH = 0x5D, /*!< default: Chorus Send Level, value : 0-127, reset : do not set */
  EFFECTS_4_DEPTH = 0x5E, /*!< value : 0-127, reset : do not set */
  EFFECTS_5_DEPTH = 0x5F, /*!< value : 0-127, reset : do not set */
  DATA_INCREMENT = 0x60, /*!< reset : do not set */
  DATA_DECREMENT = 0x61, /*!< reset : do not set */
  NRPN_LSB = 0x62, /*!< value 0-127, used as LSB, reset : 127 */
  NRPN_MSB = 0x63, /*!< value 0-127, used as MSB, reset : 127 */
  RPN_LSB = 0x64, /*!< value 0-127, used as LSB, reset : 127 */
  RPN_MSB = 0X65, /*!< value 0-127, used as MSB, reset : 127 */
  UNDEFINED_102 = 0x66, /*!< reset : Device specific */
  UNDEFINED_103 = 0x67, /*!< reset : Device specific */
  UNDEFINED_104 = 0x68, /*!< reset : Device specific */
  UNDEFINED_105 = 0x69, /*!< reset : Device specific */
  UNDEFINED_106 = 0x6A, /*!< reset : Device specific */
  UNDEFINED_107 = 0x6B, /*!< reset : Device specific */
  UNDEFINED_108 = 0x6C, /*!< reset : Device specific */
  UNDEFINED_109 = 0x6D, /*!< reset : Device specific */
  UNDEFINED_110 = 0x6E, /*!< reset : Device specific */
  UNDEFINED_111 = 0x6F, /*!< reset : Device specific */
  UNDEFINED_112 = 0x70, /*!< reset : Device specific */
  UNDEFINED_113 = 0x71, /*!< reset : Device specific */
  UNDEFINED_114 = 0x72, /*!< reset : Device specific */
  UNDEFINED_115 = 0x73, /*!< reset : Device specific */
  UNDEFINED_116 = 0x74, /*!< reset : Device specific */
  UNDEFINED_117 = 0x75, /*!< reset : Device specific */
  UNDEFINED_118 = 0x76, /*!< reset : Device specific */
  UNDEFINED_119 = 0x77, /*!< reset : Device specific */
  ALL_SOUND_OFF = 0x78, /*!< value : 0, reset : do not set */
  RESET = 0x79, /*!< value : 0, reset : do not set */
  LOCAL_CONTROL = 0x7A, /*!< value : 0 off - 127 on, reset : do not set */
  ALL_NOTES_OFF = 0x7B, /*!< value : 0, reset : do not set */
  OMNI_OFF = 0x7C, /*!< value : 0, reset : do not set */
  OMNI_ON = 0x7D, /*!< value : 0, reset : do not set */
  MONO_ON = 0x7E,  /*!<  This equals the number of channels,
                    * or zero if the number of channels equals
                    * the number of voices in the receiver */
  POLY_ON = 0x7F, /*!< 2nd byte means nothing (set to 0) */
  CCFIRSTDATABYTE_ERROR = 0x80 /*!< to handle null instantiation in QMidiMessage */
};

/*! \def LOCAL_CONTROL_ON
 *  \brief 2nd byte of data for local control channel mode status
 */
#define LOCAL_CONTROL_ON  127

/*! \def LOCAL_CONTROL_OFF
 *  \brief 2nd byte of data for local control channel mode status
 */
#define LOCAL_CONTROL_OFF  0

/*!
 *  \brief Manufactor ID for sysex messages
 */
enum ManuID // for sysex
{
  SEQUENTIAL_CIRCUITS = 0x01,
  BIG_BRIAR = 0x02,
  OCTAVE_PLATEAU = 0x03,
  MOOG = 0x04,
  PASSPORT_DESIGNS = 0x05,
  LEXICON = 0x06,
  KURZWEIL = 0x07,
  FENDER = 0x08,
  GULBRANSEN = 0x09,
  DELTA_LABS = 0x0A,
  SOUND_COMP = 0x0B,
  GENERAL_ELECTRO = 0x0C,
  TECHMAR = 0x0D,
  MATTHEWS_RESEARCH = 0x0E,
  OBERHEIM = 0x10,
  PAIA = 0x11,
  SIMMONS = 0x12,
  DIGIDESIGN = 0x13,
  FAIRLIGHT = 0x14,
  PEAVEY = 0x1B,
  JL_COOPER = 0x15,
  LOWERY = 0x16,
  LIN = 0x17,
  EMU = 0x18,
  BONTEMPI = 0x20,
  SIEL = 0x21,
  SYNTHEAXE = 0x23,
  HOHNER = 0x24,
  CRUMAR = 0x25,
  SOLTON = 0x26,
  JELLINGHAUS_MS = 0x27,
  CTS = 0x28,
  PPG = 0x29,
  ELKA = 0x2F,
  CHEETAH = 0x36,
  KAWAI = 0x40,
  ROLAND = 0x41,
  KORG = 0x42,
  YAMAHA = 0x43,
  CASIO = 0x44,
  AKAI = 0x45
};


/*! \cond */
#define QUEUE_SIZE_LIMIT 1024
#define MIDI_IN_CLIENT_DEFAULT_NAME "QMidiIn Client"
#define MIDI_OUT_CLIENT_DEFAULT_NAME "QMidiOut Client"

#define DEFAULT_VELOCITY 64
// higher status byte value with channel info
#define CHANNEL_VOICE_INFORMATION_LIMIT 239
// higher control change controler id
#define CONTROL_CHANGE_CONTROLER_ID_LIMIT 120
// masks to deal with channel in byte
#define STATUS_MASK 0xF0 // 11110000 mask
#define CHANNEL_MASK 0x0F // 00001111 mask

// to have id for objects QMidiIn et QMidiOut
static int QMIDIIN_COUNT;
static int QMIDIOUT_COUNT;

// for QMidiPianoRoll
#define KEY_WIDTH 14
#define KEY_HEIGHT 60
/*! \endcond */

#endif // QMIDI_H
