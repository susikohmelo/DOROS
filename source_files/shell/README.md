# Shell documentation

This is a very basic shell. I've made much more sophisticated shells before, it's a boring process so here I just went for a simple one that works.

Input format is:
```
command arg1 arg2 arg3 ...
```
The first word is the command and the rest are arguments, delimited by spaces.

Input buffer has a predefined max size of `SCREEN_WIDTH - PROMPT_LENGTH`. Characters after that are ignored.<br>
This is usually about 65 characters, which sounds very little, but since you only run one command at a time it is rare to even come close to it.

## Font
`Font` is a file that contains the system font. It is the 8x16 bitmap of 256 characters used by the VGA hardware.
It being inside the shell may seem like an odd location, but the font is only used for the paint/draw program.

## cmd_draw
This is the painting program. Most of it is easy to follow, as it's just drawing characters into a grid.

The mouse cursor however is a bit more tricky. A very, very old trick is used here to get the illusion of a graphical cursor whilst still in text mode.

We have the mouse at a given position. We grab it's surrounding characters (2x2), and replace them with the characters in the system font reserved for the mouse.
These mouse characters are then modified to have the same pixel data as the characters they replaced. Then, we can finally do a bitmask of the cursor onto the 2x2 characters.
When the mouse moves outside these characters, just return the original ones to the screen.
