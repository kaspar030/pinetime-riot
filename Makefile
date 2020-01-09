#
# PineTime watch application
#
#
RIOTBASE ?= $(CURDIR)/RIOT
APPLICATION=pinetime-riot
BOARD ?= pinetime

# include display driver
USEMODULE += ili9341
USEMODULE += mineplex
PSEUDOMODULES += stdio_display
USEMODULE += stdio_display

#
USEMODULE += ztimer ztimer_rtt xtimer_on_ztimer xtimer

include $(RIOTBASE)/Makefile.include

clone_riot:
	git clone https://github.com/kaspar030/RIOT -b pinetime_display
