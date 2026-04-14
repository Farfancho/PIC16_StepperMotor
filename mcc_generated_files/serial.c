#include <xc.h> 

#include "serial.h"

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart1.h"

#define SERIAL_RX_LINE_SIZE  24u

static motor_status_t *s_motor = NULL;
static char s_rx_line[SERIAL_RX_LINE_SIZE];
static uint8_t s_rx_index = 0;
static volatile bool s_position_report_pending = false;

/* ========================= Helpers privados ========================= */

static void Serial_WriteChar(char c);
static void Serial_WriteString(const char *s);
static void Serial_WriteUInt32(uint32_t value);
static void Serial_WritePosition(void);

static void Serial_PollRx(void);
static void Serial_ProcessLine(const char *line);
static bool Serial_ParseInt32(const char *s, int32_t *value);

static void Serial_CommandMoveDegrees(int32_t degrees);
static void Serial_CommandSetFrequency(uint32_t hz);
static void Serial_CommandEmergencyStop(void);
static void Serial_CommandSetAbsolute(void);
static void Serial_CommandSetRelative(void);

/* ========================= API pública ========================= */

void Serial_Init(motor_status_t *motor)
{
    s_motor = motor;
    s_rx_index = 0;
    s_position_report_pending = false;
}

void Serial_Task(void)
{
    Serial_PollRx();

    if (s_position_report_pending)
    {
        s_position_report_pending = false;
        Serial_WritePosition();
    }
}

void Serial_Timer100msISR(void)
{
    s_position_report_pending = true;
}

/* ========================= Implementación ========================= */

static void Serial_WriteChar(char c)
{
    EUSART1_Write((uint8_t)c);
}

static void Serial_WriteString(const char *s)
{
    while (*s != '\0')
    {
        Serial_WriteChar(*s);
        s++;
    }
}

static void Serial_WriteUInt32(uint32_t value)
{
    char buffer[10];
    uint8_t i = 0u;

    if (value == 0u)
    {
        Serial_WriteChar('0');
        return;
    }

    while (value > 0u)
    {
        buffer[i++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    while (i > 0u)
    {
        i--;
        Serial_WriteChar(buffer[i]);
    }
}

static void Serial_WritePosition(void)
{
    int32_t position_steps;
    int32_t steps_per_rev;
    uint32_t pos_abs;
    uint32_t integer_part;
    uint8_t decimal_part;
    int32_t deg_x10;

    if (s_motor == NULL)
    {
        return;
    }

    /* Snapshot atómico: absolute_position es int32_t y lo toca la ISR del NCO */
    INTERRUPT_GlobalInterruptDisable();
    position_steps = s_motor->absolute_position;
    steps_per_rev = MotorGetStepsPerOutputRevolution(s_motor);
    INTERRUPT_GlobalInterruptEnable();

    if (steps_per_rev <= 0)
    {
        Serial_WriteString("P0.0\n");
        return;
    }

    deg_x10 = (int32_t)(((int32_t)position_steps * 3600LL) / (int32_t)steps_per_rev);

    Serial_WriteChar('P');

    if (deg_x10 < 0)
    {
        Serial_WriteChar('-');
        pos_abs = (uint32_t)(-deg_x10);
    }
    else
    {
        pos_abs = (uint32_t)deg_x10;
    }

    integer_part = pos_abs / 10u;
    decimal_part = (uint8_t)(pos_abs % 10u);

    Serial_WriteUInt32(integer_part);
    Serial_WriteChar('.');
    Serial_WriteChar((char)('0' + decimal_part));
    Serial_WriteChar('\n');
}

static void Serial_PollRx(void)
{
    char c;

    while (EUSART1_is_rx_ready())
    {
        c = (char)EUSART1_Read();

        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            s_rx_line[s_rx_index] = '\0';
            Serial_ProcessLine(s_rx_line);
            s_rx_index = 0u;
            continue;
        }

        if (s_rx_index < (SERIAL_RX_LINE_SIZE - 1u))
        {
            s_rx_line[s_rx_index++] = c;
        }
        else
        {
            /* overflow de línea: descartar comando actual */
            s_rx_index = 0u;
            Serial_WriteString("ERR\n");
        }
    }
}

static void Serial_ProcessLine(const char *line)
{
    int32_t value;

    if ((line == NULL) || (line[0] == '\0'))
    {
        return;
    }

    switch (line[0])
    {
        case 'M':
            if (Serial_ParseInt32(&line[1], &value))
            {
                Serial_CommandMoveDegrees(value);
            }
            else
            {
                Serial_WriteString("ERR\n");
            }
            break;

        case 'F':
            if (Serial_ParseInt32(&line[1], &value) && (value > 0))
            {
                Serial_CommandSetFrequency((uint32_t)value);
            }
            else
            {
                Serial_WriteString("ERR\n");
            }
            break;

        case 'S':
            Serial_CommandEmergencyStop();
            break;

        case 'A':
            Serial_CommandSetAbsolute();
            break;

        case 'R':
            Serial_CommandSetRelative();
            break;

        default:
            Serial_WriteString("ERR\n");
            break;
    }
}

static bool Serial_ParseInt32(const char *s, int32_t *value)
{
    bool negative = false;
    int32_t result = 0;

    if ((s == NULL) || (value == NULL))
    {
        return false;
    }

    while ((*s == ' ') || (*s == '\t'))
    {
        s++;
    }

    if (*s == '-')
    {
        negative = true;
        s++;
    }
    else if (*s == '+')
    {
        s++;
    }

    if ((*s < '0') || (*s > '9'))
    {
        return false;
    }

    while ((*s >= '0') && (*s <= '9'))
    {
        result = (result * 10) + (int32_t)(*s - '0');
        s++;
    }

    while ((*s == ' ') || (*s == '\t'))
    {
        s++;
    }

    if (*s != '\0')
    {
        return false;
    }

    *value = negative ? -result : result;
    return true;
}

static void Serial_CommandMoveDegrees(int32_t degrees)
{
    if (s_motor == NULL)
    {
        return;
    }

    /*
     * target y absolute_position son compartidos con la ISR del NCO,
     * así que conviene hacer esta secuencia en sección crítica.
     */
    INTERRUPT_GlobalInterruptDisable();
    MotorSetTarget(s_motor, degrees);
    MotorMoveToSteps(s_motor);
    MotorSetMoving(s_motor, s_motor->moving);
    INTERRUPT_GlobalInterruptEnable();

    Serial_WriteString("OK\n");
}

static void Serial_CommandSetFrequency(uint32_t hz)
{
    if (s_motor == NULL)
    {
        return;
    }

    INTERRUPT_GlobalInterruptDisable();
    Motor_SetStepRateHz(s_motor, hz);
    INTERRUPT_GlobalInterruptEnable();

    Serial_WriteString("OK\n");
}

static void Serial_CommandEmergencyStop(void)
{
    if (s_motor == NULL)
    {
        return;
    }

    INTERRUPT_GlobalInterruptDisable();
    MotorEmergencyStop(s_motor);
    INTERRUPT_GlobalInterruptEnable();

    Serial_WriteString("STOP\n");
}

static void Serial_CommandSetAbsolute(void)
{
    if (s_motor == NULL)
    {
        return;
    }

    s_motor->movement_type = MV_ABSOLUTE;
    Serial_WriteString("ABS\n");
}

static void Serial_CommandSetRelative(void)
{
    if (s_motor == NULL)
    {
        return;
    }

    s_motor->movement_type = MV_Relative;
    Serial_WriteString("REL\n");
}