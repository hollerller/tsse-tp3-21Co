/************************************************************************************************
Copyright (c) 2024, David Tobasura <hollerller@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

#ifndef LEDS_H
#define LEDS_H

/**
 * @file leds.h
 * @brief Declaracion de las funciones para controlar los leds.
 */

/* === Headers files inclusions ================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/**
 * @brief Funcion para inicializar los leds
 *
 * @param puerto Puerto virtual para los leds. Simula el puerto de Hardware donde se conectan los
 * leds
 */

void leds_init(uint16_t * puerto);

/**
 * @brief Funciona para encender un led
 *
 * @param led Numero del led a verificar
 */

void leds_turn_on(int led);

/**
 * @brief Funcion para apagar un led
 *
 * @param led Numero del led a verificar
 */

void leds_turn_off(int led);

/**
 * @brief Funcion para verificar si un led esta encendido o apagado
 *
 * @param led Numero del led a verificar
 * @return true si el led esta encendido
 * @return false si el led esta apagado
 */

bool led_is_turned_on(int led);

/**
 * @brief Funcion para encender todos los leds
 */

void leds_turn_on_all();

/**
 * @brief Funcion para apagar todos los leds
 */

void leds_turn_off_all();

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* LEDS_H */
