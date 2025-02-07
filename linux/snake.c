#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 400

char nap = ' ';
int prev_index = 0; //я кароч хз, для меня всегда было проблемой обрабатывать нажатия клавиш, особенно в линухе. Тут для дефолтного направления в самом начале игры когда ты ещё ничего не нажал, используются индекс и действие, т.е 0 индекс это ось Y, 1 - это + соответственно змея по дефолту будет двигаться вниз (учитывая что верхний левый угол терминала это 0:0)
int prev_d = 1;
char prev_nap = 's';
unsigned int head[2] = {2, 10};
int food[2];

int found;

int length = 0;

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

char getKeyPress() {
    struct termios oldt, newt;
    int ch;
    
    // Получаем старые настройки терминала
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // Отключаем каноничный режим и эхо
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    
    // Применяем новые настройки
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // Устанавливаем stdin в неблокирующий режим
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    
    // Пробуем считать символ, если он есть
    ch = getchar();
    
    // Восстанавливаем старые настройки терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return ch;
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

// void PrintArray(FixedSizeArray *f) {
//     int * tmp = malloc(2*sizeof(int));
//     for (int i = 0; i < f->currentSize; i++) {
//             tmp = f->data[i];
//             printf("[%d,%d] ", tmp[0], tmp[1]);
//     }
//     printf("\n");
// }

// void FreeArray(FixedSizeArray *f) {
//     for (int i = 0; i < f->currentSize; i++) {
//         free(f->data[i]);
//     }
//     free(f->data);
//     free(f);
// }

int main() {
    srand(time(NULL));

    FixedSizeArray *snake_body = NewFixedSizeArray(MAX_SIZE);
    food[0] = (rand() % 18) + 1;
    food[1] = (rand() % 18) + 1;
    printf("\033c");

    int CompareWithHead(FixedSizeArray *f, unsigned int head[2]) {
    for (int i = f->currentSize - 2; i >= ((f->currentSize - 1) - length); i--) { // X X X X X
        if (f->data[i][0] == head[0] && f->data[i][1] == head[1]) {
            exit(0);
        }
    }
}

    for (;;) {

        nap = getKeyPress();

        if (head[0] == food[0] && head[1] == food[1]) {
            food[0] = (rand() % 18) + 1;
            food[1] = (rand() % 18) + 1;
            length++;
        }

        CompareWithHead(snake_body, head);

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

		Push(snake_body, head);

        if (head[0] == 19 || head[0] == 0 || head[1] == 19 || head[1] == 0) {
			exit(0);
		}

        for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 20; j++) {
                found = 0;
				for (int b = 1; b <= length; b++) {
					if (i == snake_body->data[snake_body->currentSize-b-1][0] && j == snake_body->data[snake_body->currentSize-b-1][1]) {
						found = 1;
					}
				}

				if (i == 0 || i == 19 || j == 0 || j == 19) {
					printf(" *");
				} else if (i == head[0] && j == head[1]) {
					printf(" @");
				} else if (found == 1) {
					printf(" X");
				} else if (i == food[0] && j == food[1]) {
					printf(" \033[31;1;4m#\033[0m");
				} else {
					printf("  ");
				}
			}
            printf("\n");
		}

    //printf("snake_body size: %d\n", snake_body->currentSize);
    printf("snake length: %d\n", length);
    printf("\033[%d;%dH", 0, 0);
    usleep(121000); // Sleep for 191 milliseconds

    }

    return 0;
}
