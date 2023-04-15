__main_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

src-y+=$(__main_dir)main.c
inc-dirs+=$(__main_dir)

undefine __main_dir
