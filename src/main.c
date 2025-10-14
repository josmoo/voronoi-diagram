#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 800

#define SEEDS_COUNT 32
#define SEED_MARKER_COLOR 0xFF000000
#define UNDEFINED_COLOR 0x00BABABA

#define EMPTY_ORIGIN (Point){-1,-1}

#define OUTPUT_FILE_PATH "output.ppm"

typedef struct {
    int x, y;
} Point;

typedef struct {
    uint32_t color;
    Point origin;
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

void fill_seed_marker(int x, int y,uint32_t color){
    image[y][x] = (Pixel){color, (Point){x, y}};
}

void fill_seed_marker_random_color(int x, int y){
    fill_seed_marker(x, y, ((rand() % 0x1000000) + 0xFF000000));
}

void render_seed_markers(void){
    for(size_t i = 0; i < SEEDS_COUNT; ++i){
        fill_seed_marker_random_color(seeds[i].x, seeds[i].y);
    }
}

void render_voronoi(void){
    int k = WIDTH/2;
    while(k > 2){
        int offset = k/2;
        for(int y = offset; y < HEIGHT - offset; ++y){
            for(int x = offset; x < WIDTH - offset; ++x){
                Point neighbors[8] = {
                    {x - offset, y - offset}, {x, y - offset}, {x + offset, y - offset},
                    {x - offset, y},                           {x + offset, y},
                    {x - offset, y + offset}, {x, y + offset}, {x + offset, y + offset} 
                };
                for(int i = 0; i < 8; ++i){
                    uint32_t neighbors_color = image[neighbors[i].y][neighbors[i].x].color;
                    if(neighbors_color == UNDEFINED_COLOR){
                       continue;
                    }

                    Point neighbors_origin = image[neighbors[i].y][neighbors[i].x].origin;
                    if(image[y][x].color == UNDEFINED_COLOR){
                        image[y][x].color = neighbors_color;
                        image[y][x].origin = neighbors_origin;
                        continue;
                    }
                    int sqr_dist_to_pixel_origin = sqr_dist(x, y, image[y][x].origin.x, image[y][x].origin.y);
                    int sqr_dist_to_neighbor_origin = sqr_dist(x, y, neighbors_origin.x, neighbors_origin.y);
                    if(sqr_dist_to_neighbor_origin < sqr_dist_to_pixel_origin){
                       image[y][x].color = neighbors_color;
                       image[y][x].origin = neighbors_origin;
                       continue;
                    }
                }
            }
        }
        k = offset;
    }
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
    render_voronoi();
    save_image_as_ppm(OUTPUT_FILE_PATH);
    printf("hello world");
    return 0;
}
