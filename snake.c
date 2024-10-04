#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 20
#define MAX_SIZE 400

char nap = 'd';
int prev_index;
int prev_d;
char prev_nap;
unsigned int head[2] = {5, 5};
int food[2];
char * field[20][20];

int found;

int length = 0;

int * tmp;

typedef struct {
    int **data;
    int maxSize;
    int currentSize;
} FixedSizeArray;

FixedSizeArray* NewFixedSizeArray(int maxSize) {
    FixedSizeArray *array = (FixedSizeArray *)malloc(sizeof(FixedSizeArray));
    array->data = (int **)malloc(maxSize * sizeof(int *));
    for (int i = 0; i < maxSize; i++) {
        array->data[i] = (int *)malloc(2 * sizeof(int)); // Выделение памяти для подмассива
    }
    array->maxSize = maxSize;
    array->currentSize = 0;
    return array;
}

void Push(FixedSizeArray *f, int *element) {
    if (f->currentSize < f->maxSize) {
        // Если еще есть место, просто добавляем элемент
        f->data[f->currentSize] = (int *)malloc(2 * sizeof(int)); // Выделяем память для нового элемента
        memcpy(f->data[f->currentSize], element, 2 * sizeof(int)); // Копируем элемент
        f->currentSize++;
    } else {
        // Если массив заполнен, освобождаем первый элемент
        free(f->data[0]);

        // Сдвигаем массив влево
        memmove(f->data, f->data + 1, (f->maxSize - 1) * sizeof(int *)); // Сдвигаем указатели

        // Выделяем память для нового элемента и копируем его
        f->data[f->maxSize - 1] = (int *)malloc(2 * sizeof(int)); // Выделяем память для нового элемента
        memcpy(f->data[f->maxSize - 1], element, 2 * sizeof(int)); // Копируем новый элемент
    }
}

_Bool isValid(int head[2], FixedSizeArray *f) {
    int check = 0;
    for (int b = 0; b < length; b++) {
        int *gg = f->data[f->currentSize-b-1];
        if (head[0] == gg[0] && head[1] == gg[1]) {
            check = 1;
        }
    }
    return (head[1] > 0 && head[1] < MAX_SIZE && head[0] > 0 && head[0] < MAX_SIZE && check != 1);
}

void PrintArray(FixedSizeArray *f) {
    int * tmp = malloc(2*sizeof(int));
    for (int i = 0; i < f->currentSize; i++) {
            tmp = f->data[i];
            printf("[%d,%d] ", tmp[0], tmp[1]);
    }
    printf("\n");
}

void FreeArray(FixedSizeArray *f) {
    for (int i = 0; i < f->currentSize; i++) {
        free(f->data[i]);
    }
    free(f->data);
    free(f);
}

int main() {
    srand(time(NULL));
    tmp = (int *)malloc(2 * sizeof(int));
    for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                field[i][j] = "  ";
            }
        }
    FixedSizeArray *snake_body = NewFixedSizeArray(MAX_SIZE);
    food[0] = (rand() % 18) + 1;
    food[1] = (rand() % 18) + 1;
    printf("\033c");

    for (;;) {
        if (_kbhit()) {
            nap = _getch();
        }

        if (head[0] == food[0] && head[1] == food[1]) {
            food[0] = (rand() % 18) + 1;
            food[1] = (rand() % 18) + 1;
            length++;
        }

        if (nap == 'w') { //i = y, j = x
			if (prev_nap != 's') {
				head[0] = head[0] - 1;
				prev_index = 0;
				prev_d = 0;
				prev_nap = 'w';
			} else {
				head[0] = head[0] + 1;
				prev_index = 0;
				prev_d = 1;
			}
		} else if (nap == 'a') {
			if (prev_nap != 'd') {
				head[1] = head[1] - 1;
				prev_index = 1;
				prev_d = 0;
				prev_nap = 'a';
			} else {
				head[1] = head[1] + 1;
				prev_index = 1;
				prev_d = 1;
			}
		} else if (nap == 's') {
			if (prev_nap != 'w') {
				head[0] = head[0] + 1;
				prev_index = 0;
				prev_d = 1;
				prev_nap = 's';
			} else {
				head[0] = head[0] - 1;
				prev_index = 0;
				prev_d = 0;
			}
		} else if (nap == 'd') {
			if (prev_nap != 'a') {
				head[1] = head[1] + 1;
				prev_index = 1;
				prev_d = 1;
				prev_nap = 'd';
			} else {
				head[1] = head[1] - 1;
				prev_index = 1;
				prev_d = 0;
			}
		} else {
			if (prev_d == 1) {
				head[prev_index] = head[prev_index] + 1;
			} else if (prev_d == 0) {
				head[prev_index] = head[prev_index] - 1;
			}
		}

        tmp[0] = head[0];
        tmp[1] = head[1];

		Push(snake_body, tmp);

        if (head[0] == 19 || head[0] == 0 || head[1] == 19 || head[1] == 0) {
			exit(0);
		}

        for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
                found = 0;
				for (int b = 1; b <= length; b++) {
                    size_t length = snake_body->currentSize;
					tmp = snake_body->data[length-b-1];
					if (i == tmp[0] && j == tmp[1]) {
						found = 1;
					}
				}

				if (i == 0 || i == 19 || j == 0 || j == 19) {
					field[i][j] = " *";
				} else if (i == head[0] && j == head[1]) {
					field[i][j] = " @";
				} else if (found == 1) {
					field[i][j] = " X";
				} else if (i == food[0] && j == food[1]) {
					field[i][j] = " \033[31;1;4m#\033[0m";
				} else {
					field[i][j] = "  ";
				}
			}
		}

        for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            printf("%s", field[i][j]);
        }
        printf("\n");
    }
    printf("snake_body size: %d\n", snake_body->currentSize);
    printf("snake length: %d", length);
    printf("\033[%d;%dH", 21, 30);
    printf("   ");
    printf("\033[%d;%dH", 21, 30);
    printf("%d", snake_body->data[snake_body->currentSize-1][1]);
    printf("\033[%d;%dH", 0, 0);
    usleep(111000); // Sleep for 111 milliseconds
    }
    return 0;
}
