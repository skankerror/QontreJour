#ifndef QDMXUSBGLOBAL_H
#define QDMXUSBGLOBAL_H

#include <QtGlobal>

namespace dmxusb_details
{

constexpr const quint16 ftdi_vid                        = 0x0403; //! FTDI Vendor ID
constexpr const quint16 atmel_vid                       = 0x03EB; //! Atmel Vendor ID
constexpr const quint16 microchip_vid                   = 0x04D8; //! Microchip Vendor ID

constexpr const quint16 ftdi_pid                        = 0x6001; //! FTDI Product ID
constexpr const quint16 dmx4all_pid                     = 0xC850; //! DMX4ALL FTDI Product ID
constexpr const quint16 nanodmx_pid                     = 0x2018; //! DMX4ALL Nano DMX Product ID
constexpr const quint16 eurolite_pid                    = 0xFA63; //! Eurolite USB DMX Product ID
constexpr const quint16 electrotas_pid                  = 0x0000; //! ElectroTAS USB DMX Product ID

constexpr const quint8  enttec_pro_dmx_zero             = 0x00;
constexpr const quint8  enttec_pro_recv_dmx_pkt         = 0x05;
constexpr const quint8  enttec_pro_send_dmx_rq          = 0x06;
constexpr const quint8  enttec_pro_read_serial          = 0x0A;
constexpr const quint8  enttec_pro_enable_api2          = 0x0D;
constexpr const quint8  enttec_pro_send_dmx_rq2         = 0xA9;
constexpr const quint8  enttec_pro_port_ass_req         = 0xCB;
constexpr const quint8  enttec_pro_start_of_msg         = 0x7E;
constexpr const quint8  enttec_pro_end_of_msg           = 0xE7;
constexpr const quint8  enttec_pro_midi_out_msg         = 0xBE;
constexpr const quint8  enttec_pro_midi_in_msg          = 0xE8;

// RDM defines
constexpr const quint8  enttec_pro_rdm_send             = 0x07;
constexpr const quint8  enttec_pro_rdm_discovery_req    = 0x0B;
constexpr const quint8  enttec_pro_rdm_recv_timeout     = 0x0C;
constexpr const quint8  enttec_pro_rdm_recv_timeout2    = 0x8E;
constexpr const quint8  enttec_pro_rdm_send2            = 0x9D;
constexpr const quint8  enttec_pro_rdm_discovery_req2   = 0xB6;

constexpr const quint16 dmxking_esta_id                 = 0x6A6B;
constexpr const quint8  ultradmx_dmx512a_dev_id         = 0x00;
constexpr const quint8  ultradmx_pro_dev_id             = 0x02;
constexpr const quint8  ultradmx_micro_dev_id           = 0x03;

constexpr const quint8  dmxking_usb_device_manufacturer = 0x4D;
constexpr const quint8  dmxking_usb_device_name         = 0x4E;
constexpr const quint8  dmxking_send_port1              = 0x64;
constexpr const quint8  dmxking_send_port2              = 0x65;

constexpr const quint8 eurolite_usb_dmx_pro_dmx_zero     = 0x00;
constexpr const quint8 eurolite_usb_dmx_pro_send_dmx_rq  = 0x06;
constexpr const quint8 eurolite_usb_dmx_pro_start_of_msg = 0x7E;
constexpr const quint8 eurolite_usb_dmx_pro_end_of_msg   = 0xE7;


constexpr const quint8 vince_cmd_start_dmx  = 0x01; //! CMD_START_DMX
constexpr const quint8 vince_cmd_stop_dmx   = 0x02; //! CMD_STOP_DMX
constexpr const quint8 vince_cmd_reset_dmx  = 0x10; //! CMD_RAZ_DMX
constexpr const quint8 vince_cmd_update_dmx = 0x11; //! CMD_MAJ_DMX
constexpr const quint8 vince_start_of_msg   = 0x0f; //! STX
constexpr const quint8 vince_end_of_msg     = 0x04; //! ETX

constexpr const quint8 vince_resp_ok        = 0x00; //! CMD_OK
constexpr const quint8 vince_resp_ko        = 0x01; //! CMD_KO
constexpr const quint8 vince_resp_warning   = 0x02; //! CMD_WARNING
constexpr const quint8 vince_resp_unknown   = 0x02; //! CMD_UNKNOWN
constexpr const quint8 vince_resp_io_err    = 0x10; //! CMD_IO_ERR
constexpr const quint8 vince_resp_param_err = 0x11; //! CMD_PARAM_ERR

}

#endif // QDMXUSBGLOBAL_H
