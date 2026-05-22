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

Le programme tire un code de quatre chiffres entre `1` et `6`.
Les doublons sont possibles et tu disposes de dix essais.
Apres chaque proposition, le jeu indique les chiffres bien places et mal places.
Entre `q` pour quitter une partie.
