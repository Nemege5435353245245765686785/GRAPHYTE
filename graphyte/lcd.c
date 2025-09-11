#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// GPIO pin mapping (BCM numbering)
#define LCD_RS 17
#define LCD_E  27
#define LCD_D4 22
#define LCD_D5 10
#define LCD_D6  9
#define LCD_D7 11

// Commands
#define LCD_CHR 1 // Sending data
#define LCD_CMD 0 // Sending command

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0

// Timing
#define E_PULSE 38000L
#define E_DELAY 1L
#define INTERLACE_DELAY 31000000L

// Shared memory name
#define SHM_NAME "/lcd_custom_chars"

// Pointer to shared memory
unsigned char (*CUSTOM_CHARS_SETS)[8][8];

// Busy-wait sleep in nanoseconds
void busy_sleep(long ns) {
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec - start.tv_sec) * 1000000000L +
             (now.tv_nsec - start.tv_nsec) < ns);
}

void lcd_toggle_enable(void) {
    digitalWrite(LCD_E, HIGH);
    busy_sleep(E_PULSE);
    digitalWrite(LCD_E, LOW);
    busy_sleep(E_DELAY);
}

void lcd_byte(int bits, int mode) {
    digitalWrite(LCD_RS, mode);

    // High nibble
    digitalWrite(LCD_D4, (bits & 0x10) ? 1 : 0);
    digitalWrite(LCD_D5, (bits & 0x20) ? 1 : 0);
    digitalWrite(LCD_D6, (bits & 0x40) ? 1 : 0);
    digitalWrite(LCD_D7, (bits & 0x80) ? 1 : 0);
    lcd_toggle_enable();

    // Low nibble
    digitalWrite(LCD_D4, (bits & 0x01) ? 1 : 0);
    digitalWrite(LCD_D5, (bits & 0x02) ? 1 : 0);
    digitalWrite(LCD_D6, (bits & 0x04) ? 1 : 0);
    digitalWrite(LCD_D7, (bits & 0x08) ? 1 : 0);
    lcd_toggle_enable();
}

void lcd_clear(void) {
    for (int i = 0; i < 80; i++) {
        lcd_byte(' ', LCD_CHR);
    }
}

void lcd_clear_cmd(void) {
    lcd_byte(0x01, LCD_CMD);
    busy_sleep(2000000L);
}

void lcd_init(void) {
    wiringPiSetupGpio();
    pinMode(LCD_E,  OUTPUT);
    pinMode(LCD_RS, OUTPUT);
    pinMode(LCD_D4, OUTPUT);
    pinMode(LCD_D5, OUTPUT);
    pinMode(LCD_D6, OUTPUT);
    pinMode(LCD_D7, OUTPUT);

    lcd_byte(0x33, LCD_CMD);
    lcd_byte(0x32, LCD_CMD);
    lcd_byte(0x28, LCD_CMD);
    lcd_byte(0x0C, LCD_CMD);
    lcd_byte(0x06, LCD_CMD);
    lcd_clear();
}

void lcd_string(const char *message) {
    for (int i = 0; i < strlen(message); i++) {
        lcd_byte(message[i], LCD_CHR);
    }
}

void lcd_create_char(int location, unsigned char charmap[8], int prevline) {
    location &= 0x7;
    lcd_byte(0x40 | (location << 3), LCD_CMD);
    for (int i = 0; i < 8; i++) {
        lcd_byte(charmap[i], LCD_CHR);
    }
    lcd_byte(prevline, LCD_CMD);
}

int main() {
    // --- Set up shared memory ---
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }
    ftruncate(fd, sizeof(unsigned char[4][8][8]));
    CUSTOM_CHARS_SETS = mmap(NULL, sizeof(unsigned char[4][8][8]),
                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (CUSTOM_CHARS_SETS == MAP_FAILED) { perror("mmap"); return 1; }
    close(fd);

    // Optional: initialize default chars if empty
    static unsigned char default_chars[4][8][8] = {
        { {0} }, { {0} }, { {0} }, { {0} }
    };
    memcpy(CUSTOM_CHARS_SETS, default_chars, sizeof(default_chars));

    lcd_init();
    lcd_clear_cmd();

    int frame_index = 0;
    int prev_line = LCD_LINE_1;
    struct timespec last_update, now;
    clock_gettime(CLOCK_MONOTONIC, &last_update);

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed = (now.tv_sec - last_update.tv_sec) * 1000000000L +
                       (now.tv_nsec - last_update.tv_nsec);

        if (elapsed >= INTERLACE_DELAY) {
            lcd_clear();
            for (int i = 0; i < 8; i++) {
                lcd_create_char(i, CUSTOM_CHARS_SETS[frame_index][i], prev_line);
            }

            char buf[32] = "";
            if (frame_index == 0 || frame_index == 2) {
                for (int i = 1; i <= 9; i++)
                    sprintf(buf + strlen(buf), "%c ", i);
            } else {
                for (int i = 1; i <= 9; i++)
                    sprintf(buf + strlen(buf), " %c", i);

                prev_line = (frame_index == 1) ? LCD_LINE_2 : LCD_LINE_1;
            }
            lcd_string(buf);

            frame_index = (frame_index + 1) % 4;
            last_update = now;
        }
    }

    return 0;
}
