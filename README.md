# conskid

<b>Projekt CONSKID:</b>
	"Povezani sistem za zajemanje in analizo tornih lastnosti voznih ploskev"
	Izdelan v okviru programa "Po kreativni poti do znanja 2017-2020", ki ga sofinancirata Republika Slovenija in Evropska unija iz Evropskega socialnega sklada.

<b>Avtorji programske opreme:</b>
	Uroš Hudomalj, Aleksander Marinšek, Miha Ambrož
<b>Sodelavci pri razvoju in preizkušanju:
	Simon Grum, Rok Kogovšek, Rok Štefančič, Patrik Tarfila, Luka Grgurič, Juljiana Jamnik, Bajko Kulauzović, Mitja Pajek, Gašper Oman, Matija Mavrič, Robert Brozovič

<b>Vsebina direktorijev:</b>
	conskidautodl - strežniška skripta za avtomatski prenos datotek z izmerki
	data_save - modul za shranjevanje izmerkov
	fifo - modul za delo z nizi prek FIFO bufferja
	gpio - modul za delo z GPIO vmesnikom na računalniku Raspberry Pi
	i2c - modul za delo z vmasnikom i2c na računalniku Raspberry Pi
	measuement_flow - modul za krmiljenje in spremljanje poteka meritev
	mpu_9250 - modul za delo z pospeškomerom na osnovi vezja MPU-9250
	requests - pomožne funkcije za ustvarjanje in nadzor zahtevkov
	rtos -  pomožne funkcije za krmiljenje in nadzor realnočasovnega OS
	serial_read - modul za delo s serijskim vmesnikom
	systemd.service - sistemski servis za samodejni zagon merilnega programa
	tasks - pomožne funkcije in definicije za krmiljenje in nadzor izvajalnikov
	main.c - glavni program
	supervisor.h - datoteka z nadzornimi spremenljivkami

<b>Sistemske zahteve:</b>
	Raspberry Pi Zero W
	Pospeškomer na osnovi MPU-9250
	GNSS-sprejemnik skladen z NMEA 0183 s priklopom na USB (priklop definiran v serial_read/serial_read.h)
	dva gumba in dve LED za uporabniški vmesnik (priklopi definirani v gpio/gpio.h)
	<a href="https://www.raspberrypi.org/downloads/raspbian/">Raspbian Lite</a>
	
<b>Namestitev:</b>
	Vključena je datoteka makefile (prevajanje z "make"), preveden program je možno pognati od koder koli na datotečnem sistemu.
