#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 파라미터
#define LEN_MIN 15 // 기차 길이
#define LEN_MAX 50
#define STM_MIN 0 // 마동석 체력
#define STM_MAX 5
#define PROB_MIN 10 // 확률
#define PROB_MAX 90
#define AGGRO_MIN 0 // 어그로 범위
#define AGGRO_MAX 5

// 마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

void print_intro() {
    printf("\n********************************************\n");
    printf("*                                          *\n");
    printf("*       부산헹  게임을  시작합니다.        *\n");
    printf("*                                          *\n");
    printf("********************************************\n\n");
}

void get_input(int* trainLength, int* stamina, int* probability) {
    do {
        printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf_s("%d", trainLength);
    } while (*trainLength < LEN_MIN || *trainLength > LEN_MAX);

    do {
        printf("madongseok stamina(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf_s("%d", stamina);
    } while (*stamina < STM_MIN || *stamina > STM_MAX);

    do {
        printf("percentile probability 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
        scanf_s("%d", probability);
    } while (*probability < PROB_MIN || *probability > PROB_MAX);
}

void print_train(int trainLength, int citizenPos, int zombiePos, int madongseokPos) {
    for (int i = 0; i < trainLength; ++i) printf("#");
    printf("\n#");
    for (int i = 1; i < trainLength - 1; ++i) {
        if (i == citizenPos) printf("C");
        else if (i == zombiePos) printf("Z");
        else if (i == madongseokPos) printf("M");
        else printf(" ");
    }
    printf("#\n");
    for (int i = 0; i < trainLength; ++i) printf("#");
    printf("\n\n");
}

int move_citizen(int citizenPos, int probability, int* citizenAggro) {
    if (citizenPos > 1 && rand() % 100 >= probability) {
        (*citizenAggro)++;
        printf("citizen: %d -> %d (aggro: %d)\n", citizenPos, citizenPos - 1, *citizenAggro);
        return citizenPos - 1;
    }
    (*citizenAggro)--;
    printf("citizen: stay %d (aggro: %d)\n", citizenPos, *citizenAggro);
    return citizenPos;
}

int move_zombie(int zombiePos, int citizenPos, int madongseokPos, int turnCount, int citizenAggro, int madongseokAggro) {
    if (turnCount % 2 == 0 && zombiePos > 1) {
        if (citizenAggro >= madongseokAggro) {
            printf("zombie: %d -> %d\n", zombiePos, zombiePos - 1);
            return zombiePos - 1;
        }
        else {
            printf("zombie: stay %d\n", zombiePos);
            return zombiePos;
        }
    }
    printf("zombie: stay %d (cannot move)\n", zombiePos);
    return zombiePos;
}

int get_madongseok_move() {
    int move;
    do {
        printf("madongseok move(0:stay, 1:left)>> ");
        scanf_s("%d", &move);
    } while (move != MOVE_STAY && move != MOVE_LEFT);
    return move;
}

int get_madongseok_action(int isAdjacent) {
    int action;
    if (isAdjacent) {
        do {
            printf("madongseok action(0:rest, 1:provoke, 2:pull)>> ");
            scanf_s("%d", &action);
        } while (action != ACTION_REST && action != ACTION_PROVOKE && action != ACTION_PULL);
    }
    else {
        do {
            printf("madongseok action(0:rest, 1:provoke)>> ");
            scanf_s("%d", &action);
        } while (action != ACTION_REST && action != ACTION_PROVOKE);
    }
    return action;
}

void print_madongseok(int madongseokPos, int madongseokAggro, int stamina) {
    printf("madongseok: stay %d (aggro: %d, stamina: %d)\n", madongseokPos, madongseokAggro, stamina);
}

void print_outro(const char* message) {
    printf("\n********************************************\n");
    printf("*                                          *\n");
    printf("%s\n", message);
    printf("*                                          *\n");
    printf("********************************************\n\n");
}

int main() {
    int trainLength, probability, stamina;
    int citizenPos, zombiePos, madongseokPos, citizenAggro = 1, madongseokAggro = 1;
    int turnCount = 1;

    srand((unsigned int)time(NULL));
    print_intro();
    get_input(&trainLength, &stamina, &probability);

    citizenPos = trainLength - 8;
    zombiePos = trainLength - 5;
    madongseokPos = trainLength - 3;

    print_train(trainLength, citizenPos, zombiePos, madongseokPos);

    while (1) {
        // 이동 페이즈
        citizenPos = move_citizen(citizenPos, probability, &citizenAggro);
        zombiePos = move_zombie(zombiePos, citizenPos, madongseokPos, turnCount, citizenAggro, madongseokAggro);

        int madongseokMove = get_madongseok_move();
        if (madongseokMove == MOVE_LEFT && madongseokPos > 1) {
            madongseokPos--;
            madongseokAggro++;
            printf("madongseok: %d -> %d (aggro: %d, stamina: %d)\n", madongseokPos + 1, madongseokPos, madongseokAggro, stamina);
        }
        else {
            madongseokAggro--;
            print_madongseok(madongseokPos, madongseokAggro, stamina);
        }

        print_train(trainLength, citizenPos, zombiePos, madongseokPos);

        // 행동 페이즈
        printf("citizen does nothing.\n");

        int attackTarget = ATK_NONE;
        if (abs(zombiePos - citizenPos) == 1 && abs(zombiePos - madongseokPos) == 1) {
            attackTarget = (citizenAggro >= madongseokAggro) ? ATK_CITIZEN : ATK_DONGSEOK;
        }
        else if (abs(zombiePos - citizenPos) == 1) {
            attackTarget = ATK_CITIZEN;
        }
        else if (abs(zombiePos - madongseokPos) == 1) {
            attackTarget = ATK_DONGSEOK;
        }

        if (attackTarget == ATK_CITIZEN) {
            printf("GAME OVER! Citizen(s) has(have) been attacked by a zombie\n");
            print_outro("*      부산헹  게임이 종료되었습니다.      *");
            break;
        }
        else if (attackTarget == ATK_DONGSEOK) {
            stamina--;
            printf("zombie attacked madongseok! (stamina: %d)\n", stamina);
            if (stamina <= STM_MIN) {
                printf("GAME OVER! Madongseok has been defeated\n");
                print_outro("*      부산헹  게임이 종료되었습니다.      *");
                break;
            }
        }
        else {
            printf("zombie attacked nobody.\n");
        }

        int isAdjacent = (abs(zombiePos - madongseokPos) == 1);
        int madongseokAction = get_madongseok_action(isAdjacent);
        if (madongseokAction == ACTION_REST) {
            madongseokAggro--;
            stamina++;
            if (madongseokAggro < AGGRO_MIN) madongseokAggro = AGGRO_MIN;
            if (stamina > STM_MAX) stamina = STM_MAX;
            printf("madongseok rests... (aggro: %d, stamina: %d)\n", madongseokAggro, stamina);
        }
        else if (madongseokAction == ACTION_PROVOKE) {
            madongseokAggro = AGGRO_MAX;
            printf("madongseok provokes... (aggro: %d, stamina: %d)\n", madongseokAggro, stamina);
        }
        else if (madongseokAction == ACTION_PULL) {
            madongseokAggro += 2;
            stamina--;
            if (madongseokAggro > AGGRO_MAX) madongseokAggro = AGGRO_MAX;
            if (stamina < STM_MIN) stamina = STM_MIN;
            int pullSuccess = rand() % 100 >= probability;
            if (pullSuccess) {
                printf("madongseok pulls... Success! (aggro: %d, stamina: %d)\n", madongseokAggro, stamina);
                // 좀비 다음 턴 이동 불가 표시 추가
            }
            else {
                printf("madongseok pulls... Failed! (aggro: %d, stamina: %d)\n", madongseokAggro, stamina);
            }
        }

        // 게임 종료 조건
        if (citizenPos == 1) {
            printf("SUCCESS!\nCitizen(s) escaped to the next train!\n");
            print_outro("*      부산헹  게임이 종료되었습니다.      *");
            break;
        }
        if (citizenPos == zombiePos || citizenPos == zombiePos + 1 || citizenPos == zombiePos - 1) {
            printf("GAME OVER!\nCitizen(s) has(have) been attacked by a zombie\n");
            print_outro("*      부산헹  게임이 종료되었습니다.      *");
            break;
        } //

        turnCount++;
    }

    return 0;
}
