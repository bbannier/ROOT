/* Automatically generated file, do not edit */
#include "sys.h"
#include "el.h"
el_private const struct el_bindings_t el_func_help[] = {
   { "vi-paste-next", VI_PASTE_NEXT,
     "Vi paste previous deletion to the right of the cursor" },
   { "vi-paste-prev", VI_PASTE_PREV,
     "Vi paste previous deletion to the left of the cursor" },
   { "vi-prev-space-word", VI_PREV_SPACE_WORD,
     "Vi move to the previous space delimited word" },
   { "vi-prev-word", VI_PREV_WORD,
     "Vi move to the previous word" },
   { "vi-next-space-word", VI_NEXT_SPACE_WORD,
     "Vi move to the next space delimited word" },
   { "vi-next-word", VI_NEXT_WORD,
     "Vi move to the next word" },
   { "vi-change-case", VI_CHANGE_CASE,
     "Vi change case of character under the cursor and advance one character" },
   { "vi-change-meta", VI_CHANGE_META,
     "Vi change prefix command" },
   { "vi-insert-at-bol", VI_INSERT_AT_BOL,
     "Vi enter insert mode at the beginning of line" },
   { "vi-replace-char", VI_REPLACE_CHAR,
     "Vi replace character under the cursor with the next character typed" },
   { "vi-replace-mode", VI_REPLACE_MODE,
     "Vi enter replace mode" },
   { "vi-substitute-char", VI_SUBSTITUTE_CHAR,
     "Vi replace character under the cursor and enter insert mode" },
   { "vi-substitute-line", VI_SUBSTITUTE_LINE,
     "Vi substitute entire line" },
   { "vi-change-to-eol", VI_CHANGE_TO_EOL,
     "Vi change to end of line" },
   { "vi-insert", VI_INSERT,
     "Vi enter insert mode" },
   { "vi-add", VI_ADD,
     "Vi enter insert mode after the cursor" },
   { "vi-add-at-eol", VI_ADD_AT_EOL,
     "Vi enter insert mode at end of line" },
   { "vi-delete-meta", VI_DELETE_META,
     "Vi delete prefix command" },
   { "vi-end-word", VI_END_WORD,
     "Vi move to the end of the current space delimited word" },
   { "vi-to-end-word", VI_TO_END_WORD,
     "Vi move to the end of the current word" },
   { "vi-undo", VI_UNDO,
     "Vi undo last change" },
   { "vi-command-mode", VI_COMMAND_MODE,
     "Vi enter command mode (use alternative key bindings)" },
   { "vi-zero", VI_ZERO,
     "Vi move to the beginning of line" },
   { "vi-delete-prev-char", VI_DELETE_PREV_CHAR,
     "Vi move to previous character (backspace)" },
   { "vi-list-or-eof", VI_LIST_OR_EOF,
     "Vi list choices for completion or indicate end of file if empty line" },
   { "vi-kill-line-prev", VI_KILL_LINE_PREV,
     "Vi cut from beginning of line to cursor" },
   { "vi-search-prev", VI_SEARCH_PREV,
     "Vi search history previous" },
   { "vi-search-next", VI_SEARCH_NEXT,
     "Vi search history next" },
   { "vi-repeat-search-next", VI_REPEAT_SEARCH_NEXT,
     "Vi repeat current search in the same search direction" },
   { "vi-repeat-search-prev", VI_REPEAT_SEARCH_PREV,
     "Vi repeat current search in the opposite search direction" },
   { "vi-next-char", VI_NEXT_CHAR,
     "Vi move to the character specified next" },
   { "vi-prev-char", VI_PREV_CHAR,
     "Vi move to the character specified previous" },
   { "vi-to-next-char", VI_TO_NEXT_CHAR,
     "Vi move up to the character specified next" },
   { "vi-to-prev-char", VI_TO_PREV_CHAR,
     "Vi move up to the character specified previous" },
   { "vi-repeat-next-char", VI_REPEAT_NEXT_CHAR,
     "Vi repeat current character search in the same search direction" },
   { "vi-repeat-prev-char", VI_REPEAT_PREV_CHAR,
     "Vi repeat current character search in the opposite search direction" },
   { "em-delete-or-list", EM_DELETE_OR_LIST,
     "Delete character under cursor or list completions if at end of line" },
   { "em-delete-next-word", EM_DELETE_NEXT_WORD,
     "Cut from cursor to end of current word" },
   { "em-yank", EM_YANK,
     "Paste cut buffer at cursor position" },
   { "em-kill-line", EM_KILL_LINE,
     "Cut the entire line and save in cut buffer" },
   { "em-kill-region", EM_KILL_REGION,
     "Cut area between mark and cursor and save in cut buffer" },
   { "em-copy-region", EM_COPY_REGION,
     "Copy area between mark and cursor to cut buffer" },
   { "em-gosmacs-traspose", EM_GOSMACS_TRASPOSE,
     "Exchange the two characters before the cursor" },
   { "em-next-word", EM_NEXT_WORD,
     "Move next to end of current word" },
   { "em-upper-case", EM_UPPER_CASE,
     "Uppercase the characters from cursor to end of current word" },
   { "em-capitol-case", EM_CAPITOL_CASE,
     "Capitalize the characters from cursor to end of current word" },
   { "em-lower-case", EM_LOWER_CASE,
     "Lowercase the characters from cursor to end of current word" },
   { "em-set-mark", EM_SET_MARK,
     "Set the mark at cursor" },
   { "em-exchange-mark", EM_EXCHANGE_MARK,
     "Exchange the cursor and mark" },
   { "em-universal-argument", EM_UNIVERSAL_ARGUMENT,
     "Universal argument (argument times 4)" },
   { "em-meta-next", EM_META_NEXT,
     "Add 8th bit to next character typed" },
   { "em-toggle-overwrite", EM_TOGGLE_OVERWRITE,
     "Switch from insert to overwrite mode or vice versa" },
   { "em-copy-prev-word", EM_COPY_PREV_WORD,
     "Copy current word to cursor" },
   { "em-inc-search-next", EM_INC_SEARCH_NEXT,
     "Emacs incremental next search" },
   { "em-inc-search-prev", EM_INC_SEARCH_PREV,
     "Emacs incremental reverse search" },
   { "em-undo", EM_UNDO,
     "Emacs undo last change" },
   { "ed-end-of-file", ED_END_OF_FILE,
     "Indicate end of file" },
   { "ed-insert", ED_INSERT,
     "Add character to the line" },
   { "ed-delete-prev-word", ED_DELETE_PREV_WORD,
     "Delete from beginning of current word to cursor" },
   { "ed-delete-next-char", ED_DELETE_NEXT_CHAR,
     "Delete character under cursor" },
   { "ed-kill-line", ED_KILL_LINE,
     "Cut to the end of line" },
   { "ed-move-to-end", ED_MOVE_TO_END,
     "Move cursor to the end of line" },
   { "ed-move-to-beg", ED_MOVE_TO_BEG,
     "Move cursor to the beginning of line" },
   { "ed-transpose-chars", ED_TRANSPOSE_CHARS,
     "Exchange the character to the left of the cursor with the one under it" },
   { "ed-next-char", ED_NEXT_CHAR,
     "Move to the right one character" },
   { "ed-prev-word", ED_PREV_WORD,
     "Move to the beginning of the current word" },
   { "ed-prev-char", ED_PREV_CHAR,
     "Move to the left one character" },
   { "ed-quoted-insert", ED_QUOTED_INSERT,
     "Add the next character typed verbatim" },
   { "ed-digit", ED_DIGIT,
     "Adds to argument or enters a digit" },
   { "ed-argument-digit", ED_ARGUMENT_DIGIT,
     "Digit that starts argument" },
   { "ed-unassigned", ED_UNASSIGNED,
     "Indicates unbound character" },
   { "ed-tty-sigint", ED_TTY_SIGINT,
     "Tty interrupt character" },
   { "ed-tty-dsusp", ED_TTY_DSUSP,
     "Tty delayed suspend character" },
   { "ed-tty-flush-output", ED_TTY_FLUSH_OUTPUT,
     "Tty flush output characters" },
   { "ed-tty-sigquit", ED_TTY_SIGQUIT,
     "Tty quit character" },
   { "ed-tty-sigtstp", ED_TTY_SIGTSTP,
     "Tty suspend character" },
   { "ed-tty-stop-output", ED_TTY_STOP_OUTPUT,
     "Tty disallow output characters" },
   { "ed-tty-start-output", ED_TTY_START_OUTPUT,
     "Tty allow output characters" },
   { "ed-newline", ED_NEWLINE,
     "Execute command" },
   { "ed-delete-prev-char", ED_DELETE_PREV_CHAR,
     "Delete the character to the left of the cursor" },
   { "ed-clear-screen", ED_CLEAR_SCREEN,
     "Clear screen leaving current line at the top" },
   { "ed-redisplay", ED_REDISPLAY,
     "Redisplay everything" },
   { "ed-start-over", ED_START_OVER,
     "Erase current line and start from scratch" },
   { "ed-sequence-lead-in", ED_SEQUENCE_LEAD_IN,
     "First character in a bound sequence" },
   { "ed-prev-history", ED_PREV_HISTORY,
     "Move to the previous history line" },
   { "ed-next-history", ED_NEXT_HISTORY,
     "Move to the next history line" },
   { "ed-search-prev-history", ED_SEARCH_PREV_HISTORY,
     "Search previous in history for a line matching the current" },
   { "ed-search-next-history", ED_SEARCH_NEXT_HISTORY,
     "Search next in history for a line matching the current" },
   { "ed-prev-line", ED_PREV_LINE,
     "Move up one line" },
   { "ed-next-line", ED_NEXT_LINE,
     "Move down one line" },
   { "ed-command", ED_COMMAND,
     "Editline extended command" },
   { NULL, 0, NULL }
};

el_protected const el_bindings_t*
help__get() { return el_func_help; }
