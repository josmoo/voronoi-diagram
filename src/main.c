#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600

#define SEEDS_COUNT 10
#define SEED_MARKER_RADIUS 10
#define SEED_MARKER_COLOR 0xFF000000
#define UNDEFINED_COLOR 0x00BABABA

#define EMPTY_ORIGIN -1
#define IS_AN_ORIGIN -2

#define OUTPUT_FILE_PATH "output.ppm"

typedef struct {
    int x, y;
} Point;

typedef struct {
    uint32_t color;
    int origin_sqr_dist;
} Pixel;

static Pixel image[HEIGHT][WIDTH];
static Point seeds[SEEDS_COUNT];
 
void generate_random_seeds(void){
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < SEEDS_COUNT; ++i){
        seeds[i].x = rand() % WIDTH;
        seeds[i].y = rand() % HEIGHT;
    }
}
int sqr_dist(int x1, int y1, int x2, int y2){
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx*dx + dy*dy;
}

void fill_seed_marker(int cx, int cy, int radius, uint32_t color){
    Point start = {cx - radius, cy - radius};
    Point end = {cx + radius, cy + radius};

    for(int x = start.x; x < end.x; ++x){
        for (int y = start.y; y < end.y; ++y){
            if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT){
                continue;
            }
            if(sqr_dist(cx, cy, x, y) <= radius*radius){
                image[y][x] = (Pixel){color, IS_AN_ORIGIN};
            }
            
        }
    }
}

void fill_seed_marker_random_color(int cx, int cy, int radius){
    fill_seed_marker(cx, cy, radius, ((rand() % 0x1000000) + 0xFF000000));
}

void render_seed_markers(void){
    for(size_t i = 0; i < SEEDS_COUNT; ++i){
        fill_seed_marker_random_color(seeds[i].x, seeds[i].y, SEED_MARKER_RADIUS);
    }
}

void render_voronoi(void){
    // for(int k = WIDTH/2; k < 2; k/=2){
    //     for(int y = 0; y < HEIGHT; ++y){
    //         for(int x = 0; x < WIDTH; ++x){


    //             if(image[y][x].color == )
    //         }
    //     }
    // }
}

void fill_image(uint32_t color){
    for(size_t y = 0; y < HEIGHT; ++y){
        for(size_t x = 0; x < WIDTH; ++x){
            image[y][x] = (Pixel){color, EMPTY_ORIGIN};
        }
    }
}

void save_image_as_ppm(const char *file_path){
    FILE *f = fopen(file_path, "wb");
    if(f == NULL){
        fprintf(stderr, "ERROR: could not write into file %s.\n%s\n", file_path, strerror(errno));
    }

    fprintf(f, "P6\n%d %d 255\n", WIDTH, HEIGHT);
    for(size_t y = 0; y < HEIGHT; ++y){
        for(size_t x = 0; x < WIDTH; ++x){
            Pixel pixel = image[y][x];

            //little endian
            uint8_t bytes[3] = { 
                (pixel.color & 0x000000FF),    //R
                (pixel.color & 0x0000FF00)>>8, //G
                (pixel.color & 0x00FF0000)>>16 //B
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            assert(!ferror(f));
        }
    }
    assert(fclose(f) == 0);
}

int main(void){
    generate_random_seeds();
    fill_image(UNDEFINED_COLOR); 
    render_seed_markers();
    save_image_as_ppm(OUTPUT_FILE_PATH);
    printf("hello world");
    return 0;
}