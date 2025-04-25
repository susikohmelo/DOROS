# Shell documentation

This is a very basic shell. I've made much more sophisticated shells before, it's a boring process so here I just went for a simple one that works.

Input format is:
```
command arg1 arg2 arg3 ...
```
The first word is the command and the rest are arguments, delimited by spaces.

Input buffer has a predefined max size of `SCREEN_WIDTH - PROMPT_LENGTH`. Characters after that are ignored.<br>
This is usually about 65 characters, which sounds very little, but since you only run one command at a time it is rare to even come close to it.
