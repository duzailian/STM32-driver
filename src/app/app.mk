__app_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

inc-dirs+=$(__app_dir)

undefine __app_dir
