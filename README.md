# conskid

<b>Projekt CONSKID:</b>
	"Povezani sistem za zajemanje in analizo tornih lastnosti voznih ploskev"<br>
	Izdelan v okviru programa "Po kreativni poti do znanja 2017-2020", ki ga sofinancirata Republika Slovenija in Evropska unija iz Evropskega socialnega sklada.<br>

<b>Avtorji programske opreme:</b><br>
	Uroš Hudomalj, Aleksander Marinšek, Miha Ambrož<br>
<b>Sodelavci pri razvoju in preizkušanju:</b><br>
	Simon Grum, Rok Kogovšek, Rok Štefančič, Patrik Tarfila, Luka Grgurič, Juljiana Jamnik, Bajko Kulauzović, Mitja Pajek, Gašper Oman, Matija Mavrič, Robert Brozovič<br>

<b>Vsebina direktorijev:</b><br>
<ul>
	<li>conskidautodl - strežniška skripta za avtomatski prenos datotek z izmerki</li>
	<li>data_save - modul za shranjevanje izmerkov</li>
	<li>fifo - modul za delo z nizi prek FIFO bufferja</li>
	<li>gpio - modul za delo z GPIO vmesnikom na računalniku Raspberry Pi</li>
	<li>i2c - modul za delo z vmasnikom i2c na računalniku Raspberry Pi</li>
	<li>measuement_flow - modul za krmiljenje in spremljanje poteka meritev</li>
	<li>mpu_9250 - modul za delo z pospeškomerom na osnovi vezja MPU-9250</li>
	<li>requests - pomožne funkcije za ustvarjanje in nadzor zahtevkov</li>
	<li>rtos -  pomožne funkcije za krmiljenje in nadzor realnočasovnega OS</li>
	<li>serial_read - modul za delo s serijskim vmesnikom</li>
	<li>systemd.service - sistemski servis za samodejni zagon merilnega programa</li>
	<li>tasks - pomožne funkcije in definicije za krmiljenje in nadzor izvajalnikov</li>
	<li>main.c - glavni program</li>
	<li>supervisor.h - datoteka z nadzornimi spremenljivkami</li>
</ul>

<b>Sistemske zahteve:</b><br>
<ul>
	<li>Raspberry Pi Zero W</li>
	<li>Pospeškomer na osnovi MPU-9250</li>
	<li>GNSS-sprejemnik skladen z NMEA 0183 s priklopom na USB (priklop definiran v serial_read/serial_read.h)</li>
	<li>dva gumba in dve LED za uporabniški vmesnik (priklopi definirani v gpio/gpio.h)</li>
	<li><a href="https://www.raspberrypi.org/downloads/raspbian/">Raspbian Lite</a></li>
</ul>
	
<b>Namestitev:</b><br>
	Vključena je datoteka makefile (prevajanje z "make"), preveden program je možno pognati od koder koli na datotečnem sistemu.<br>
