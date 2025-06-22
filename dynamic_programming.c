#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    int id;
    int weight;
    float value;
    float ratio;
} Item;

#define MAX_CAPACITY 1000000
#define CAPACITIES 3
#define MAX_ITEM_LIMIT 320000 
const int capacities[CAPACITIES] = {10000, 100000, 1000000};

// ��ȡ��ǰʱ�䣨ms��
long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// ���������Ʒ
void generate_items(Item* items, int n) {
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = rand() % 100 + 1;
        items[i].value = (float)(rand() % 901 + 100) + (rand() % 100) / 100.0f;
        items[i].ratio = items[i].value / items[i].weight;
    }
}

// ��̬�滮��
float knapsack_dp(Item* items, int n, int capacity, int* selected, int* total_weight) {
    float* dp = (float*)calloc(capacity + 1, sizeof(float));
    int* keep = (int*)calloc(n * (capacity + 1), sizeof(int));

    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            float new_val = dp[w - items[i].weight] + items[i].value;
            if (new_val > dp[w]) {
                dp[w] = new_val;
                keep[i * (capacity + 1) + w] = 1;
            }
        }
    }

    float max_val = dp[capacity];

    memset(selected, 0, sizeof(int) * n);
    int w = capacity;
    *total_weight = 0;
    for (int i = n - 1; i >= 0; i--) {
        if (keep[i * (capacity + 1) + w]) {
            selected[i] = 1;
            w -= items[i].weight;
            *total_weight += items[i].weight;
        }
    }

    free(dp);
    free(keep);
    return max_val;
}

float knapsack_dp_simple(Item* items, int n, int capacity) {
    float* dp = (float*)calloc(capacity + 1, sizeof(float));
    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            float new_val = dp[w - items[i].weight] + items[i].value;
            if (new_val > dp[w]) {
                dp[w] = new_val;
            }
        }
    }
    float max_val = dp[capacity];
    free(dp);
    return max_val;
}

void print_excel(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value) {
    int unselected_count = 0;
    for (int i = 0; i < n; i++) {
        if (selected[i] == 0) unselected_count++;
    }

    fprintf(fp, "�㷨����̬�滮��\n");
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

void print_text(FILE* fp, int n, int capacity, float total_value, long long time_ms) {
    fprintf(fp, "��Ʒ��: %d, ��������: %d\n", n, capacity);
    fprintf(fp, "�ܼ�ֵ: %.2f\n", total_value);
    fprintf(fp, "ִ��ʱ��: %lld ms\n", time_ms);
    fprintf(fp, "------------------------\n\n");
}


int main() {
    srand(123456);

    int sizes[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
                   20000, 40000, 80000, 160000, 320000};
    int sizes_count = sizeof(sizes) / sizeof(int);

    FILE* fp_txt = fopen("dp_results.txt", "w");
    FILE* fp_xls = fopen("dp_1000.xls", "w");

    if (!fp_txt || !fp_xls) {
        printf("�ļ���ʧ�ܣ�\n");
        return 1;
    }

    for (int si = 0; si < sizes_count; si++) {
        int n = sizes[si];
        
        
        if (n > MAX_ITEM_LIMIT) {
            printf("���棺��Ʒ���� %d �������� %d����������\n", n, MAX_ITEM_LIMIT);
            continue;
        }
        
        Item* items = (Item*)malloc(n * sizeof(Item));
        int* selected = (int*)calloc(n, sizeof(int));
        generate_items(items, n);

        for (int ci = 0; ci < CAPACITIES; ci++) {
            int capacity = capacities[ci];
            int total_weight = 0;
            float total_value = 0.0f;

            long long start = current_time_ms();

            if (n <= 1000 && (capacity == 10000 || capacity == 100000)) {
                total_value = knapsack_dp(items, n, capacity, selected, &total_weight);
                long long end = current_time_ms();
                
                print_text(fp_txt, n, capacity, total_value, end - start);
                print_excel(fp_xls, items, n, selected, total_weight, capacity, total_value);
            } else {
                total_value = knapsack_dp_simple(items, n, capacity);
                long long end = current_time_ms();
                total_weight = 0;
                memset(selected, 0, sizeof(int) * n);
                
                print_text(fp_txt, n, capacity, total_value, end - start);
            }
        }

        free(items);
        free(selected);
    }

    fclose(fp_txt);
    fclose(fp_xls);
    printf("��̬�滮����ɡ�\n");
    return 0;
}
