#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <poulailleau.h>

#define PROTOCOL_VERSION 2.0

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

int dxl_open(char *serial_link, uint32_t baudrate)
{
    port_num = portHandler(serial_link);

    packetHandler();

    if (openPort(port_num))
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        getch();
        return 1;
    }

    if (setBaudRate(port_num, baudrate))
    {
        printf("Succeeded to change the baudrate!\n");
    }
    else
    {
        printf("Failed to change the baudrate!\n");
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
        printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
        return 1;
    }
    else
    {
        uint8_t dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION);
        if (dxl_error)
        {
            printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
            return 2;
        }
    }
    return 0;
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