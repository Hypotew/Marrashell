# Marrashell — Refactoring ncurses TUI

**Date:** 2026-05-22
**Branche cible:** tommy

---

## Objectif

Remplacer le terminal brut actuel par une interface TUI ncurses à 3 zones :
- **Sidebar gauche** : bannière ASCII, thème actif, branche git, PWD, dernière commande
- **Zone output** (droite haut) : sortie des commandes, scrollable
- **Barre input** (bas, pleine largeur) : saisie interactive

Le comportement en mode non-interactif (`stdin` n'est pas un TTY) reste inchangé : pas de TUI, exécution directe des commandes.

---

## Architecture

### Principe général

La couche d'exécution (parser, exec, expand, builtins, env) n'est **pas modifiée**. Seule la couche I/O change :

| Supprimé | Remplacé par |
|----------|-------------|
| `src/shortcuts/` (4 fichiers) | `src/tui/` (8 fichiers) |
| `include/readline.h` | `include/tui.h` |
| `termios` raw mode | ncurses `wgetch` |
| `printf`/`fprintf` directs | pipe interne → `wprintw` dans `pad_output` |

### Redirection stdout

`stdout` est redirigé via `dup2` vers un pipe interne à l'init TUI. Un thread POSIX lit ce pipe en continu et écrit dans `pad_output` via `wprintw`. Les commandes exécutées continuent d'écrire sur `stdout` sans modification.

---

## Structures de données

### `tui_t` (dans `include/tui.h`)

```c
typedef struct {
    WINDOW *win_sidebar;    // panneau gauche
    WINDOW *win_output;     // viewport du pad (zone visible)
    WINDOW *win_input;      // barre de saisie
    WINDOW *pad_output;     // newpad() — hauteur virtuelle illimitée
    int     scroll_offset;  // lignes scrollées vers le haut
    int     pad_lines;      // nb de lignes écrites dans le pad
    int     theme_id;       // index dans G_THEMES[]
    int     pipe_fds[2];    // pipe stdout → pad_output
    pthread_t reader_thread;
} tui_t;
```

`shell_t` (dans `include/shell.h`) gagne un champ `tui_t *tui`.

### `theme_t` (dans `include/tui.h`)

```c
typedef struct {
    const char *name;
    short bg_main;      // fond output
    short bg_sidebar;   // fond sidebar
    short bg_input;     // fond input bar
    short fg_accent;    // PWD, prompt ✓
    short fg_error;     // prompt ✗, erreurs
    short fg_info;      // bannière, branche git
    short fg_cmd;       // dernière commande affichée
} theme_t;

extern const theme_t G_THEMES[5];
```

### 5 thèmes disponibles

| ID | Nom | Inspiré de |
|----|-----|-----------|
| 0 | Dark Blue | Thème actuel du projet |
| 1 | Gruvbox | Gruvbox dark |
| 2 | Catppuccin | Catppuccin Mocha |
| 3 | Nord | Nord theme |
| 4 | Dracula | Dracula theme |

---

## Fichiers

### Supprimés
- `src/shortcuts/terminal_mode.c`
- `src/shortcuts/input_controls.c`
- `src/shortcuts/interactive_inputs.c`
- `src/shortcuts/history_navigation.c`
- `include/readline.h`

### Modifiés
- `include/shell.h` — ajoute `tui_t *tui` dans `shell_t`
- `src/shell/init.c` — appelle `tui_init()`, lit `~/.42shrc` pour le thème
- `src/shell/loop.c` — remplace `read_line()` par `tui_read_line()`, `printf` prompt par `tui_update_sidebar()`
- `Makefile` — ajoute `src/tui/*.c`, flags `-lncurses -lpthread`, retire `src/shortcuts/*.c`

### Nouveaux (`src/tui/`)
- `tui_init.c` — `tui_init()` / `tui_destroy()` : `initscr()`, création des WINDOWs, démarrage du thread reader
- `tui_input.c` — `tui_read_line()` : boucle `wgetch`, édition ligne, navigation historique (flèches), `Ctrl+C`/`Ctrl+D`/`Ctrl+L`
- `tui_output.c` — thread reader : lit le pipe, `wprintw` dans `pad_output`, gère le scroll automatique
- `tui_sidebar.c` — `tui_update_sidebar()` : redessine bannière, thème, git, PWD, dernière commande
- `tui_theme.c` — `G_THEMES[]`, `apply_theme()`, `tui_init_color_pairs()`
- `tui_theme_menu.c` — `tui_open_theme_menu()` : overlay centré, navigation ↑↓, Enter valide, Esc annule, sauvegarde dans `~/.42shrc`
- `tui_resize.c` — handler `SIGWINCH` : recalcul des dimensions, `wresize()` + `mvwin()` sur tous les WINDOWs

---

## Comportements clés

### Scroll output
- `PgUp` / `PgDn` depuis la barre input (toujours active) : ajuste `scroll_offset`, rappelle `prefresh()`
- Le curseur reste dans `win_input` — seul le contenu de `pad_output` défile
- Le scroll est automatiquement remis à zéro (bottom) à chaque nouvelle commande exécutée

### Commande `/theme`
- Interceptée dans `tui_read_line()` avant d'être passée au parser
- Appelle `tui_open_theme_menu()` qui dessine un overlay `WINDOW` centré par-dessus le TUI
- Après sélection : `apply_theme()` + `wrefresh()` de tous les WINDOWs + écriture dans `~/.42shrc`

### Persistance du thème
- Lu au démarrage dans `shell_init()` via `local_get_value(shell, "theme")` après parsing de `~/.42shrc`
- Format : `set theme=gruvbox`
- Si absent ou valeur inconnue : thème 0 (Dark Blue) par défaut

### Resize terminal
- `SIGWINCH` → handler qui appelle `tui_resize()` : recalcule `LINES`/`COLS`, redimensionne et repositionne les 3 WINDOWs, redessine tout

### Mode non-interactif
- `isatty(STDIN_FILENO)` vérifié dans `shell_loop()` avant `tui_init()`
- Si non-TTY : comportement actuel inchangé, `tui_init()` n'est pas appelé, `shell_t.tui` reste `NULL`

---

## Contraintes Epitech

- Chaque fichier `.c` respecte la norme : ≤ 25 lignes par fonction, ≤ 5 fonctions par fichier, ≤ 4 paramètres par fonction
- Header EPITECH en tête de chaque fichier
- Compilation via `epiclang` (wrapper gcc avec vérification de norme)
- `-lncurses` et `-lpthread` ajoutés aux `LDFLAGS` du Makefile
