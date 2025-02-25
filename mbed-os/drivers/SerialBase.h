/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SERIALBASE_H
#define MBED_SERIALBASE_H

#include "platform/platform.h"

#if DEVICE_SERIAL

#include "MbedStream.h"
#include "Callback.h"
#include "serial_api.h"
#include "mbed_toolchain.h"

#if DEVICE_SERIAL_ASYNCH
#include "CThunk.h"
#include "dma_api.h"
#endif

namespace mbed {
/** \addtogroup drivers */
/** @{*/

/** A base class for serial port implementations
 * Can't be instantiated directly (use Serial or RawSerial)
 *
 * @Note Synchronization level: Set by subclass
 */
class SerialBase {

public:
    /** Set the baud rate of the serial port
     *
     *  @param baudrate The baudrate of the serial port (default = 9600).
     */
    void baud(int baudrate);

    enum Parity {
        None = 0,
        Odd,
        Even,
        Forced1,
        Forced0
    };

    enum IrqType {
        RxIrq = 0,
        TxIrq,

        IrqCnt
    };

    enum Flow {
        Disabled = 0,
        RTS,
        CTS,
        RTSCTS
    };

    /** Set the transmission format used by the serial port
     *
     *  @param bits The number of bits in a word (5-8; default = 8)
     *  @param parity The parity used (SerialBase::None, SerialBase::Odd, SerialBase::Even, SerialBase::Forced1, SerialBase::Forced0; default = SerialBase::None)
     *  @param stop The number of stop bits (1 or 2; default = 1)
     */
    void format(int bits=8, Parity parity=SerialBase::None, int stop_bits=1);

    /** Determine if there is a character available to read
     *
     *  @returns
     *    1 if there is a character available to read,
     *    0 otherwise
     */
    int readable();

    /** Determine if there is space available to write a character
     *
     *  @returns
     *    1 if there is space to write a character,
     *    0 otherwise
     */
    int writeable();

    /** Attach a function to call whenever a serial interrupt is generated
     *
     *  @param func A pointer to a void function, or 0 to set as none
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     */
    void attach(Callback<void()> func, IrqType type=RxIrq);

    /** Attach a member function to call whenever a serial interrupt is generated
     *
     *  @param obj pointer to the object to call the member function on
     *  @param method pointer to the member function to be called
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     *  @deprecated
     *      The attach function does not support cv-qualifiers. Replaced by
     *      attach(callback(obj, method), type).
     */
    template<typename T>
    MBED_DEPRECATED_SINCE("mbed-os-5.1",
        "The attach function does not support cv-qualifiers. Replaced by "
        "attach(callback(obj, method), type).")
    void attach(T *obj, void (T::*method)(), IrqType type=RxIrq) {
        attach(callback(obj, method), type);
    }

    /** Attach a member function to call whenever a serial interrupt is generated
     *
     *  @param obj pointer to the object to call the member function on
     *  @param method pointer to the member function to be called
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     *  @deprecated
     *      The attach function does not support cv-qualifiers. Replaced by
     *      attach(callback(obj, method), type).
     */
    template<typename T>
    MBED_DEPRECATED_SINCE("mbed-os-5.1",
        "The attach function does not support cv-qualifiers. Replaced by "
        "attach(callback(obj, method), type).")
    void attach(T *obj, void (*method)(T*), IrqType type=RxIrq) {
        attach(callback(obj, method), type);
    }

    /** Generate a break condition on the serial line
     */
    void send_break();

protected:

    /** Acquire exclusive access to this serial port
     */
    virtual void lock(void);

    /** Release exclusive access to this serial port
     */
    virtual void unlock(void);

public:

#if DEVICE_SERIAL_FC
    /** Set the flow control type on the serial port
     *
     *  @param type the flow control type (Disabled, RTS, CTS, RTSCTS)
     *  @param flow1 the first flow control pin (RTS for RTS or RTSCTS, CTS for CTS)
     *  @param flow2 the second flow control pin (CTS for RTSCTS)
     */
    void set_flow_control(Flow type, PinName flow1=NC, PinName flow2=NC);
#endif

    static void _irq_handler(uint32_t id, SerialIrq irq_type);

#if DEVICE_SERIAL_ASYNCH

    /** Begin asynchronous write using 8bit buffer. The completition invokes registered TX event callback
     *
     *  @param buffer   The buffer where received data will be stored
     *  @param length   The buffer length in bytes
     *  @param callback The event callback function
     *  @param event    The logical OR of TX events
     */
    int write(const uint8_t *buffer, int length, const event_callback_t& callback, int event = SERIAL_EVENT_TX_COMPLETE);

    /** Begin asynchronous write using 16bit buffer. The completition invokes registered TX event callback
     *
     *  @param buffer   The buffer where received data will be stored
     *  @param length   The buffer length in bytes
     *  @param callback The event callback function
     *  @param event    The logical OR of TX events
     */
    int write(const uint16_t *buffer, int length, const event_callback_t& callback, int event = SERIAL_EVENT_TX_COMPLETE);

    /** Abort the on-going write transfer
     */
    void abort_write();

    /** Begin asynchronous reading using 8bit buffer. The completition invokes registred RX event callback.
     *
     *  @param buffer     The buffer where received data will be stored
     *  @param length     The buffer length in bytes
     *  @param callback   The event callback function
     *  @param event      The logical OR of RX events
     *  @param char_match The matching character
     */
    int read(uint8_t *buffer, int length, const event_callback_t& callback, int event = SERIAL_EVENT_RX_COMPLETE, unsigned char char_match = SERIAL_RESERVED_CHAR_MATCH);

    /** Begin asynchronous reading using 16bit buffer. The completition invokes registred RX event callback.
     *
     *  @param buffer     The buffer where received data will be stored
     *  @param length     The buffer length in bytes
     *  @param callback   The event callback function
     *  @param event      The logical OR of RX events
     *  @param char_match The matching character
     */
    int read(uint16_t *buffer, int length, const event_callback_t& callback, int event = SERIAL_EVENT_RX_COMPLETE, unsigned char char_match = SERIAL_RESERVED_CHAR_MATCH);

    /** Abort the on-going read transfer
     */
    void abort_read();

    /** Configure DMA usage suggestion for non-blocking TX transfers
     *
     *  @param usage The usage DMA hint for peripheral
     *  @return Zero if the usage was set, -1 if a transaction is on-going
     */
    int set_dma_usage_tx(DMAUsage usage);

    /** Configure DMA usage suggestion for non-blocking RX transfers
     *
     *  @param usage The usage DMA hint for peripheral
     *  @return Zero if the usage was set, -1 if a transaction is on-going
     */
    int set_dma_usage_rx(DMAUsage usage);

protected:
    void start_read(void *buffer, int buffer_size, char buffer_width, const event_callback_t& callback, int event, unsigned char char_match);
    void start_write(const void *buffer, int buffer_size, char buffer_width, const event_callback_t& callback, int event);
    void interrupt_handler_asynch(void);
#endif

protected:
    SerialBase(PinName tx, PinName rx, int baud);
    virtual ~SerialBase() {
    }

    int _base_getc();
    int _base_putc(int c);

#if DEVICE_SERIAL_ASYNCH
    CThunk<SerialBase> _thunk_irq;
    event_callback_t _tx_callback;
    event_callback_t _rx_callback;
    DMAUsage _tx_usage;
    DMAUsage _rx_usage;
#endif

    serial_t         _serial;
    Callback<void()> _irq[IrqCnt];
    int              _baud;

};

} // namespace mbed

#endif

#endif

/** @}*/
