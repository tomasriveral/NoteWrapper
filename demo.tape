Require echo

Set Shell "zsh"
Set TypingSpeed 35ms
Set FontSize 18
Set Width 1800
Set Height 900

Output demo.gif

# ─────────────────────────────────────────────────────────────────────
# Introduction
# ─────────────────────────────────────────────────────────────────────

Type "echo 'Welcome to NoteWrapper!'"
Sleep 800ms
Enter
Sleep 1500ms

Type "notewrapper --help"
Sleep 500ms
Enter
Sleep 4000ms

# ─────────────────────────────────────────────────────────────────────
# Create / open a unified journal
# ─────────────────────────────────────────────────────────────────────

Type "notewrapper"
Sleep 700ms
Enter
Sleep 1500ms

Down
Sleep 500ms
Down
Sleep 500ms
Down
Sleep 500ms
Down
Sleep 700ms

Enter
Sleep 1200ms

Down
Sleep 600ms
Enter
Sleep 1500ms

# Enter the editor
Type "i"
Sleep 800ms

Type "NoteWrapper uses standard Markdown files, so you can open your notes in any other application that supports Markdown."
Sleep 700ms
Enter
Sleep 800ms

Type "This is a unified journal: all your daily entries live in the same file. You can also configure NoteWrapper to use monthly or other time-based entries."
Sleep 700ms
Enter
Sleep 800ms

Type "And, although you can't see it in this demo, this Markdown file is rendered live in my browser thanks to Vivify!"
Sleep 1500ms
Sleep 3000ms

# Leave the editor
Escape
Sleep 500ms
Type ":q!"
Sleep 800ms
Enter
Sleep 1500ms

# ─────────────────────────────────────────────────────────────────────
# Create / open a divided journal
# ─────────────────────────────────────────────────────────────────────

Down
Sleep 500ms
Down
Sleep 500ms
Down
Sleep 700ms

Enter
Sleep 2000ms

Enter
Sleep 1000ms

Type "i"
Sleep 800ms

Type "And this is a divided journal."
Sleep 500ms
Type " It uses one file per entry."
Sleep 700ms
Enter
Sleep 1000ms

Type "Find the dates and journal divisions a little too cumbersome?"
Sleep 700ms
Enter
Sleep 1500ms

Type "Don't worry: NoteWrapper also works with basic notes."
Sleep 800ms

Type "It distinguishes them using a regular expression, which defaults to `.*journal.*`."
Sleep 800ms
Enter
Sleep 700ms

Type "The unified or divided journal mode is chosen when you first create the journal."
Sleep 1000ms
Enter
Sleep 1500ms

# ─────────────────────────────────────────────────────────────────────
# Search
# ─────────────────────────────────────────────────────────────────────

Type "Let me show you one last cool feature."
Sleep 1000ms

Type "Have you ever forgotten when or where you wrote something?"
Sleep 1000ms
Enter
Sleep 800ms

Type "Well ... NoteWrapper was made for you!"
Sleep 1500ms

Escape
Sleep 500ms
Type ":q!"
Sleep 800ms
Enter
Sleep 1500ms

# Navigate to search
Down
Sleep 500ms
Down
Sleep 500ms
Down
Sleep 800ms

Enter
Sleep 1500ms

Down
Sleep 500ms
Down
Sleep 800ms

Enter
Sleep 2000ms

# Search all notes with fzf
Type "c"
Sleep 300ms
Type "o"
Sleep 300ms
Type "o"
Sleep 300ms
Type "l"
Sleep 1500ms

# Show the search result / preview
Up
Sleep 5000ms
Enter
Sleep 1000ms

# ─────────────────────────────────────────────────────────────────────
# Final note
# ─────────────────────────────────────────────────────────────────────

Type "i"
Sleep 700ms

Type "Never lose a note."
Sleep 1500ms
Enter
Sleep 1000ms

Type "P. S. Don't mind the broken fzf preview. Normally, you'll see the context around your search result, but sometimes `sed` breaks when fzf is opened for the first time."
Sleep 5000ms
