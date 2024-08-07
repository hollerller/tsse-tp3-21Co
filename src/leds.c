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

/**
 * @file leds.c
 * @brief Definicion de las funciones para controlar los leds.
 */

/* === Headers files inclusions =============================================================== */

#include "leds.h"

/* === Macros definitions ====================================================================== */

/**
 * @brief Offset en la posicion del led para calcular la posicion del bit correspondiente a un led
 */
#define LED_OFFSET 1

/**
 * @brief Posicion del bit correspondiente al primer led
 */
#define FIRST_BIT 1

/**
 * @brief Valor que representa que todos los leds estan apagados
 */
#define ALL_LEDS_OFF 0x0000

/**
 * @brief Valor que representa que todos los leds estan encendidos
 */
#define ALL_LEDS_ON 0xFFFF

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/**
 * @brief Puntero que corresponde al puerto virtual para los leds.
 * Simula el puerto de Hardware donde se conectan los leds
 */

static uint16_t * puerto_virtual;

/* === Private function declarations =========================================================== */

/**
 * @brief Funcion que convierte un numero de led a una mascara de bits
 *
 * @param led  Numero del led a convertir
 * @return * uint16_t Mascara de bits corresopndiente al led
 */

static uint16_t led_to_mask(int led);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static uint16_t led_to_mask(int led) {
    return (FIRST_BIT << (led - LED_OFFSET));
}

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Funcion para inicializar los leds. Coloca en cero todos los leds
 *
 * @param puerto Puerto virtual para los leds. Simula el puerto de Hardware donde se conectan los
 * leds
 */

void leds_init(uint16_t * puerto) {
    puerto_virtual = puerto;
    leds_turn_off_all();
}

/**
 * @brief Funcion para encender un led. Coloca un 1 en el bit correspondiente al led
 * si el led se encuentra en el rango de 1 a 16 inclusive
 *
 * @param led Numero del led a verificar
 */

void leds_turn_on(int led) {
    if (led >= 1 && led <= 16) {
        *puerto_virtual |= led_to_mask(led);
    }
}

/**
 * @brief Funcion para apagar un led. Coloca un cero en el bit correspondiente al led
 * si el led se encue en el rango de 1 a 16 inclusive
 *
 * @param led Numero del led a verificar
 */

void leds_turn_off(int led) {
    if (led >= 1 && led <= 16) {
        *puerto_virtual &= ~led_to_mask(led);
    }
}

/**
 * @brief Funcion para verificar si un led esta encendido o apagado
 * si el led se encuentra dentro en el rango de 1 a 16 inclusive
 *
 * @param led Numero del led a verificar
 * @return true si el led esta encendido
 * @return false si el led esta apagado
 */

bool led_is_turned_on(int led) {
    bool check = ((*puerto_virtual & led_to_mask(led)) != 0);
    if (led >= 1 && led <= 16) {
        return check;
    }
}

/**
 * @brief Funcion para encender todos los leds. Coloca el valor 0xFFFF en el puerto virtual
 */

void leds_turn_on_all() {
    *puerto_virtual = ALL_LEDS_ON;
}

/**
 * @brief Funcion para apagar todos los leds. Coloca el valor 0x0000 en el puerto virtual
 */

void leds_turn_off_all() {
    *puerto_virtual = ALL_LEDS_OFF;
}

/* === End of documentation ==================================================================== */
