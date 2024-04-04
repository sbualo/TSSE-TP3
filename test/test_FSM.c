
#include "unity.h"
#include "mock_RC522.h"
#include "mock_TTP229.h"
#include "mock_USERS_DATA.h"
#include "mock_TIMER.h"
#include "mock_LED.h"
#include "mock_SPI.h"
#include "FSM.h"

#define TEST_NUMERO_PULSADO_DEFAULT 255U
#define TEST_NUMERO_PULSADO_EN_USO  0

STATE * TestState;

unsigned char test_id_tarjeta_valido[5] = "CARD";

/**
 * @brief Funcion que se ejecuta antes de cada test (nombre especifico de ceedling)
 *
 */
void setUp(void) {
    TIMER_Start_CMockIgnore();
    LED_KeyboardPress_Ignore(); // Se ignora la funcion que prende los leds
    LED_Card_Blink_Ignore();    // Se ignora la funcion que parpadea leds cuando se lee una tarjeta
    LED_Wrong_Pin_Blink_Ignore(); // Se ignora la funcion que parpadea leds cuando se ingresa pin
                                  // incorrecto
}

void test_inicializacion_FSM_puerta_cerrada(void) {
    TestState = FSM_GetInitState();
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
}

void test_validar_id_tarjeta_FSM(void) {
    unsigned char tarjeta_leida[5] = "CARD";
    GetKeyRead_CMockIgnoreAndReturn(1, tarjeta_leida);
    USERS_DATA_VALIDATE_KEYCARD_CMockExpectAndReturn(1, test_id_tarjeta_valido, true);
    validar_id_tarjeta();
}
void test_id_tarjeta_incorrecta_FSM(void) {
    unsigned char tarjeta_leida[5] = "ACME";
    GetKeyRead_CMockIgnoreAndReturn(1, tarjeta_leida);
    USERS_DATA_VALIDATE_KEYCARD_CMockExpectAndReturn(1, tarjeta_leida, false);
    validar_id_tarjeta();
}

void test_validar_avance_FSM_a_estado_validando_tarjeta(void) {
    TestState = FSM_GetInitState();
    unsigned char tarjeta_leida[5] = "CARD";
    GetKeyRead_CMockIgnoreAndReturn(1, tarjeta_leida);
    USERS_DATA_VALIDATE_KEYCARD_CMockExpectAndReturn(1, test_id_tarjeta_valido, 1);
    TestState =
        fsm(TestState,
            LECTURA_TARJETA); // La FSM avanza de estado y ejecuta fn USERS_DATA_VALIDATE_KEYCARD
    TEST_ASSERT_EQUAL(estado_validando_tarjeta, TestState);
}

void test_lectura_tarjeta_invalida_en_avance_FSM_a_estado_validando_tarjeta(void) {
    TestState = estado_validando_tarjeta;
    TestState = fsm(TestState, TARJETA_INVALIDA); // La FSM debe volver a Puerta cerrada
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
}

void test_reset_fsm_a_estado_inicial(void) {
    TestState = estado_validando_tarjeta;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
}

void test_reset_fsm_desde_todos_los_estados_hacia_estado_inicial(void) {
    TestState = estado_puerta_cerrada;
    TestState = fsm(TestState,
                    TIMEOUT_DEFAULT); // Con este evento TestState debe de quedar en el mismo lugar
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_validando_tarjeta;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_primer_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_segundo_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_tercer_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_cuarto_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_validando_pin;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
    LED_OPEN_DOOR_CMockIgnore();
    TestState = estado_puerta_abierta;
    TestState = fsm(TestState,
                    TIMEOUT_DEFAULT); // Con este evento TestState debe de quedar en el mismo lugar
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
}

void test_avance_FSM_de_estado_validando_tarjeta_a_estado_ingreso_primer_numero(void) {
    TestState = estado_validando_tarjeta;
    TestState = fsm(TestState,
                    TARJETA_VALIDA); // Con este evento la FSM avanza a ingreso_primer_numero
    TEST_ASSERT_EQUAL(estado_ingreso_primer_numero, TestState);
}

void test_avance_FSM_de_estado_estado_ingreso_primer_numero_a_estado_ingreso_segundo_numero(void) {
    TestState = estado_ingreso_primer_numero;
    uint8_t NumeroPulsado = TEST_NUMERO_PULSADO_DEFAULT;
    USERS_DATA_COLLECT_FIRST_NUMBER_CMockExpect(1, &NumeroPulsado);
    TestState = fsm(TestState, LECTURA_NUMERO_TECLADO);
    TEST_ASSERT_EQUAL(TEST_NUMERO_PULSADO_DEFAULT, NumeroPulsado);
    TEST_ASSERT_EQUAL(estado_ingreso_segundo_numero, TestState);
}

void test_avance_FSM_de_estado_estado_ingreso_segundo_numero_a_estado_ingreso_tercer_numero(void) {
    TestState = estado_ingreso_segundo_numero;
    uint8_t NumeroPulsado = 0;
    USERS_DATA_COLLECT_SECOND_NUMBER_CMockExpect(1, &NumeroPulsado);
    TestState = fsm(TestState, LECTURA_NUMERO_TECLADO);
    TEST_ASSERT_EQUAL(TEST_NUMERO_PULSADO_EN_USO, NumeroPulsado);
    TEST_ASSERT_EQUAL(estado_ingreso_tercer_numero, TestState);
}

void test_avance_FSM_de_estado_estado_ingreso_tercer_numero_a_estado_ingreso_cuarto_numero(void) {
    TestState = estado_ingreso_tercer_numero;
    uint8_t NumeroPulsado = 0;
    USERS_DATA_COLLECT_THIRD_NUMBER_CMockExpect(1, &NumeroPulsado);
    TestState = fsm(TestState, LECTURA_NUMERO_TECLADO);
    TEST_ASSERT_EQUAL(TEST_NUMERO_PULSADO_EN_USO, NumeroPulsado);
    TEST_ASSERT_EQUAL(estado_ingreso_cuarto_numero, TestState);
}

void test_avance_FSM_de_estado_estado_ingreso_cuarto_numero_a_estado_validando_pin(void) {
    TestState = estado_ingreso_cuarto_numero;
    uint8_t NumeroPulsado = 0;
    USERS_DATA_COLLECT_FOURTH_NUMBER_CMockExpect(1, &NumeroPulsado);

    USERS_DATA_VALIDATE_PIN_CMockExpectAndReturn(
        1, true); // Se asume siempre un pin válido aunque no afeca este test
    TestState = fsm(TestState, LECTURA_NUMERO_TECLADO);
    TEST_ASSERT_EQUAL(TEST_NUMERO_PULSADO_EN_USO, NumeroPulsado);
    TEST_ASSERT_EQUAL(estado_validando_pin, TestState);
}

void test_estado_ingreso_pin_incorrecto(void) {
    TestState = estado_ingreso_cuarto_numero;
    uint8_t NumeroPulsado = 0;
    USERS_DATA_COLLECT_FOURTH_NUMBER_CMockExpect(1, &NumeroPulsado);

    USERS_DATA_VALIDATE_PIN_CMockExpectAndReturn(1, false); // el pin en este caso es incorrecto
    TestState = fsm(TestState, LECTURA_NUMERO_TECLADO);
    TEST_ASSERT_EQUAL(TEST_NUMERO_PULSADO_EN_USO, NumeroPulsado);
    TEST_ASSERT_EQUAL(estado_validando_pin, TestState);
}

void test_avance_FSM_de_estado_estado_validando_pin_a_estado_puerta_abierta(void) {
    TestState = estado_validando_pin;
    LED_OPEN_DOOR_Ignore();
    TestState = fsm(TestState, PIN_VALIDO);
    TEST_ASSERT_EQUAL(estado_puerta_abierta, TestState);
}
void test_avance_FSM_de_estado_estado_validando_pin_a_estado_ingreso_primer_numero(void) {
    TestState = estado_validando_pin;
    TestState = fsm(TestState, PIN_INVALIDO);
    TEST_ASSERT_EQUAL(estado_ingreso_primer_numero, TestState);
}

void test_avance_FSM_por_timeout_desde_todos_los_estados() {

    TestState = estado_puerta_abierta;
    LED_OPEN_DOOR_Ignore();
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_puerta_cerrada;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TestState = estado_validando_tarjeta;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_primer_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_segundo_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_tercer_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_ingreso_cuarto_numero;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);

    TestState = estado_validando_pin;
    TestState = fsm(TestState, TIMEOUT_DEFAULT);
    TEST_ASSERT_EQUAL(estado_puerta_cerrada, TestState);
}

void test_funcion_generador_evento_RFID(void) {
    get_RFID_event_ocurrence_CMockExpectAndReturn(1, true); // Lectura positiva de RFID
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(LECTURA_TARJETA, TestEvent);
}
void test_funcion_generador_evento_numero_teclado(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(0);
    KEYBOARD_ReadData_CMockExpectAndReturn(1, 5); // Se presiona el numero 5
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(LECTURA_NUMERO_TECLADO, TestEvent);
}

void test_funcion_generador_evento_numero_teclado_invalido(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(0);
    KEYBOARD_ReadData_IgnoreAndReturn(0);      // Se presiona el numero "-1" (lectura defectuosa)
    test_set_TarjetaValida(0);                 // No evento tarjeta
    test_set_pinValido(0);                     // No hay evento de pin
    TIME_GetTimeStatus_IgnoreAndReturn(false); // Evento de timer
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(FIN_TABLA, TestEvent);
}

void test_funcion_generador_evento_tarjeta_valida(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(1);                       // Lectura tarjeta válida
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(TARJETA_VALIDA, TestEvent);
}

void test_funcion_generador_evento_tarjeta_invalida(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(-1);                      // Lectura tarjeta inválida
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(TARJETA_INVALIDA, TestEvent);
}

void test_funcion_generador_evento_pin_valido(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(0);                       // No evento tarjeta
    test_set_pinValido(1);                           // El pin ingresado es correcto
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(PIN_VALIDO, TestEvent);
}

void test_funcion_generador_evento_pin_invalida(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(0);                       // No evento tarjeta
    test_set_pinValido(-1);                          // El pin ingresado es incorrecto
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(PIN_INVALIDO, TestEvent);
}

void test_funcion_generador_evento_timer(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(0);                       // No evento tarjeta
    test_set_pinValido(0);                           // El pin ingresado es incorrecto
    TIME_GetTimeStatus_IgnoreAndReturn(true);        // Evento de timer
    TIME_ResetTimeStatus_Ignore();
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(TIMEOUT_DEFAULT, TestEvent);
}

void test_funcion_generador_evento_fin_tabla(void) {
    get_RFID_event_ocurrence_IgnoreAndReturn(false); // Lectura negativa de RFID
    test_set_NumeroPulsado(-1);                      // No hay evento de numeros
    test_set_TarjetaValida(0);                       // No evento tarjeta
    test_set_pinValido(0);                           // El pin ingresado es incorrecto
    TIME_GetTimeStatus_IgnoreAndReturn(false);       // No hay evento de timer
    eventos TestEvent = get_event();
    TEST_ASSERT_EQUAL(FIN_TABLA, TestEvent);
}
