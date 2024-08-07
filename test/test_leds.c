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
 * @file test_leds.c
 * @brief Test unitario para modulo de control de 16 leds de dos estados
 */

/* === Headers files inclusions =============================================================== */

#include "unity.h"
#include "leds.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/**
 * @brief Puerto virtual para los leds. Simula el puerto de Hardware donde se conectan los leds
 */

static uint16_t puerto_virtual;

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

/**
 * @brief Llamo a la funcion de inicializacion del Hardware
 */

void setUp(void) {

    leds_init(&puerto_virtual);
}

/**
 * @test Al arrancar el sistema todos los leds tienen que estar apagados
 */

void test_todos_los_leds_deben_arrancar_apagados(void) {

    uint16_t puerto_virtual = 0xFFFF;
    leds_init(&puerto_virtual);
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
}

/**
 * @test Despues de arrancar el sistema, con todos los leds apagados,
 * voy a prender un led cualquiera
 *
 * Implicitamente tambien prueba que los leds estan bien mapeados en la memoria
 */

void test_prender_un_solo_led(void) {

    leds_turn_on(3);
    TEST_ASSERT_EQUAL_HEX16(1 << 2, puerto_virtual);
}

/**
 * @test Voy a prender un led y volver a apagarlo para ver si se apaga
 */

void test_prender_y_apagar_un_solo_led(void) {

    leds_turn_on(3);
    leds_turn_off(3);
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
}

/**
 * @test Prender dos leds, apagar uno, y ver que solo se apaga el que corresponde
 * y que el otro sigue prendido
 */

void test_prender_dos_leds_y_apagar_un_solo_led(void) {

    leds_turn_on(3);
    leds_turn_on(7);
    leds_turn_off(3);
    TEST_ASSERT_EQUAL_HEX16(1 << 6, puerto_virtual);
}

/**
 * @test Voy a consultar el estado de un led apagado y tiene que figurar como apagado
 */

void test_consultar_estado_led_apagado(void) {

    TEST_ASSERT_FALSE(led_is_turned_on(3));
}

/**
 * @test Prender un led, voy a consultar el estado y tiene que figurar como prendido
 */

void test_consultar_estado_led_encendido(void) {

    leds_turn_on(3);
    TEST_ASSERT_TRUE(led_is_turned_on(3));
}

/**
 * @test Prender todos los leds juntos
 */

void test_prender_todos_los_leds(void) {

    leds_turn_on_all();
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, puerto_virtual);
}

/**
 * @test Prender y apagar todos los leds juntos
 */

void test_prender_y_apagar_todos_los_leds(void) {

    leds_turn_on_all();
    leds_turn_off_all();
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
}

/**
 * @test Probar leds fuera del rango valido
 *
 * Encender y apagar leds con valores mayores a 16 y menores a 1
 * y validar que el estado del puerto no cambia en cada cambio.
 */

void test_leds_fuera_de_rango(void) {

    leds_turn_on(-1);
    TEST_ASSERT_FALSE(led_is_turned_on(-1));
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
    leds_turn_on(17);
    TEST_ASSERT_FALSE(led_is_turned_on(17));
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
    leds_turn_off(-1);
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
    leds_turn_off(17);
    TEST_ASSERT_EQUAL_HEX16(0x0000, puerto_virtual);
}

/* === End of documentation ==================================================================== */
