ifeq ($(IDE),keil)
_script := keil.py
else
_script := iar.py
endif
.PHONY: all,help
all : 
	@cd scripts; python $(_script)
help:
	@echo "'make IDE=keil' generate keil project otherwise generate IAR project"