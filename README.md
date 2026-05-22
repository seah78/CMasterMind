# MasterMind

Jeu Mastermind en ligne de commande ecrit en C.

## Compilation

```sh
cc -std=c11 -Wall -Wextra -Wpedantic main.c -o mastermind
```

## Execution

```sh
./mastermind
```

Le programme tire un code de quatre pions parmi six couleurs.
Les doublons sont possibles et tu disposes de dix essais.
Le plateau utilise les couleurs ANSI du terminal pour afficher les pions.

Pour proposer une combinaison, entre les initiales des couleurs :

```text
R = Rouge
V = Vert
B = Bleu
J = Jaune
M = Magenta
C = Cyan
```

Par exemple, `RVBJ` et `R V B J` sont deux saisies valides.
Apres chaque proposition, le jeu indique les pions bien places et les bonnes
couleurs mal placees. Entre `q` pour quitter une partie.
