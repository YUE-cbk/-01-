// backtrack.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// ��ȡ��ǰʱ��
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
        items[i].value = (float)(rand() % 901 + 100) + (rand() % 100) / 100.0;
        items[i].ratio = items[i].value / items[i].weight;
    }
}

// ����ֵ�ܶȽ�������ȽϺ���
int compare(const void* a, const void* b) {
    float r1 = ((Item*)a)->ratio;
    float r2 = ((Item*)b)->ratio;
    return (r1 < r2) - (r1 > r2);
}

// ������޺���
float bound(Item* items, int level, int n, int weight, float value, int capacity) {
    float bound_val = value;
    int remain = capacity - weight;
    for (int i = level; i < n && remain > 0; i++) {
        if (items[i].weight <= remain) {
            bound_val += items[i].value;
            remain -= items[i].weight;
        } else {
            bound_val += items[i].ratio * remain;
            remain = 0;
        }
    }
    return bound_val;
}

typedef struct {
    float value;
    int* selected;
} Solution;

// ��������
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
        backtrack(items, level + 1, n, weight + items[level].weight, value + items[level].value,
                  capacity, current_selected, best);
    }

    // ��ѡ��ǰ��Ʒ����֦�ж�
    current_selected[level] = 0;
    if (bound(items, level + 1, n, weight, value, capacity) > best->value) {
        backtrack(items, level + 1, n, weight, value, capacity, current_selected, best);
    }
}

// ̰���������û���������
float knapsack_backtrack(Item* items, int n, int capacity, int* selected, int* total_weight) {
    Item* sorted = (Item*)malloc(n * sizeof(Item));
    memcpy(sorted, items, n * sizeof(Item));
    qsort(sorted, n, sizeof(Item), compare);

    Solution best;
    best.value = 0;
    best.selected = (int*)calloc(n, sizeof(int));
    int* current_selected = (int*)calloc(n, sizeof(int));

    backtrack(sorted, 0, n, 0, 0.0f, capacity, current_selected, &best);

    // ����ѡ����Ʒ��������ӳ���ԭʼ��Ʒ˳��
    *total_weight = 0;
    memset(selected, 0, n * sizeof(int));
    for (int i = 0; i < n; i++) {
        if (best.selected[i]) {
            selected[sorted[i].id - 1] = 1;
            *total_weight += sorted[i].weight;
        }
    }

    float max_val = best.value;

    free(sorted);
    free(best.selected);
    free(current_selected);
    return max_val;
}

void print_excel(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value) {
    int all_weight = 0;
    float all_value = 0.0f;
    int unselected_count = 0;
    
    for (int i = 0; i < n; i++) {
        all_weight += items[i].weight;
        all_value += items[i].value;
        if (selected[i] == 0) {
            unselected_count++;
        }
    }
    
    fprintf(fp, "�㷨�����ݷ�\n");
    fprintf(fp, "��Ʒ��: %d, ����: %d\n", n, capacity);
    fprintf(fp, "ѡ����Ʒ������ = %d\n", total_weight);
    fprintf(fp, "ѡ����Ʒ�ܼ�ֵ = %.2f\n", total_value);
    fprintf(fp, "δѡ����Ʒ���� = %d\n", unselected_count);
    fprintf(fp, "��ƷID\t����\t��ֵ\t״̬\n");

    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t%d\t%.2f\t%s\n",
                items[i].id,
                items[i].weight,
                items[i].value,
                selected[i] ? "ѡ��" : "δѡ��");
    }

    fprintf(fp, "------------------------\n");
    fprintf(fp, "д����ɣ������ %d ����Ʒ��\n", n);
    fflush(fp);
}


// �ı��������
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

    int sizes[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 40000, 80000, 160000, 320000};
    int sizes_count = sizeof(sizes) / sizeof(int);

    FILE* fp_txt = fopen("backtrack_results.txt", "w");
    FILE* fp_xls = fopen("backtrack_1000.xls", "w");
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

            // ���ݷ�����
            long long start = current_time_ms();
            float total_value = 0.0f;
            // ���ƻ��ݷ���ģ���ⳬʱ
            if (n <= 40000) {
                total_value = knapsack_backtrack(items, n, capacity, selected, &total_weight);
            } else {
                fprintf(fp_txt, "��Ʒ��=%d, ����=%d �������ݷ���������ģ���ƣ�\n\n", n, capacity);
                total_value = 0.0f;
                total_weight = 0;
                memset(selected, 0, n * sizeof(int));
            }
            long long end = current_time_ms();

            print_text(fp_txt, items, n, selected, total_weight, capacity, total_value, end - start);
            if (n == 1000 && total_value > 0) {
                print_excel(fp_xls, items, n, selected, total_weight, capacity, total_value);
            }
        }

        free(items);
        free(selected);
    }

    fclose(fp_txt);
    fclose(fp_xls);

    printf("���ݷ���ɡ�\n");
    return 0;
}
