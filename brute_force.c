#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
    long long current_time_ms() {
        return GetTickCount();
    }
#else
    long long current_time_ms() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
#endif

typedef struct {
    int id;
    int weight;
    float value;
    float ratio;
} Item;

#define MAX_ITEMS 1000
#define CAPACITIES 3
const int capacities[CAPACITIES] = {500,1000,2000};

// ���������Ʒ
void generate_items(Item* items, int n) {
    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;
        items[i].weight = rand() % 100 + 1;
        items[i].value = (float)(rand() % 901 + 100) + (rand() % 100) / 100.0;
        items[i].ratio = items[i].value / items[i].weight;
    }
}

// ��������� 01��������
float knapsack_brute(Item* items, int n, int capacity, int* selected, int* total_weight) {
    int total_combinations = 1 << n;
    float max_value = 0;
    *total_weight = 0;
    
    for (int i = 0; i < total_combinations; i++) {
        int current_weight = 0;
        float current_value = 0;
        for (int j = 0; j < n; j++) {
            if (i & (1 << j)) {
                current_weight += items[j].weight;
                current_value += items[j].value;
            }
        }
        if (current_weight <= capacity && current_value > max_value) {
            max_value = current_value;
            *total_weight = current_weight;
            for (int j = 0; j < n; j++) {
                selected[j] = (i & (1 << j)) ? 1 : 0;
            }
        }
    }
    return max_value;
}

// �����Excel�ļ�
void print_excel(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value) {
    int all_weight = 0, unselected_count = 0;
    float all_value = 0.0f;
    for (int i = 0; i < n; i++) {
        all_weight += items[i].weight;
        all_value += items[i].value;
        if (!selected[i]) unselected_count++;
    }
    
    fprintf(fp, "�㷨��������\n");
    fprintf(fp, "��Ʒ��: %d, ����: %d\n", n, capacity);
    fprintf(fp, "ѡ����Ʒ������ = %d\n", total_weight);
    fprintf(fp, "ѡ����Ʒ�ܼ�ֵ = %.2f\n", total_value);
    fprintf(fp, "δѡ����Ʒ���� = %d\n", unselected_count);
    fprintf(fp, "��ƷID\t����\t��ֵ\t״̬\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\t%d\t%.2f\t%s\n",
                items[i].id, items[i].weight, items[i].value,
                selected[i] ? "ѡ��" : "δѡ��");
    }
    fprintf(fp, "------------------------\n");
    fprintf(fp, "д����ɣ������ %d ����Ʒ��\n", n);
    fflush(fp);
}

// ������ı��ļ�
void print_text(FILE* fp, Item* items, int n, int* selected, int total_weight, int capacity, float total_value, long long time_ms) {
    fprintf(fp, "��Ʒ��: %d, ��������: %d\n", n, capacity);
    fprintf(fp, "�ܼ�ֵ: %.2f\n", total_value);
    fprintf(fp, "������: %d\n", total_weight);
    fprintf(fp, "ִ��ʱ��: %lld ms\n", time_ms);
    fprintf(fp, "------------------------\n\n");
}

int main() {
    srand(123456);
    int sizes[] = {5, 10, 15, 20, 25,30};
    int sizes_count = sizeof(sizes) / sizeof(int);
   
    FILE* fp_txt = fopen("brute_results.txt", "w");
    FILE* fp_xls = fopen("brute_20.xls", "w");
    if (!fp_txt || !fp_xls) {
        printf("�ļ���ʧ�ܡ�\n");
        return 1;
    }
   
    for (int si = 0; si < sizes_count; si++) {
        int n = sizes[si];
        if (n > MAX_ITEMS) {
            fprintf(fp_txt, "���� n=%d����Ʒ�����������ֵ��\n\n", n);
            continue;
        }
   
        Item* items = (Item*)malloc(n * sizeof(Item));
        int* selected = (int*)calloc(n, sizeof(int));
        generate_items(items, n);
   
        for (int ci = 0; ci < CAPACITIES; ci++) {
            int capacity = capacities[ci];
            int total_weight = 0;
            long long start = current_time_ms();
            float total_value = knapsack_brute(items, n, capacity, selected, &total_weight);
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
    printf("������ִ����ɣ������д���ļ���\n");
    return 0;
}
