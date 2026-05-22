# MasterMind

Jeu Mastermind en ligne de commande ecrit en C.

## Compilation

### macOS

Dans un terminal, depuis le dossier du projet :

```sh
cc -std=c11 -Wall -Wextra -Wpedantic main.c -o mastermind
```

La commande `cc` est disponible avec les outils de compilation Xcode.

### Windows

Avec GCC, par exemple via MinGW :

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic main.c -o mastermind.exe
```

Avec Clang :

```sh
clang -std=c11 -Wall -Wextra -Wpedantic main.c -o mastermind.exe
```

Avec le compilateur Microsoft Visual C++ dans un terminal de developpement
Visual Studio :

```bat
cl /W4 main.c /Fe:mastermind.exe
```

## Execution

### macOS

```sh
./mastermind
```

### Windows

Dans PowerShell :

```powershell
.\mastermind.exe
```

Dans l'invite de commandes Windows :

```bat
mastermind.exe
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
