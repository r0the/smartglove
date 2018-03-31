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

#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

#define BUTTON_THUMB_1          0x0001
#define BUTTON_THUMB_2          0x0002
#define BUTTON_THUMB_3          0x0004
#define BUTTON_THUMB_4          0x0008
#define BUTTON_INDEX_FINGER_1   0x0010
#define BUTTON_MIDDLE_FINGER_1  0x0020
#define BUTTON_RING_FINGER_1    0x0040
#define BUTTON_LITTLE_FINGER_1  0x0080
#define BUTTON_INDEX_FINGER_2   0x0100
#define BUTTON_MIDDLE_FINGER_2  0x0200

#define BUTTON_COUNT 12

class Buttons {
public:
    Buttons();
    bool down(uint16_t button) const;
    inline bool longPress() const { return _longPress; }
    bool pressed(uint16_t button) const;
    void setupLongPress(uint16_t buttons, uint16_t millis);
    void updateState(uint16_t current);
private:
    uint16_t _current;
    uint16_t _last;
    bool _longPress;
    unsigned long _longPressMillis;
    unsigned long _longPressEnd;
    uint16_t _longPressButtons;
};

#endif

