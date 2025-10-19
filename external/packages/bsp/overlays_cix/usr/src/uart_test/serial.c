#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <serialport>\n", argv[0]);
        return 1;
    }

    char *portname = argv[1];

    // 打开串口
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return -1;
    }

    // 配置串口
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, B115200); // 设置波特率为 115200
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                         // 禁用break处理
    tty.c_lflag = 0;                                // 不使用规范模式
    tty.c_oflag = 0;                                // 不执行输出处理
    tty.c_cc[VMIN]  = 0;                            // 读取不需要字符
    tty.c_cc[VTIME] = 5;                            // 0.5秒超时

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // 关闭流控制
    tty.c_cflag |= (CLOCAL | CREAD);                // 忽略调制解调器状态线，开启接收
    tty.c_cflag &= ~(PARENB | PARODD);              // 关闭奇偶校验
    tty.c_cflag &= ~CSTOPB;                         // 无停止位
    tty.c_cflag &= ~CRTSCTS;                        // 禁用硬件流控制

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    // 向串口写数据
    char *msg = "Hello, Serial Port!\n";
    int wlen = write(fd, msg, strlen(msg));
    if (wlen != strlen(msg)) {
        printf("Error from write: %d, %d\n", wlen, errno);
    }
    tcdrain(fd); // 等待传输完成
    printf("W: %s", msg);

    // 从串口读数据
    unsigned char buf[100];
    int total_read = 0;
    int rdlen;
    while ((rdlen = read(fd, buf + total_read, sizeof(buf) - 1 - total_read)) > 0) {
        total_read += rdlen;
        if (total_read >= sizeof(buf) - 1 || buf[total_read - 1] == '\n') {
            break;
        }
    }
    if (total_read > 0) {
        buf[total_read] = 0; // 确保字符串正确终止
        printf("R: %s", buf);
    }
    
    // 关闭串口 - 这部分代码将不会被执行，因为循环是无限的
    close(fd);
    return 0;
}
