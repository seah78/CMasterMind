#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CODE_LENGTH 4
#define COLOR_COUNT 6
#define MAX_ATTEMPTS 10
#define INPUT_SIZE 64
#define ANSI_RESET "\033[0m"

typedef struct {
    char code;
    const char *name;
    const char *ansi_style;
} Color;

static const Color COLORS[COLOR_COUNT] = {
    {'R', "Rouge", "\033[41;97m"},
    {'V', "Vert", "\033[42;30m"},
    {'B', "Bleu", "\033[44;97m"},
    {'J', "Jaune", "\033[43;30m"},
    {'M', "Magenta", "\033[45;97m"},
    {'C', "Cyan", "\033[46;30m"}
};

/* Retourne l'indice de la couleur associee a une initiale. */
static int color_index(char code)
{
    int i;
    char normalized = (char)toupper((unsigned char)code);

    for (i = 0; i < COLOR_COUNT; i++) {
        if (COLORS[i].code == normalized) {
            return i;
        }
    }

    return -1;
}

/* Affiche un pion avec la couleur ANSI associee a son initiale. */
static void print_peg(char code)
{
    int index = color_index(code);

    if (index >= 0) {
        printf("%s %c %s", COLORS[index].ansi_style, COLORS[index].code, ANSI_RESET);
    } else {
        printf(" . ");
    }
}

/* Affiche une combinaison sous forme de pions colores. */
static void print_code(const char code[CODE_LENGTH + 1])
{
    int i;

    for (i = 0; i < CODE_LENGTH; i++) {
        print_peg(code[i]);
        printf(" ");
    }
}

/* Cree un code secret aleatoire compose des couleurs autorisees. */
static void generate_secret(char secret[CODE_LENGTH + 1])
{
    int i;

    for (i = 0; i < CODE_LENGTH; i++) {
        secret[i] = COLORS[rand() % COLOR_COUNT].code;
    }
    secret[CODE_LENGTH] = '\0';
}

/* Vide les caracteres restants quand une ligne depasse le tampon de lecture. */
static void discard_line_remainder(void)
{
    int character;

    while ((character = getchar()) != '\n' && character != EOF) {
        /* Ignore le surplus pour repartir sur une ligne propre au tour suivant. */
    }
}

/* Indique si la ligne saisie contient uniquement la commande q. */
static int is_quit_command(const char *input)
{
    while (isspace((unsigned char)*input)) {
        input++;
    }

    if (*input != 'q' && *input != 'Q') {
        return 0;
    }
    input++;

    while (isspace((unsigned char)*input)) {
        input++;
    }

    return *input == '\0';
}

/*
 * Lit et valide une proposition.
 * Retourne 1 pour une proposition valide, 2 pour une saisie invalide,
 * -1 quand le joueur quitte, et 0 quand l'entree se termine.
 */
static int read_guess(char guess[CODE_LENGTH + 1])
{
    char input[INPUT_SIZE];
    size_t length;
    int color_count = 0;
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

    if (is_quit_command(input)) {
        return -1;
    }

    for (i = 0; input[i] != '\0'; i++) {
        int index;

        if (isspace((unsigned char)input[i])) {
            continue;
        }

        index = color_index(input[i]);
        if (index < 0 || color_count >= CODE_LENGTH) {
            return 2;
        }
        guess[color_count] = COLORS[index].code;
        color_count++;
    }

    if (color_count != CODE_LENGTH) {
        return 2;
    }

    guess[CODE_LENGTH] = '\0';

    return 1;
}

/*
 * Calcule les pions bien places et mal places.
 * Les couleurs deja bien placees ne sont pas recomptees dans les doublons.
 */
static void score_guess(const char secret[CODE_LENGTH + 1],
                        const char guess[CODE_LENGTH + 1],
                        int *well_placed,
                        int *misplaced)
{
    int secret_counts[COLOR_COUNT] = {0};
    int guess_counts[COLOR_COUNT] = {0};
    int i;

    *well_placed = 0;
    *misplaced = 0;

    for (i = 0; i < CODE_LENGTH; i++) {
        if (secret[i] == guess[i]) {
            (*well_placed)++;
        } else {
            secret_counts[color_index(secret[i])]++;
            guess_counts[color_index(guess[i])]++;
        }
    }

    for (i = 0; i < COLOR_COUNT; i++) {
        *misplaced += secret_counts[i] < guess_counts[i]
                          ? secret_counts[i]
                          : guess_counts[i];
    }
}

/* Affiche les pions disponibles et leurs initiales de saisie. */
static void print_palette(void)
{
    int i;

    printf("Couleurs disponibles :\n");
    for (i = 0; i < COLOR_COUNT; i++) {
        printf("  ");
        print_peg(COLORS[i].code);
        printf(" %c = %s\n", COLORS[i].code, COLORS[i].name);
    }
}

/* Affiche les indices donnes apres une proposition. */
static void print_feedback(int well_placed, int misplaced)
{
    int i;

    for (i = 0; i < well_placed; i++) {
        printf("\033[40;97m X " ANSI_RESET " ");
    }
    for (i = 0; i < misplaced; i++) {
        printf("\033[47;30m O " ANSI_RESET " ");
    }
    for (i = well_placed + misplaced; i < CODE_LENGTH; i++) {
        printf(" .  ");
    }

    printf("BP:%d MP:%d", well_placed, misplaced);
}

/* Dessine le plateau avec l'historique des essais valides. */
static void print_board(char guesses[MAX_ATTEMPTS][CODE_LENGTH + 1],
                        const int well_placed[MAX_ATTEMPTS],
                        const int misplaced[MAX_ATTEMPTS],
                        int attempt_count)
{
    int i;

    printf("\n");

    /* Bordure superieure : les + marquent les coins et changements de colonne. */
    printf("+------+-----------------+----------------------------+\n");

    /* En-tete : chaque | est un separateur vertical entre les colonnes. */
    printf("| Tour | Proposition     | Indices                    |\n");

    /* Ligne horizontale qui separe l'en-tete des essais du joueur. */
    printf("+------+-----------------+----------------------------+\n");
    for (i = 0; i < MAX_ATTEMPTS; i++) {
        /* Debut de ligne : bord gauche, numero du tour et colonne suivante. */
        printf("| %2d   | ", i + 1);
        if (i < attempt_count) {
            print_code(guesses[i]);
        } else {
            print_code("....");
        }

        /* Separe la proposition des indices affiches a droite. */
        printf("| ");
        if (i < attempt_count) {
            print_feedback(well_placed[i], misplaced[i]);
        } else {
            printf("                         ");
        }

        /* Termine la ligne avec la bordure droite du tableau. */
        printf("  |\n");
    }

    /* Bordure inferieure qui ferme les trois colonnes du plateau. */
    printf("+------+------------------+---------------------------+\n");
    printf("X = bien place, O = bonne couleur mal placee.\n\n");
}

/* Affiche les regles visibles au debut de chaque partie. */
static void print_rules(void)
{
    printf("Mastermind\n");
    printf("Devine un code de %d pions colores.\n", CODE_LENGTH);
    printf("Les doublons sont possibles. Tu as %d essais.\n", MAX_ATTEMPTS);
    printf("Saisis les initiales ensemble ou separees par des espaces.\n");
    printf("Exemple : RVBJ ou R V B J. Tape q pour quitter.\n\n");
    print_palette();
    printf("\n");
}

/* Lance la partie, gere les tours et affiche le resultat final. */
int main(void)
{
    char secret[CODE_LENGTH + 1];
    char guess[CODE_LENGTH + 1];
    char guesses[MAX_ATTEMPTS][CODE_LENGTH + 1];
    int well_placed_history[MAX_ATTEMPTS] = {0};
    int misplaced_history[MAX_ATTEMPTS] = {0};
    int attempt;

    srand((unsigned int)time(NULL));
    generate_secret(secret);
    print_rules();
    print_board(guesses, well_placed_history, misplaced_history, 0);

    for (attempt = 1; attempt <= MAX_ATTEMPTS;) {
        int status;
        int well_placed;
        int misplaced;

        printf("Essai %d/%d > ", attempt, MAX_ATTEMPTS);
        status = read_guess(guess);

        if (status == 0) {
            printf("\nFin de la saisie. Le code etait : ");
            print_code(secret);
            printf("\n");
            return 0;
        }
        if (status == -1) {
            printf("Partie quittee. Le code etait : ");
            print_code(secret);
            printf("\n");
            return 0;
        }
        if (status == 2) {
            printf("Saisie invalide : entre %d couleurs parmi R V B J M C.\n",
                   CODE_LENGTH);
            continue;
        }

        score_guess(secret, guess, &well_placed, &misplaced);
        strcpy(guesses[attempt - 1], guess);
        well_placed_history[attempt - 1] = well_placed;
        misplaced_history[attempt - 1] = misplaced;
        print_board(guesses, well_placed_history, misplaced_history, attempt);

        if (well_placed == CODE_LENGTH) {
            printf("Bravo ! Code trouve en %d essai%s : ",
                   attempt,
                   attempt == 1 ? "" : "s");
            print_code(secret);
            printf("\n");
            return 0;
        }

        printf("%d bien place%s, %d mal place%s.\n\n",
               well_placed,
               well_placed > 1 ? "s" : "",
               misplaced,
               misplaced > 1 ? "s" : "");
        attempt++;
    }

    printf("Perdu. Le code etait : ");
    print_code(secret);
    printf("\n");
    return 0;
}
