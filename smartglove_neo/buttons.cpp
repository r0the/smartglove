/*
 * Copyright (C) 2018 by Stefan Rothe
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

#include "buttons.h"

Buttons::Buttons() :
    _current(0),
    _last(0),
    _longPress(false),
    _longPressButtons(0),
    _longPressMillis(5000) {
}

bool Buttons::down(uint16_t button) const {
    return pressed(button) && ((_last & button) == 0);
}

bool Buttons::pressed(uint16_t button) const {
    return (_current & button) == button;
}

void Buttons::setupLongPress(uint16_t buttons, uint16_t millis) {
    _longPressButtons = buttons;
    _longPressMillis = millis;
}

void Buttons::updateState(uint16_t current) {
    unsigned long now = millis();
    _last = _current;
    _current = current;
    if ((_current & _longPressButtons) != _longPressButtons) {
        _longPressEnd = now + _longPressMillis;
    }

    if (_longPressEnd <= now) {
        _longPress = true;
        _longPressEnd = now + _longPressMillis;
    }
    else {
        _longPress = false;
    }
}

