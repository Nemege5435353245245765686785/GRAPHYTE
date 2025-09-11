#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>   // for kill()
#include <sys/types.h>
#include <sys/wait.h>

#define SHM_NAME "/lcd_custom_chars"
#define WIDTH 80
#define HEIGHT 16
#define FONT_W 10
#define FONT_H 16
#define ASCII_START 32
#define ASCII_END 126
#define PID_FILE "/tmp/lcd_driver.pid"

unsigned char (*CUSTOM_CHARS_SETS)[8][8];
unsigned char pixels[HEIGHT][WIDTH];
unsigned char font[ASCII_END-ASCII_START+1][FONT_H][FONT_W];
int debug_flag = 0;

// -----------------------------
// Load font file
int load_font_file(const char *filename){
    FILE *f = fopen(filename,"r");
    if(!f){ perror("fopen"); return -1; }
    char line[256];
    int ch=-1,row=0;
    while(fgets(line,sizeof(line),f)){
        if(strncmp(line,"CHAR ",5)==0){ ch=atoi(line+5); row=0; }
        else if(ch>=ASCII_START && ch<=ASCII_END && row<FONT_H){
            for(int col=0; col<FONT_W; col++)
                font[ch-ASCII_START][row][col]=(line[col]=='1')?1:0;
            row++;
        }
    }
    fclose(f);
    return 0;
}

// -----------------------------
// Draw text
void draw_text(int x0,int y0,const char *msg){
    for(int i=0; msg[i]; i++){
        char ch = msg[i];
        if(ch<ASCII_START || ch>ASCII_END) continue;
        int x_offset = x0 + i*(FONT_W+1); // 1px spacing
        for(int row=0; row<FONT_H; row++){
            for(int col=0; col<FONT_W; col++){
                int px = x_offset + col, py = y0 + row;
                if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT)
                    pixels[py][px]=font[ch-ASCII_START][row][col];
            }
        }
    }
}

// -----------------------------
// Clear display
void clear_display(){
    for(int y=0;y<HEIGHT;y++)
        for(int x=0;x<WIDTH;x++)
            pixels[y][x]=0;
}

// -----------------------------
// Line and inverted line
void draw_line(int x0,int y0,int x1,int y1){
    int dx=abs(x1-x0), sx=(x0<x1)?1:-1;
    int dy=-abs(y1-y0), sy=(y0<y1)?1:-1;
    int err=dx+dy,e2;
    while(1){
        if(x0>=0 && x0<WIDTH && y0>=0 && y0<HEIGHT) pixels[y0][x0]=1;
        if(x0==x1 && y0==y1) break;
        e2=2*err;
        if(e2>=dy){ err+=dy; x0+=sx; }
        if(e2<=dx){ err+=dx; y0+=sy; }
    }
}
void invert_line(int x0,int y0,int x1,int y1){
    int dx=abs(x1-x0), sx=(x0<x1)?1:-1;
    int dy=-abs(y1-y0), sy=(y0<y1)?1:-1;
    int err=dx+dy,e2;
    while(1){
        if(x0>=0 && x0<WIDTH && y0>=0 && y0<HEIGHT) pixels[y0][x0]^=1;
        if(x0==x1 && y0==y1) break;
        e2=2*err;
        if(e2>=dy){ err+=dy; x0+=sx; }
        if(e2<=dx){ err+=dx; y0+=sy; }
    }
}

// -----------------------------
// Rectangles
void draw_rect(int x,int y,int w,int h){
    for(int i=0;i<w;i++){
        if(x+i>=0 && x+i<WIDTH){
            if(y>=0 && y<HEIGHT) pixels[y][x+i]=1;
            if(y+h-1>=0 && y+h-1<HEIGHT) pixels[y+h-1][x+i]=1;
        }
    }
    for(int i=0;i<h;i++){
        if(y+i>=0 && y+i<HEIGHT){
            if(x>=0 && x<WIDTH) pixels[y+i][x]=1;
            if(x+w-1>=0 && x+w-1<WIDTH) pixels[y+i][x+w-1]=1;
        }
    }
}
void invert_rect(int x,int y,int w,int h){
    for(int i=0;i<w;i++){
        if(x+i>=0 && x+i<WIDTH){
            if(y>=0 && y<HEIGHT) pixels[y][x+i]^=1;
            if(y+h-1>=0 && y+h-1<HEIGHT) pixels[y+h-1][x+i]^=1;
        }
    }
    for(int i=0;i<h;i++){
        if(y+i>=0 && y+i<HEIGHT){
            if(x>=0 && x<WIDTH) pixels[y+i][x]^=1;
            if(x+w-1>=0 && x+w-1<WIDTH) pixels[y+i][x+w-1]^=1;
        }
    }
}
void draw_filled_rect(int x,int y,int w,int h){
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++){
            int px=x+j, py=y+i;
            if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT)
                pixels[py][px]=1;
        }
}
void invert_filled_rect(int x,int y,int w,int h){
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++){
            int px=x+j, py=y+i;
            if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT)
                pixels[py][px]^=1;
        }
}

// -----------------------------
// Circles
void draw_circle(int xc,int yc,int r){
    int x=0,y=r,d=1-r;
    while(x<=y){
        int pts[8][2]={{xc+x,yc+y},{xc-x,yc+y},{xc+x,yc-y},{xc-x,yc-y},
                        {xc+y,yc+x},{xc-y,yc+x},{xc+y,yc-x},{xc-y,yc-x}};
        for(int i=0;i<8;i++){
            int px=pts[i][0], py=pts[i][1];
            if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT) pixels[py][px]=1;
        }
        if(d<0) d+=2*x+3;
        else { d+=2*(x-y)+5; y--; }
        x++;
    }
}
void invert_circle(int xc,int yc,int r){
    int x=0,y=r,d=1-r;
    while(x<=y){
        int pts[8][2]={{xc+x,yc+y},{xc-x,yc+y},{xc+x,yc-y},{xc-x,yc-y},
                        {xc+y,yc+x},{xc-y,yc+x},{xc+y,yc-x},{xc-y,yc-x}};
        for(int i=0;i<8;i++){
            int px=pts[i][0], py=pts[i][1];
            if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT) pixels[py][px]^=1;
        }
        if(d<0) d+=2*x+3;
        else { d+=2*(x-y)+5; y--; }
        x++;
    }
}
void draw_filled_circle(int xc,int yc,int r){
    for(int y=-r;y<=r;y++){
        for(int x=-r;x<=r;x++){
            if(x*x + y*y <= r*r){
                int px=xc+x, py=yc+y;
                if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT)
                    pixels[py][px]=1;
            }
        }
    }
}
void invert_filled_circle(int xc,int yc,int r){
    for(int y=-r;y<=r;y++){
        for(int x=-r;x<=r;x++){
            if(x*x + y*y <= r*r){
                int px=xc+x, py=yc+y;
                if(px>=0 && px<WIDTH && py>=0 && py<HEIGHT)
                    pixels[py][px]^=1;
            }
        }
    }
}

// -----------------------------
// Update shared memory
void update_custom_chars(){
    for(int set=0; set<4; set++)
        for(int ch=0; ch<8; ch++)
            for(int row=0; row<8; row++){
                unsigned char b=0;
                for(int col=0; col<5; col++){
                    int x=0,y=0;
                    if(set==0){ x=(ch*10+col+70)%80; y=row; }
                    if(set==1){ x=(ch*10+5+col+70)%80; y=row; }
                    if(set==2){ x=(ch*10+col+70)%80; y=row+8; }
                    if(set==3){ x=(ch*10+5+col+70)%80; y=row+8; }
                    if(pixels[y][x]) b|=(1<<(4-col));
                }
                CUSTOM_CHARS_SETS[set][ch][row]=b;
            }
}

// Load pixels from shared memory
void load_pixels_from_custom_chars(){
    memset(pixels,0,sizeof(pixels));
    for(int set=0; set<4; set++)
        for(int ch=0; ch<8; ch++)
            for(int row=0; row<8; row++){
                unsigned char b = CUSTOM_CHARS_SETS[set][ch][row];
                for(int col=0; col<5; col++){
                    int val=(b & (1<<(4-col)))?1:0;
                    int x=0,y=0;
                    if(set==0){ x=(ch*10+col+70)%80; y=row; }
                    if(set==1){ x=(ch*10+5+col+70)%80; y=row+0; }
                    if(set==2){ x=(ch*10+col+70)%80; y=row+8; }
                    if(set==3){ x=(ch*10+5+col+70)%80; y=row+8; }
                    pixels[y][x]=val;
                }
            }
}

// Debug print
void print_pixels(){
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++) putchar(pixels[y][x]?'#':'.');
        putchar('\n');
    }
}

// -----------------------------
// Scroll text
void scroll_text(const char *msg, int y, int speed_ms, int direction){
    if(!msg) return;
    int len = strlen(msg);
    int text_width = len*(FONT_W+1);
    unsigned char text_bitmap[FONT_H][text_width];

    for(int i=0;i<len;i++){
        char ch = msg[i];
        if(ch<ASCII_START||ch>ASCII_END) continue;
        for(int row=0;row<FONT_H;row++)
            for(int col=0;col<FONT_W;col++)
                text_bitmap[row][i*(FONT_W+1)+col] = font[ch-ASCII_START][row][col];
        for(int row=0; row<FONT_H; row++)
            text_bitmap[row][i*(FONT_W+1)+FONT_W]=0;
    }

    int offset = (direction==1)? WIDTH : -text_width;

    while(1){
        clear_display();
        for(int row=0; row<FONT_H; row++){
            for(int col=0; col<WIDTH; col++){
                int src_col = col - offset;
                if(src_col>=0 && src_col<text_width)
                    pixels[y+row][col] = text_bitmap[row][src_col];
                else
                    pixels[y+row][col] = 0;
            }
        }

        update_custom_chars();
        if(debug_flag) print_pixels();
        usleep(speed_ms*1000);

        if(direction==1){
            offset--;
            if(offset + text_width <= 0) break;
        } else {
            offset++;
            if(offset >= WIDTH) break;
        }
    }
}

// -----------------------------
// LCD driver control
void start_lcd_driver() {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        exit(1);
    }
    if(pid == 0) {
        execl("./lcd", "./lcd", NULL);
        perror("execl");
        exit(1);
    } else {
        FILE *f = fopen(PID_FILE, "w");
        if(f) {
            fprintf(f, "%d\n", pid);
            fclose(f);
        }
        printf("Started ./lcd with PID %d\n", pid);
    }
}
void kill_lcd_driver() {
    FILE *f = fopen(PID_FILE, "r");
    if(!f) {
        fprintf(stderr, "PID file not found. Is ./lcd running?\n");
        return;
    }
    int pid;
    if(fscanf(f, "%d", &pid) != 1) {
        fprintf(stderr, "Invalid PID file\n");
        fclose(f);
        return;
    }
    fclose(f);

    if(kill(pid, SIGTERM) == 0) {
        printf("Killed ./lcd (PID %d)\n", pid);
        unlink(PID_FILE);
    } else {
        perror("kill");
    }
}

// -----------------------------
// Main
int main(int argc,char *argv[]){
    int fd = shm_open(SHM_NAME,O_RDWR,0666);
    if(fd==-1){ perror("shm_open"); return 1; }
    CUSTOM_CHARS_SETS = mmap(NULL,sizeof(unsigned char[4][8][8]),
                             PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(CUSTOM_CHARS_SETS==MAP_FAILED){ perror("mmap"); return 1; }
    close(fd);

    load_pixels_from_custom_chars();

    char *font_file=NULL;
    char *text_str=NULL;
    int text_x=-1,text_y=-1;

    for(int i=1;i<argc;i++){
        if(strcmp(argv[i],"--debug")==0) debug_flag=1;
        else if(strcmp(argv[i],"--start-lcd")==0) {
            start_lcd_driver();
            return 0;
        }
        else if(strcmp(argv[i],"--kill-lcd")==0) {
            kill_lcd_driver();
            return 0;
        }
        else if(strcmp(argv[i],"--font")==0 && i+1<argc) font_file=argv[++i];
        else if(strcmp(argv[i],"--text")==0 && i+3<argc){
            text_x=atoi(argv[++i]);
            text_y=atoi(argv[++i]);
            text_str=argv[++i];
        }
        else if(strcmp(argv[i],"--scroll-text")==0 && i+4<argc){
            int y = atoi(argv[++i]);
            int speed = atoi(argv[++i]);
            char *msg = argv[++i];
            int dir = atoi(argv[++i]);
            if(!font_file){ fprintf(stderr,"Font required for scrolling text\n"); return 1;}
            load_font_file(font_file);
            scroll_text(msg,y,speed,dir);
        }
        else if(strcmp(argv[i],"--clear")==0) clear_display();
        else if(strcmp(argv[i],"--line")==0 && i+4<argc)
            draw_line(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--invert-line")==0 && i+4<argc)
            invert_line(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--rect")==0 && i+4<argc)
            draw_rect(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--invert-rect")==0 && i+4<argc)
            invert_rect(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--rect-filled")==0 && i+4<argc)
            draw_filled_rect(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--invert-rect-filled")==0 && i+4<argc)
            invert_filled_rect(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--circle")==0 && i+3<argc)
            draw_circle(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--invert-circle")==0 && i+3<argc)
            invert_circle(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--circle-filled")==0 && i+3<argc)
            draw_filled_circle(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
        else if(strcmp(argv[i],"--invert-circle-filled")==0 && i+3<argc)
            invert_filled_circle(atoi(argv[++i]),atoi(argv[++i]),atoi(argv[++i]));
    }

    if(font_file && load_font_file(font_file)!=0){
        fprintf(stderr,"Failed to load font\n"); return 1;
    }
    if(text_str && text_x>=0 && text_y>=0) draw_text(text_x,text_y,text_str);

    update_custom_chars();
    if(debug_flag) print_pixels();

    return 0;
}
