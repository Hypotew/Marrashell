# 42sh — Features TODO

## Easy / Quick
- [x] `&&` / `||` — logical operators between commands
- [x] `!` history expansion — `!!`, `!n`, `!str` before tokenizing
- [ ] Inhibitors (quotes) — single/double quote handling in tokenizer

## Medium
- [x] Variable expansion — `$VAR`, `$?`, `set`/`unset` builtins, local var storage
- [ ] Aliases — `alias`/`unalias` builtins, expansion before tokenizing
- [ ] Special variables — `$term`, `$cwd`, `$precmd`, `$cwdcmd`, `$ignoreof`
- [ ] Globbing — `*`, `?`, `[`, `]` via `glob()` from libc
- [ ] Backticks — command substitution, fork+pipe+capture stdout
- [ ] Parentheses — `( )` subshells, fork and run in child

## Hard / Large
- [ ] Job control — `&`, `fg`, `bg`, signal handling, job table, process groups
- [ ] Line editing — custom input with ncurses/termios, cursor, history nav, keybindings
- [ ] Auto-completion — tab handler, path/command scanning, prefix matching

## Very Hard
- [ ] Scripting — `if`/`else`/`endif`, `foreach`, `while`
