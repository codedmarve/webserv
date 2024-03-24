#include <stdio.h>

#define WIDTH 5
#define HEIGHT 5
#define COLLECTIBLE 3

static int g_collect= 0;

void flood_fill(char (*tab)[WIDTH], int width, int height, int x, int y) {
    if (y < 0 || y >= height || x < 0 || x >= width)
        return;
    if (tab[y][x] == 'v' || tab[y][x] == '1')
        return;
    if (tab[y][x] == 'C')
        g_collect += 1;
    tab[y][x] = 'v';
    flood_fill(tab, width, height, x, y + 1);
    flood_fill(tab, width, height, x, y - 1);
    flood_fill(tab, width, height, x - 1, y);
    flood_fill(tab, width, height, x + 1, y);
}

int main() {
    const int width = WIDTH;
    const int height = HEIGHT;
    char testGrid[HEIGHT][WIDTH] = {
        {'1', '1', '1', '1', '1'},
        {'1', '0', '1', 'C', '1'},
        {'1', '0', 'C', '1', '1'},
        {'1', '0', '0', '0', '1'},
        {'1', '1', '1', '1', '1'}
    };

    int x = 1;
    int y = 1;

    // Call flood_fill with a pointer to the first element of testGrid
    flood_fill(testGrid, width, height, x, y);

    // Print the grid after flood_fill to visualize changes
    printf("Grid after flood_fill:\n");
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            printf("%c ", testGrid[i][j]);
        }
        printf("\n");
    }

    printf("Collectibles: %d", g_collect);

    return 0;
}
