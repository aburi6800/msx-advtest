// License:MIT License
// copyright-holders:Hitoshi Iwai

#ifndef ASMSUB_H
#define ASMSUB_H

void put_message(uint8_t x, uint8_t y, const void *value_addr) __naked;

void put_message_direct(uint8_t x, uint8_t y, const void *value_addr) __naked;

void clear_message() __naked;

void getkeycode() __naked;

void keywait() __naked;

void buffer_check() __naked;

void switch_bank(uint8_t bank_no) __naked;

void check_region() __naked;

void set_palette() __naked;

#endif
