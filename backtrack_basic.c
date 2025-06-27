#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    int weight;
    float value;
} Item;

#define MAX_N 100
#define CAPACITIES 3
const int capacities[CAPACITIES] = {500, 1000, 2000};

// ��ȡ��ǰʱ�䣨ms��
long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// ���������Ʒ
void generate_items(Item* items, int n) {
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = rand() % 100 + 1;
        items[i].value = (float)(rand() % 901 + 100) + (rand() % 100) / 100.0f;
    }
}

typedef struct {
    float value;
    int* selected;
} Solution;

// ��׼���ݷ�
void backtrack(Item* items, int level, int n, int weight, float value,
               int capacity, int* current_selected, Solution* best) {
    if (level == n) {
        if (weight <= capacity && value > best->value) {
            best->value = value;
            memcpy(best->selected, current_selected, n * sizeof(int));
        }
        return;
    }

    // ����ѡ��ǰ��Ʒ
    if (weight + items[level].weight <= capacity) {
        current_selected[level] = 1;
        backtrack(items, level + 1, n,
                  weight + items[level].weight,
                  value + items[level].value,
                  capacity, current_selected, best);
    }

    // ��ѡ��ǰ��Ʒ
    current_selected[level] = 0;
    backtrack(items, level + 1, n, weight, value, capacity, current_selected, best);
}

// �������������
float knapsack_backtrack(Item* items, int n, int capacity, int* selected, int* total_weight) {
    Solution best;
    best.value = 0.0f;
    best.selected = (int*)calloc(n, sizeof(int));
    int* current_selected = (int*)calloc(n, sizeof(int));

    backtrack(items, 0, n, 0, 0.0f, capacity, current_selected, &best);

    *total_weight = 0;
    memset(selected, 0, n * sizeof(int));
    for (int i = 0; i < n; i++) {
        if (best.selected[i]) {
            selected[i] = 1;
            *total_weight += items[i].weight;
        }
    }

    float max_val = best.value;
    free(best.selected);
    free(current_selected);
    return max_val;
}

void print_excel(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value) {
    fprintf(fp, "�㷨����׼���ݷ�\n");
    fprintf(fp, "��Ʒ��: %d, ����: %d\n", n, capacity);
    fprintf(fp, "ѡ����Ʒ������ = %d\n", total_weight);
    fprintf(fp, "ѡ����Ʒ�ܼ�ֵ = %.2f\n", total_value);
    fprintf(fp, "��ƷID\t����\t��ֵ\t״̬\n");

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t%d\t%.2f\t%s\n",
                items[i].id,
                items[i].weight,
                items[i].value,
                selected[i] ? "ѡ��" : "δѡ��");
    }

    fprintf(fp, "------------------------\n\n");
}

void print_text(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value, long long time_ms) {
    int count = 0;
    fprintf(fp, "��Ʒ��: %d, ��������: %d\n", n, capacity);
    fprintf(fp, "�ܼ�ֵ: %.2f\n", total_value);
    fprintf(fp, "������: %d\n", total_weight);
    fprintf(fp, "ִ��ʱ��: %lld ms\n", time_ms);
    fprintf(fp, "ѡ��ǰ3����Ʒ:\n");
    for (int i = 0; i < n && count < 3; i++) {
        if (selected[i]) {
            fprintf(fp, "  ID=%d, ����=%d, ��ֵ=%.2f\n", items[i].id, items[i].weight, items[i].value);
            count++;
        }
    }
    if (count == 0) {
        fprintf(fp, "  ��ѡ����Ʒ\n");
    }
    fprintf(fp, "------------------------\n\n");
}

int main() {
    srand((unsigned int)time(NULL));

    int sizes[] = {5, 10, 15, 20, 25, 30};
    int sizes_count = sizeof(sizes) / sizeof(int);

    FILE* fp_txt = fopen("backtrack_basic_results.txt", "w");
    FILE* fp_xls = fopen("backtrack_basic_30.xls", "w");
    if (!fp_txt || !fp_xls) {
        printf("�ļ���ʧ�ܣ�\n");
        return 1;
    }

    for (int si = 0; si < sizes_count; si++) {
        int n = sizes[si];
        Item* items = (Item*)malloc(n * sizeof(Item));
        int* selected = (int*)calloc(n, sizeof(int));
        generate_items(items, n);

        for (int ci = 0; ci < CAPACITIES; ci++) {
            int capacity = capacities[ci];
            int total_weight = 0;

            long long start = current_time_ms();
            float total_value = knapsack_backtrack(items, n, capacity, selected, &total_weight);
            long long end = current_time_ms();

            print_text(fp_txt, items, n, selected, total_weight, capacity, total_value, end - start);

            if (n == 30) {
                print_excel(fp_xls, items, n, selected, total_weight, capacity, total_value);
            }
        }

        free(items);
        free(selected);
    }

    fclose(fp_txt);
    fclose(fp_xls);

    printf("��׼���ݷ���ɡ�\n");
    return 0;
}
