
#include "unity.h"
#include "mock_RC522.h"
#include "mock_TTP229.h"
#include "mock_USERS_DATA.h"
#include "mock_TIMER.h"
#include "mock_LED.h"
#include "mock_SPI.h"
#include "FSM.h"

STATE * TestState;

unsigned char * test_id_tarjeta_valido = "CARD";

/**
 * @brief Funcion que se ejecuta antes de cada test (nombre especifico de ceedling)
 *
 */
void setUp(void) {
  
}

void test_inicializacion_FSM_puerta_cerrada(void) {
    TestState =  FSM_GetInitState();
    TEST_ASSERT_EQUAL_UINT16(estado_puerta_cerrada, TestState);
}

void test_validar_id_tarjeta_FSM(void){
    unsigned char * tarjeta_leida = "CARD";
    GetKeyRead_CMockIgnoreAndReturn(1,tarjeta_leida);
    USERS_DATA_VALIDATE_KEYCARD_CMockExpectAndReturn(1,test_id_tarjeta_valido,1);
    validar_id_tarjeta();
}



// #include "unity.h"

// #include "leds.h"

// #define TEST_LED_TO_BIT(x) (1 << (x - 1))
// #define TEST_ALL_LED_OFF   0x00
// #define TEST_ALL_LED_ON    0b1111111111111111

// static uint16_t leds_virtuales;
// static const int LED = 4;

// /**
//  * @brief Funcion que se ejecuta antes de cada test (nombre especifico de ceedling)
//  *
//  */
// void setUp(void) {

//     leds_init(&leds_virtuales);
// }

// void test_todos_los_led_deben_estar_apagados_al_iniciar_el_driver(void) {
//     uint16_t leds_virtuales = 0xFF;
//     leds_init(&leds_virtuales);
//     TEST_ASSERT_EQUAL_UINT16(0x00, leds_virtuales);
// }

// /* prueba de encendido de un led*/

// void test_prender_un_led(void) {

//     leds_turn_on(LED);
//     // El bit 2 esta en alto
//     TEST_ASSERT_EQUAL_UINT16(1 << (LED - 1), leds_virtuales);
//     // Todos los bits estan en bajo
//     TEST_ASSERT_BITS_LOW(~(1 << (LED - 1)), leds_virtuales);
// }

// void test_apagar_un_led(void) {

//     leds_turn_on(LED);
//     leds_turn_off(LED);

//     TEST_ASSERT_EQUAL_UINT16(0x00, leds_virtuales);
// }

// void test_operar_varios_led(void) {

//     leds_turn_on(5);
//     leds_turn_on(7);
//     leds_turn_on(5);
//     leds_turn_off(5);
//     leds_turn_off(9);

//     TEST_ASSERT_EQUAL_UINT16(1 << (7 - 1), leds_virtuales);
// }

// void test_prender_todos_los_led(void) {

//     leds_turn_all_on();

//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
// }

// void test_apagar_todos_los_led(void) {
//     leds_turn_all_off();
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_OFF, leds_virtuales);
// }

// void test_limit_values(void) {
//     leds_turn_on(0);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_OFF, leds_virtuales);
//     leds_turn_on(1);
//     TEST_ASSERT_EQUAL_UINT16(TEST_LED_TO_BIT(1), leds_virtuales);

//     leds_turn_off(0);
//     TEST_ASSERT_EQUAL_UINT16(TEST_LED_TO_BIT(1), leds_virtuales);
//     leds_turn_off(1);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_OFF, leds_virtuales);

//     // test valores superiores

//     leds_turn_on(17);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_OFF, leds_virtuales);
//     leds_turn_on(16);
//     TEST_ASSERT_EQUAL_UINT16(TEST_LED_TO_BIT(16), leds_virtuales);

//     leds_turn_off(17);
//     TEST_ASSERT_EQUAL_UINT16(TEST_LED_TO_BIT(16), leds_virtuales);
//     leds_turn_off(16);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_OFF, leds_virtuales);
// }

// void test_limit_values_all_leds_on(void) {

//     leds_turn_all_on();
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
//     // Ya estan todos los led prendidos

//     leds_turn_on(0);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
//     leds_turn_on(1);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);

//     leds_turn_off(0);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
//     leds_turn_off(1);
//     TEST_ASSERT_EQUAL_UINT16(0b1111111111111110, leds_virtuales);

//     leds_turn_on(1);
//     // test valores superiores

//     leds_turn_on(17);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
//     leds_turn_on(16);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);

//     leds_turn_off(17);
//     TEST_ASSERT_EQUAL_UINT16(TEST_ALL_LED_ON, leds_virtuales);
//     leds_turn_off(16);
//     TEST_ASSERT_EQUAL_UINT16(0b0111111111111111, leds_virtuales);
// }

// void test_check_led_status_off(void) {
//     bool status = led_check_status(1);
//     TEST_ASSERT_EQUAL_UINT16(false, status);
// }

// void test_check_led_status_on(void) {
//     leds_turn_on(1);
//     bool status = led_check_status(1);
//     TEST_ASSERT_EQUAL_UINT16(true, status);
// }
