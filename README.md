# conskid

<b>Projekt CONSKID:</b>
	"Povezani sistem za zajemanje in analizo tornih lastnosti voznih ploskev"<br>
	Izdelan v okviru programa "Po kreativni poti do znanja 2017-2020", ki ga sofinancirata Republika Slovenija in Evropska unija iz Evropskega socialnega sklada.<br>

<b>Avtorji programske opreme:</b><br>
	Uroš Hudomalj, Aleksander Marinšek, Miha Ambrož
<b>Sodelavci pri razvoju in preizkušanju:</b><br>
	Simon Grum, Rok Kogovšek, Rok Štefančič, Patrik Tarfila, Luka Grgurič, Juljiana Jamnik, Bajko Kulauzović, Mitja Pajek, Gašper Oman, Matija Mavrič, Robert Brozovič<br>

<b>Vsebina direktorijev:</b><br>
	conskidautodl - strežniška skripta za avtomatski prenos datotek z izmerki<br>
	data_save - modul za shranjevanje izmerkov<br>
	fifo - modul za delo z nizi prek FIFO bufferja<br>
	gpio - modul za delo z GPIO vmesnikom na računalniku Raspberry Pi<br>
	i2c - modul za delo z vmasnikom i2c na računalniku Raspberry Pi<br>
	measuement_flow - modul za krmiljenje in spremljanje poteka meritev<br>
	mpu_9250 - modul za delo z pospeškomerom na osnovi vezja MPU-9250<br>
	requests - pomožne funkcije za ustvarjanje in nadzor zahtevkov<br>
	rtos -  pomožne funkcije za krmiljenje in nadzor realnočasovnega OS<br>
	serial_read - modul za delo s serijskim vmesnikom<br>
	systemd.service - sistemski servis za samodejni zagon merilnega programa<br>
	tasks - pomožne funkcije in definicije za krmiljenje in nadzor izvajalnikov<br>
	main.c - glavni program<br>
	supervisor.h - datoteka z nadzornimi spremenljivkami<br>

<b>Sistemske zahteve:</b><br>
	Raspberry Pi Zero W<br>
	Pospeškomer na osnovi MPU-9250<br>
	GNSS-sprejemnik skladen z NMEA 0183 s priklopom na USB (priklop definiran v serial_read/serial_read.h)<br>
	dva gumba in dve LED za uporabniški vmesnik (priklopi definirani v gpio/gpio.h)<br>
	<a href="https://www.raspberrypi.org/downloads/raspbian/">Raspbian Lite</a><br>
	
<b>Namestitev:</b><br>
	Vključena je datoteka makefile (prevajanje z "make"), preveden program je možno pognati od koder koli na datotečnem sistemu.<br>
