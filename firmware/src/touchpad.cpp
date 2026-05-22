#include "touchpad.h"

PS2Mouse mouse(TP_SCL, TP_SDA);

volatile uint8_t tp_raw[6];
volatile bool tp_data_ready = false;

volatile uint8_t tp_bit_count = 0;
volatile uint8_t tp_current_byte = 0;
volatile uint8_t tp_byte_count = 0;
volatile uint32_t tp_last_edge_time = 0;

void initTouchpad() {
  mouse.begin();
  attachInterrupt(digitalPinToInterrupt(TP_SCL), handleTouchpadInterrupt, FALLING);
}

void handleTouchpadInterrupt() {
  uint32_t now = micros();

  if (now - tp_last_edge_time > 4000) {
    tp_bit_count = 0;
    tp_current_byte = 0;
    tp_byte_count = 0;
  }
  tp_last_edge_time = now;

  uint8_t val = digitalRead(TP_SDA);

  if (tp_bit_count == 0) {
    if (val == 0) tp_bit_count++;
  } else if (tp_bit_count >= 1 && tp_bit_count <= 8) {
    tp_current_byte |= (val << (tp_bit_count - 1));
    tp_bit_count++;
  } else if (tp_bit_count == 9) {
    tp_bit_count++;
  } else if (tp_bit_count == 10) {
    if (val == 1) {
      if (!tp_data_ready) {
        tp_raw[tp_byte_count++] = tp_current_byte;
        if (tp_byte_count == 6) {
          tp_data_ready = true;
          tp_byte_count = 0;
        }
      }
    } else {
      tp_byte_count = 0;
    }
    tp_bit_count = 0;
    tp_current_byte = 0;
  }
}

void processTouchpad() {
  if (!tp_data_ready) return;

  byte raw[6];

  noInterrupts();
  memcpy(raw, (const void*)tp_raw, 6);
  tp_data_ready = false;
  interrupts();

  if ((raw[0] & 0xC0) == 0x80 && (raw[3] & 0xC0) == 0xC0) {
    uint16_t x, y;
    mouse.getAbsoluteAxis(raw, x, y);
    int z = raw[2];
    uint8_t btn = raw[0] & 0x03;

    uint8_t w = ((raw[0] & 0x30) >> 2) | ((raw[3] & 0x04) >> 2);

    static uint16_t agm_x0 = 0, agm_y0 = 0;
    static bool has_agm = false;

    x = map(constrain(x, TOUCHPAD_MIN_X, TOUCHPAD_MAX_X), TOUCHPAD_MIN_X, TOUCHPAD_MAX_X, 0, EXPECTED_RES_X);
    y = map(constrain(y, TOUCHPAD_MIN_Y, TOUCHPAD_MAX_Y), TOUCHPAD_MIN_Y, TOUCHPAD_MAX_Y, EXPECTED_RES_Y, 0);

    if (w == 2) {
      agm_x0 = x;
      agm_y0 = y;
      has_agm = true;
      return;
    }

    uint8_t fingers = 0;
    if (z > 25) {
      if (w >= 4) fingers = 1;
      else if (w == 0) fingers = 2;
      else if (w == 1) fingers = 3;
    }

    uint8_t len = 1 + 5 * fingers;

    uint8_t payload[20] = {0};

    payload[0] = btn;

    if (fingers >= 1) {
      uint16_t x0 = (fingers == 2 && has_agm) ? agm_x0 : x;
      uint16_t y0 = (fingers == 2 && has_agm) ? agm_y0 : y;

      if (x0 == 0) x0 = 1;
      if (y0 == 0) y0 = 1;

      payload[1] = 0x10;
      payload[2] = x0 & 0xFF;
      payload[3] = (x0 >> 8) & 0xFF;
      payload[4] = y0 & 0xFF;
      payload[5] = (y0 >> 8) & 0xFF;
    }

    if (fingers >= 2) {
      uint16_t x1 = has_agm ? x : x + 256;
      uint16_t y1 = has_agm ? y : y + 256;

      payload[6] = 0x20;
      payload[7] = x1 & 0xFF;
      payload[8] = (x1 >> 8) & 0xFF;
      payload[9] = y1 & 0xFF;
      payload[10] = (y1 >> 8) & 0xFF;
    }

    if (fingers == 3) {
      uint16_t x2 = (x > 256) ? x - 256 : 0;
      uint16_t y2 = (y > 256) ? y - 256 : 0;

      payload[11] = 0x30;
      payload[12] = x2 & 0xFF;
      payload[13] = (x2 >> 8) & 0xFF;
      payload[14] = y2 & 0xFF;
      payload[15] = (y2 >> 8) & 0xFF;
    }

    sendPacket(PACKET_ID_TOUCHPAD_REPORT, payload, 20);
    digitalWrite(PC13, fingers < 1);

    has_agm = false;
  } else {
    noInterrupts();
    tp_byte_count = 0;
    interrupts();
  }
}
