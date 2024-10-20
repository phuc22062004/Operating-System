#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    // Các tham số cơ bản khi không có -n
    int argPerCommand = MAXARG;
    int cmdStart = 1;
    
    // Xử lý trường hợp có tham số -n
    if (strcmp(argv[1], "-n") == 0) {
        argPerCommand = atoi(argv[2]);
        cmdStart = 3;
    }

    // Lưu các tham số đầu vào của xargs
    char* args[MAXARG];
    char buf[1024];
    int n = 0;
    while (n < argc - cmdStart) {
        args[n] = argv[cmdStart + n];
        n++;
    }

    // Thực hiện 1 lệnh
    while (1) {
        int i = 0;
        int status = 0;

        // Đọc các tham số từ đầu vào chuẩn
        while (1) {
            status = read(0, &buf[i], 1);
            if (status < 1) {
                break;
            }
            if (buf[i] == '\n') break;
            i++;
        }
        
        // Không còn dữ liệu đầu vào (EOF)
        if (i == 0 && status < 1) break;

        // Xử lý với -n tham số cho mỗi lệnh
        int argsIdx = n;
        int startArgsIdx = 0;
        int argsCount = 0;
        int j = 0;
        while (j <= i) {
            // Tách các tham số từ đâu vào chuẩn
            if (buf[j] == ' ' || buf[j] == '\n') {
                buf[j] = 0;
                args[argsIdx++] = &buf[startArgsIdx];
                startArgsIdx = j + 1;
                argsCount++;
            }
            // Đủ tham số theo -n thì thực hiện
            if (argsCount == argPerCommand) {
                args[argsIdx] = 0;
                // Tạo ra tiến trình còn và thực hiện lệnh
                if (fork() == 0) {
                    exec(args[0], args);

                    // Nếu lệnh exec fail
                    fprintf(2, "exec %s failed\n", args[0]);
                    exit(1);
                }
                else {
                    wait(0);
                }
                argsIdx = n;
                argsCount = 0;
            }
            j++;
        }

        // Trường hợp còn thừa nhưng không đủ tham số của -n
        if (argsCount != 0) {
            args[argsIdx] = 0;
            // Tạo ra tiến trình còn và thực hiện lệnh
            if (fork() == 0) {
                exec(args[0], args);

                // Nếu lệnh exec fail
                fprintf(2, "exec %s failed\n", args[0]);
                exit(1);
            }
            else {
                wait(0);
            }
            argsIdx = n;
            argsCount = 0;
        }
    }
    // Kết thúc tiến trình hoàn thành
    exit(0);
}