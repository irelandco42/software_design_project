GITBINARY := git
FEHURL := google.com
FIRMWAREREPO := simulator_firmware

ifeq ($(OS), Windows_NT)
	SHELL := CMD
endif

game: *.cpp
ifeq ($(OS), Windows_NT)
	@echo off && if exist "${FIRMWAREREPO}" ( \
		cd "$(FIRMWAREREPO)" && $(GITBINARY) pull && cd .. \
	) else ( \
		${GITBINARY} clone https://code.osu.edu/fehelectronics/proteus_software/$(FIRMWAREREPO).git \
	) \
	
	$(CXX) -g -std=c++11 -Wall -Isimulator_firmware/include -c *.cpp
	$(CXX) -Lsimulator_firmware/lib/ -o game.exe *.o -lfirmware_win -lws2_32 -lfirmware_win
else
	@ping -c 1 -W 1000 $(FEHURL) > /dev/null ; \
	if [ "$$?" -ne 0 ]; then \
		echo "Warning: No internet connection!" ; \
	else \
		if [ -d "$(FIRMWAREREPO)" ]; then \
			cd $(FIRMWAREREPO) ; \
			$(GITBINARY) pull ; \
			cd .. ; \
		else \
			$(GITBINARY) clone https://code.osu.edu/fehelectronics/proteus_software/$(FIRMWAREREPO).git ; \
		fi \
	fi \


	$(CXX) -g -std=c++11 -Wall -Isimulator_firmware/include -c *.cpp
	$(CXX) -Lsimulator_firmware/lib/ -o game *.o -lfirmware_mac
endif