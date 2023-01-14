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

#ifndef QONTREJOUR_H
#define QONTREJOUR_H

enum dmxDrivers
{
  dummy,
  artnet,
  e131,
  dmxusb,
  uart,
  Count_dmxDrivers = uart +1
};
// types.h

#ifndef	HIDDEN
#define	HIDDEN	static
#endif

/* Amiga conventions */
typedef unsigned long	ULONG;		/* unsigned 32-bit quantity */
typedef unsigned short	UWORD;		/* unsigned 16-bit quantity */
typedef unsigned char	UBYTE;		/* unsigned  8-bit quantity  */
typedef unsigned char	*STRPTR;	/* string pointer           */

/* Windows conventions */
typedef	unsigned long	DWORD;		/* unsigned 32-bit quantity */
typedef	unsigned short	WORD;		/* unsigned 16-bit quantity */
typedef	unsigned char	BYTE;		/* unsigned  8-bit quantity */

/* Other conventions */
typedef unsigned long	WORD32;		/* unsigned 32-bit quantity */
typedef unsigned short	WORD16;		/* unsigned 16-bit quantity */
typedef unsigned char	WORD8;		/* unsigned  8-bit quantity */

#ifdef	__GNUC__
#define	PACK	__attribute__ ((packed))
#else
#define	PACK
#endif

//

// errcode.h

#define	OK                  0
#define	BADPARAM            -2			// Parametri alla funzione errati

#define	FILENOTFOUND		-3			// File non aperto in lettura
#define	CREATEERROR         -4			// File non aperto in scrittura
#define	BADFILETYPE         -5			// File di tipo errato
#define	READERROR           -6			// Errore in lettura dal file
#define	WRITEERROR          -7			// Errore in scrittura da file
#define	NOTHINGTOSAVE		-8			// Nessun contenuto da salvare
#define NOTSUPPORTED		-9			// Funzionalita` non supportata (ancora)

#define	E2P_TIMEOUT         -10

//#define IICERR_NOERR		OK
#define IICERR_BUSBUSY		-11
#define IICERR_NOTACK		-12
#define IICERR_NOADDRACK	-13
#define IICERR_SDACONFLICT	-14
#define	IICERR_SCLCONFLICT	-15
#define	E2ERR_OPENFAILED	-16
#define	E2ERR_ACCESSDENIED	-17
#define E2ERR_NOTINSTALLED	-18
#define IICERR_TIMEOUT		-19
#define	IICERR_STOP         -20

#define	E2ERR_WRITEFAILED	-21

#define	DEVICE_BADTYPE		-23
#define	DEVICE_UNKNOWN		-24
#define	DEVICE_LOCKED		-25
#define	OP_ABORTED          -26

#define	BUFFEROVERFLOW		-30
#define	OUTOFMEMORY         -31
#define	BUFFERUNDERFLOW		-32

//Codici di errore restituiti dall'interprete di comandi a menu
#define	CMD_BUFFEREMPTY		-39
#define CMD_NOTHINGTOWRITE	-40
#define	CMD_NOTHINGTOVERIFY	-41
#define	CMD_NOTHINGTOLOAD	-42
#define	CMD_NOTHINGTOSAVE	-43
#define	CMD_WRITEFAILED		-44
#define	CMD_VERIFYFAILED	-45
#define	CMD_ROLLOVERFAILED	-46

//

//rs232.h

//#define	MAX_COMPORTS        4
//#define	MAXLINESIZE         128

//#define ErrTimeout          9997
//#define TIMEOUT_INF ~((timeout_t) 0)

//#define INVALID_HANDLE_VALUE -1

//#define DEBUG_RS232

//

//enttecdmxusb.h

//#define NB_CANAUX_MAX 512
//#define NB_INTERFACES 2


//#define DEBUG_DMX_USB

//

#endif // QONTREJOUR_H
