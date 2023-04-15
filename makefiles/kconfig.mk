__kconfig_path:=$(dir $(lastword $(MAKEFILE_LIST)))

srctree:=$(__kconfig_path)../scripts/kconfig

PHONY += scriptconfig iscriptconfig menuconfig guiconfig dumpvarsconfig oldconfig



PYTHONCMD ?= python3
kpython := PYTHONPATH=:$srctree$PYTHONPATH $(PYTHONCMD)

ifneq ($(filter scriptconfig,$(MAKECMDGOALS)),)
ifndef SCRIPT
$(error Use "make scriptconfig SCRIPT=<path to script> [SCRIPT_ARG=<argument>]")
endif
endif

scriptconfig:
	$(Q)$(kpython) $(SCRIPT) $(Kconfig) $(if $(SCRIPT_ARG),"$(SCRIPT_ARG)")

iscriptconfig:
	$(Q)$(kpython) -i -c \
	  "import kconfiglib; \
	   kconf = kconfiglib.Kconfig('$(Kconfig)'); \
	   print('A Kconfig instance \'kconf\' for the architecture $(ARCH) has been created.')"

menuconfig:
	$(Q)$(kpython) $(srctree)/menuconfig.py $(Kconfig)

guiconfig:
	$(Q)$(kpython) $(srctree)/guiconfig.py $(Kconfig)

dumpvarsconfig:
	$(Q)$(kpython) $(srctree)/examples/dumpvars.py $(Kconfig)

oldconfig:
	$(Q)$(kpython) $(srctree)/oldconfig.py $(Kconfig)


#undefine __kconfig_path