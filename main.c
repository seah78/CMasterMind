#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CODE_LENGTH 4
#define MIN_COLOR '1'
#define MAX_COLOR '6'
#define MAX_ATTEMPTS 10
#define INPUT_SIZE 64

static void generate_secret(char secret[CODE_LENGTH + 1])
{
    int i;

    for (i = 0; i < CODE_LENGTH; i++) {
        secret[i] = (char)(MIN_COLOR + rand() % (MAX_COLOR - MIN_COLOR + 1));
    }
    secret[CODE_LENGTH] = '\0';
}

static void discard_line_remainder(void)
{
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
        /* Discard excess input so the next turn starts cleanly. */
    }
}

static int read_guess(char guess[CODE_LENGTH + 1])
{
    char input[INPUT_SIZE];
    size_t length;
    int i;

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    length = strcspn(input, "\n");
    if (input[length] == '\n') {
        input[length] = '\0';
    } else {
        discard_line_remainder();
    }

    if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
        return -1;
    }

    if (strlen(input) != CODE_LENGTH) {
        return 2;
    }

    for (i = 0; i < CODE_LENGTH; i++) {
        if (!isdigit((unsigned char)input[i]) ||
            input[i] < MIN_COLOR ||
            input[i] > MAX_COLOR) {
            return 2;
        }
        guess[i] = input[i];
    }
    guess[CODE_LENGTH] = '\0';

    return 1;
}

static void score_guess(const char secret[CODE_LENGTH + 1],
                        const char guess[CODE_LENGTH + 1],
                        int *well_placed,
                        int *misplaced)
{
    int secret_counts[MAX_COLOR - MIN_COLOR + 1] = {0};
    int guess_counts[MAX_COLOR - MIN_COLOR + 1] = {0};
    int i;

    *well_placed = 0;
    *misplaced = 0;

    for (i = 0; i < CODE_LENGTH; i++) {
        if (secret[i] == guess[i]) {
            (*well_placed)++;
        } else {
            secret_counts[secret[i] - MIN_COLOR]++;
            guess_counts[guess[i] - MIN_COLOR]++;
        }
    }

    for (i = 0; i <= MAX_COLOR - MIN_COLOR; i++) {
        *misplaced += secret_counts[i] < guess_counts[i]
                          ? secret_counts[i]
                          : guess_counts[i];
    }
}

static void print_rules(void)
{
    printf("Mastermind\n");
    printf("Devine un code de %d chiffres entre %c et %c.\n",
           CODE_LENGTH,
           MIN_COLOR,
           MAX_COLOR);
    printf("Les doublons sont possibles. Tu as %d essais.\n", MAX_ATTEMPTS);
    printf("Tape q pour quitter.\n\n");
}

int main(void)
{
    char secret[CODE_LENGTH + 1];
    char guess[CODE_LENGTH + 1];
    int attempt;

    srand((unsigned int)time(NULL));
    generate_secret(secret);
    print_rules();

    for (attempt = 1; attempt <= MAX_ATTEMPTS;) {
        int status;
        int well_placed;
        int misplaced;

        printf("Essai %d/%d > ", attempt, MAX_ATTEMPTS);
        status = read_guess(guess);

        if (status == 0) {
            printf("\nFin de la saisie. Le code etait %s.\n", secret);
            return 0;
        }
        if (status == -1) {
            printf("Partie quittee. Le code etait %s.\n", secret);
            return 0;
        }
        if (status == 2) {
            printf("Saisie invalide : entre %d chiffres de %c a %c.\n",
                   CODE_LENGTH,
                   MIN_COLOR,
                   MAX_COLOR);
            continue;
        }

        score_guess(secret, guess, &well_placed, &misplaced);
        if (well_placed == CODE_LENGTH) {
            printf("Bravo ! Code %s trouve en %d essai%s.\n",
                   secret,
                   attempt,
                   attempt == 1 ? "" : "s");
            return 0;
        }

        printf("%d bien place%s, %d mal place%s.\n",
               well_placed,
               well_placed > 1 ? "s" : "",
               misplaced,
               misplaced > 1 ? "s" : "");
        attempt++;
    }

    printf("Perdu. Le code etait %s.\n", secret);
    return 0;
}
