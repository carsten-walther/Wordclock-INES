/*
 * Ticker
 *
 * @file Ticker.cpp
 * @package Ticker
 *
 * @author Carsten Walther
 */

#include "c_types.h"
#include "eagle_soc.h"
#include "osapi.h"

#include "Ticker.h"

Ticker::Ticker() : _timer(nullptr) {

}

Ticker::~Ticker() {
    detach();
}

void Ticker::_attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, void* arg) {
    if (_timer) {
        os_timer_disarm(_timer);
    } else {
        _timer = &_etsTimer;
    }
    os_timer_setfn(_timer, callback, arg);
    os_timer_arm(_timer, milliseconds, repeat);
}

void Ticker::detach() {
    if (!_timer) {
        return;
    }
    os_timer_disarm(_timer);
    _timer = nullptr;
    _callback_function = nullptr;
}

bool Ticker::active() const {
    return _timer;
}

void Ticker::_static_callback(void* arg) {
    Ticker* _this = reinterpret_cast<Ticker*>(arg);
    if (_this && _this->_callback_function) {
        _this->_callback_function();
    }
}
