#pragma once

#include <cstdint>

#include "zfxyzapi.h"

/**
 * @namespace
 *
 * Contains XYZ-TCP-Streaming definitions.
 *
 * @note Classic XYZ-Datastreaming where the data is decocded and send by the
 *       scanner is deprecated.
 *       It was introduced to support Linux users and is now obsolete, as Linux
 *       SDK is now also available.
 *
 * The transfer scheme uses TCP/IP to transmit data from scanner to the client.
 * The scanner creates for this an server port at port 9105 which accepts
 * exactly one session.
 * The scanner then sends, when a scan starts or is alredy running, data which
 * holds the scanned data.
 *
 * What encoding of data is used can be controlled via scan parameter. The xyz
 * scheme is deprecated.
 *
 * Instead the scanner offers the possibility to send raw line data which is
 * encoded in the same way as a ZFS file is. An SDK is needed to extract the
 * point data from it.
 * This mode comes in two forms:
 *
 * * Uncompressed Raw, which transmits always the full line, ignoring any
 *   selection. Commanded via -linestream tcp
 *
 * * Compressed Raw, which transmits only the selection and is compressed using
 *   zlib or lz4 algorithm and saves significant network traffic.
 *   Since Firmware V10. Commanded via -linestream tcpzip
 */
namespace zfxyz {
    enum payload_type
    {
        UNKNOWN = 0,        ///< Unknown payload or decoding error
        PIXEL_ENC_ID = 1,   ///< pixel_enc data is following ** Deprecated **
        CORR_VALUES_ID = 2, ///< correction_values_xyz data is following
                            /// ** Deprecated **
        LINE_HEADER_ID = 3, ///< line_header data is following
                            ///< size: length of header in Bytes
                            ///< elems: 1
                            ///< No separate Data type
        ZFS_HEADER_ID = 4,  ///< ZFS (file) header is following
                            ///< size: length of header in Bytes
                            ///< elems: 1
        PIXEL_ENC2_ID = 5,  ///< pixel_enc2 data is following ** Deprecated **
        RAW_PIXEL_ID  = 6,  ///< pixel data as in zfs file
                            ///< line header comes seperate as LINE_HEADER_ID
        COMP_PIXEL_ID = 7,  ///< Compressed pixel data. Transfers only selection
                            /// Check ZFS Header to determine compression
                            /// algorithm used!
                            /// elems: 1
                            /// size: length of the compressed data
    };


    /**
     * Hält Informationen zu den nachfolgenden Bytes
     *
     * Codiert werden:
     * @li Start-Marker: String "Z+F\0" (4 Bytes)
     * @li Datentyp der nachfolgenden Byes (payload_type): 2 Bytes
     * @li Anzahl der Payload-Bytes: 4 Bytes
     * @li Anzahl der Payload-Elemente: 4 Bytes
     *
     * Insgesamt werden also 14 Bytes übertragen.
     *
     * Der Startmaker hat immer den Wert Z+F mit nachfolgendem 0-Byte. Dies
     * dient zum wiederfinden des Starts einer Übertragung.
     *
     * Der Header leitet eine Übertragung eines anderen Typs ein. Daher wird in
     * type die Art der Daten, welche übertragen werden, kodiert. Size gibt die
     * länge des Payloades (in Bytes) an.
     *
     * Werden mehrere gleichartige Daten übertragen, so wird ledigleich einmal
     * der header gesendet. Dieser liefert mit elems, wie viele gleichartige
     * Datenelemete übertragen werden.
     *
     * Die Größe eines einzelnen Elements multipliziert mit dem Wert elems
     * ergibt den Wert size!!!
     */
    class ZFXYZ_API header
    {
        unsigned char m_data[14];

    public:
        header(payload_type type=UNKNOWN,
               unsigned int size=0,
               unsigned int elems=0);

        bool has_marker() const;

        // set/get all in once
        void set_data(payload_type type, unsigned int size, unsigned int elems);
        void get_data(
            payload_type &type,
            unsigned int &size,
            unsigned int &elems) const;

        // individual access
        void set_type(payload_type v);
        payload_type get_type() const;

        void set_size(unsigned int v);
        unsigned int get_size() const;

        void set_elems(unsigned int v);
        unsigned int get_elems() const;
    };

    /**
     * Hält den Korrekturvector für eine Encoder-Position
     *
     * Codiert werden folgende Werte:
     * @li Korrekturwerte für x-, y- und z-Richtung á 4 byte (float)
     */
    class ZFXYZ_API correction_values_xyz
    {
        unsigned char m_data[12];  ///< Codierte Daten

    public:
        static const payload_type payload_id = CORR_VALUES_ID;
        correction_values_xyz(float x=0.f, float y=0.f, float z=0.f);
        void set_data(float x, float y, float z);
        void get_data(float &x, float &y, float &z) const;
    };

    /**
     * Hält genug Codierte Daten für Imager-Mode
     *
     * Codiert werden folgende Werte:
     * @li Horizontal-Encoder 24 bit (21 Notwendig)
     * @li Vertikal-Encoder   24 bit (18 Notwendig)
     * @li Entfernung         24 bit (1/10el mm, ohne Vorzeichen)
     * @li Reflexivität       16 bit
     * @li Zusätzliches Byte   8 bit
     *
     * In Summe sind dies 12 Byte.
     *
     * Die Methoden set_data() sowie get_data() können zum extrahieren der Daten
     * genutzt werden.
     */
    class ZFXYZ_API pixel_enc
    {
        unsigned char m_data[12]; ///< Codierte Daten

    public:
        static const payload_type payload_id = PIXEL_ENC_ID;

        // init m_data with 0
        pixel_enc();

        void set_data(
            unsigned int   horz_enc,
            unsigned int   vert_enc,
            double         range,
            unsigned int   reflex,
            unsigned char  add_byte);

        void get_data(
            unsigned int   &horz_enc,
            unsigned int   &vert_enc,
            double         &range,
            unsigned int   &reflex,
            unsigned char  &add_byte) const;

        void set_horz_enc(unsigned int v);
        void set_vert_enc(unsigned int v);
        void set_range(double v);
        void set_reflex(unsigned int v);
        void set_add_byte(unsigned char v);

        unsigned int get_horz_enc() const;
        unsigned int get_vert_enc() const;
        double get_range() const;
        unsigned int get_reflex() const;
        unsigned char get_add_byte() const;
    };

    /**
     * Hält genug Codierte Daten für Imager-Mode
     *
     * Codiert werden folgende Werte:
     * @li Horizontal-Encoder 22 bit (21 Notwendig)
     * @li Vertikal-Encoder   20 bit (18 Notwendig)
     * @li Vertikal-Enc.-Timer 4 bit
     * @li Entfernung         24 bit (1/10el mm, ohne Vorzeichen)
     * @li Reflexivität       16 bit
     * @li Zusätzliches Byte   8 bit
     *
     * In Summe sind dies 12 Byte.
     *
     * Die Methoden set_data() sowie get_data() können zum extrahieren der Daten
     * genutzt werden.
     */
    class ZFXYZ_API pixel_enc2
    {
        unsigned char m_data[12]; ///< Codierte Daten

    public:
        static const payload_type payload_id = PIXEL_ENC2_ID;

        // init m_data with 0
        pixel_enc2();

        void set_data(
            unsigned int   horz_enc,
            unsigned int   vert_enc,
            unsigned char  vert_timer,
            double         range,
            unsigned int   reflex,
            unsigned char  add_byte);

        void get_data(
            unsigned int   &horz_enc,
            unsigned int   &vert_enc,
            unsigned char  &vert_timer,
            double         &range,
            unsigned int   &reflex,
            unsigned char  &add_byte) const;

        void set_horz_enc(unsigned int v);
        void set_vert_enc(unsigned int v);
        void set_vert_timer(unsigned char v);
        void set_range(double v);
        void set_reflex(unsigned int v);
        void set_add_byte(unsigned char v);

        unsigned int get_horz_enc() const;
        unsigned int get_vert_enc() const;
        unsigned char get_vert_timer() const;
        double get_range() const;
        unsigned int get_reflex() const;
        unsigned char get_add_byte() const;
    };
}
