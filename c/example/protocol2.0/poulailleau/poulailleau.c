#include "poulailleau.h"
#include <stdlib.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#include <windows.h>

void usleep(uint64_t usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif

#define PROTOCOL_VERSION 1.0

static int port_num = 0;

int getch(void)
{
#if defined(__linux__) || defined(__APPLE__)
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#elif defined(_WIN32) || defined(_WIN64)
    return _getch();
#endif
}

int kbhit(void)
{
#if defined(__linux__) || defined(__APPLE__)
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
#elif defined(_WIN32) || defined(_WIN64)
    return _kbhit();
#endif
}

void sleep_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

int dxl_open(char *serial_link, uint32_t baudrate)
{
    port_num = portHandler(serial_link);

    packetHandler();

    if (openPort(port_num))
    {
        log_info("Succeeded to open the port!\n");
    }
    else
    {
        log_error("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        getch();
        return 1;
    }

    if (setBaudRate(port_num, baudrate))
    {
        log_info("Succeeded to change the baudrate!\n");
    }
    else
    {
        log_error("Failed to change the baudrate!\n");
        printf("Press any key to terminate...\n");
        getch();
        return 1;
    }

    return 0;
}

void dxl_close(void)
{
    closePort(port_num);
}

static int _dxl_check_communication_success(void)
{
    int dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION);
    if (dxl_comm_result != COMM_SUCCESS)
    {
        char msg[1024];
        snprintf(msg, 1023, "%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
        log_error(msg);
        return 1;
    }
    else
    {
        uint8_t dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION);
        if (dxl_error)
        {
            char msg[1024];
            snprintf(msg, 1023, "%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
            log_error(msg);
            return 2;
        }
    }
    return 0;
}

void dxl_ping(int dynamixel_id)
{
    pingGetModelNum(port_num, PROTOCOL_VERSION, dynamixel_id);
    _dxl_check_communication_success();
}

void dxl_write_1byte_tx_rx(
    int dynamixel_id, int register_addr, uint8_t register_value)
{
    write1ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr,
        register_value);
    _dxl_check_communication_success();
}

uint8_t dxl_read_1byte_tx_rx(int dynamixel_id, int register_addr)
{
    uint8_t value = read1ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr);
    if (!_dxl_check_communication_success())
        return value;
    else
        return -1;
}

void dxl_write_2byte_tx_rx(
    int dynamixel_id, int register_addr, uint16_t register_value)
{
    write2ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr,
        register_value);
    _dxl_check_communication_success();
}

uint16_t dxl_read_2byte_tx_rx(int dynamixel_id, int register_addr)
{
    uint16_t value = read2ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr);
    if (!_dxl_check_communication_success())
        return value;
    else
        return -1;
}

void dxl_write_4byte_tx_rx(
    int dynamixel_id, int register_addr, uint32_t register_value)
{
    write4ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr,
        register_value);
    _dxl_check_communication_success();
}

uint32_t dxl_read_4byte_tx_rx(int dynamixel_id, int register_addr)
{
    uint32_t value = read4ByteTxRx(
        port_num,
        PROTOCOL_VERSION,
        dynamixel_id,
        register_addr);
    if (!_dxl_check_communication_success())
        return value;
    else
        return -1;
}

static void _log(char *prefix, char *msg)
{
    size_t log_len = strlen(msg) + strlen(prefix) + 5;
    char *log_message = malloc(log_len);
    snprintf(log_message, log_len, "%s %s", prefix, msg);
    puts(log_message);
    free(log_message);
}

void log_debug(char *msg)
{
    _log("[DEBUG]", msg);
}

void log_info(char *msg)
{
    _log("[INFO]", msg);
}

void log_error(char *msg)
{
    _log("[ERROR]", msg);
}
