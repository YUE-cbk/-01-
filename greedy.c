#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  
#include <time.h>    

typedef struct {
    int id;
    int weight;
    float value;
    float ratio;
} Item;

#define MAX_N 320000
#define CAPACITIES 3
const int capacities[CAPACITIES] = {10000, 100000, 1000000};
#define ITERATIONS 10  

// 获取当前时间
DWORD current_time_ms() {
    return GetTickCount();  
}

// 生成随机物品
void generate_items(Item* items, int n) {
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = rand() % 100 + 1;
        items[i].value = (float)(rand() % 901 + 100) + (rand() % 100) / 100.0;
        items[i].ratio = items[i].value / items[i].weight;
    }
}

// 按价值密度排序
int compare(const void* a, const void* b) {
    float r1 = ((Item*)a)->ratio;
    float r2 = ((Item*)b)->ratio;
    return (r1 < r2) - (r1 > r2);
}

// 贪心算法
float knapsack_greedy(Item* items, int n, int capacity, int* selected, int* total_weight) {
    Item* sorted = (Item*)malloc(n * sizeof(Item));
    if (sorted == NULL) {
        perror("内存分配失败");
        exit(1);
    }
    memcpy(sorted, items, n * sizeof(Item));
    qsort(sorted, n, sizeof(Item), compare);

    memset(selected, 0, n * sizeof(int));
    float total_value = 0;
    *total_weight = 0;
    int current_weight = 0;

    for (int i = 0; i < n; i++) {
        int id = sorted[i].id - 1;
        if (current_weight + sorted[i].weight <= capacity) {
            selected[id] = 1;
            current_weight += sorted[i].weight;
            total_value += sorted[i].value;
            *total_weight += sorted[i].weight;
        }
    }

    free(sorted);
    return total_value;
}

// 输出文本文件
void print_text(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value, double elapsed_ms) {
    int count = 0;
    fprintf(fp, "物品数: %d, 背包容量: %d\n", n, capacity);
    fprintf(fp, "总价值: %.2f\n", total_value);
    fprintf(fp, "总重量: %d\n", total_weight);
    fprintf(fp, "执行时间: %.2f ms\n", elapsed_ms);
    fprintf(fp, "选中前3个物品:\n");
    for (int i = 0; i < n && count < 3; i++) {
        if (selected[i]) {
            fprintf(fp, "  ID=%d, 重量=%d, 价值=%.2f\n", items[i].id, items[i].weight, items[i].value);
            count++;
        }
    }
    if (count == 0) {
        fprintf(fp, "  无选中物品\n");
    }
    fprintf(fp, "------------------------\n\n");
}

int main() {
    srand((unsigned int)time(NULL));  // 初始化随机数种子

    int sizes[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 40000, 80000, 160000, 320000};
    int sizes_count = sizeof(sizes) / sizeof(int);

    FILE* fp_txt = fopen("greedy_results.txt", "w");
    if (fp_txt == NULL) {
        perror("无法打开输出文件");
        return 1;
    }

    for (int si = 0; si < sizes_count; si++) {
        int n = sizes[si];
        Item* items = (Item*)malloc(n * sizeof(Item));
        int* selected = (int*)calloc(n, sizeof(int));
        if (items == NULL || selected == NULL) {
            perror("内存分配失败");
            free(items);
            free(selected);
            fclose(fp_txt);
            return 1;
        }

        generate_items(items, n);  // 生成物品

        for (int ci = 0; ci < CAPACITIES; ci++) {
            int capacity = capacities[ci];
            int total_weight = 0;
            DWORD total_elapsed_ms = 0;

            // 多次迭代取平均，减少计时波动
            for (int iter = 0; iter < ITERATIONS; iter++) {
                memset(selected, 0, n * sizeof(int));  

                DWORD start = current_time_ms();
                float total_value = knapsack_greedy(items, n, capacity, selected, &total_weight);
                DWORD end = current_time_ms();

                total_elapsed_ms += (end - start);
            }

            double avg_elapsed_ms = (double)total_elapsed_ms / ITERATIONS;
           
            print_text(fp_txt, items, n, selected, total_weight, capacity, 
                      knapsack_greedy(items, n, capacity, selected, &total_weight), 
                      avg_elapsed_ms);
        }

        free(items);
        free(selected);
    }

    fclose(fp_txt);
    printf("贪心法执行完成，结果已写入 greedy_results.txt\n");
    return 0;
}
