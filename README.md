# CONSKID

<i>For English description please scroll down.</i><br>

<b>Projekt CONSKID:</b>
	"<a href="http://kmtm.fs.uni-lj.si/conskid/">Povezani sistem za zajemanje in analizo tornih lastnosti voznih ploskev</a>"<br>
	Izdelan v okviru programa "<a href="http://www.sklad-kadri.si/si/razvoj-kadrov/po-kreativni-poti-do-znanja-pkp/">Po kreativni poti do znanja 2017-2020</a>", ki ga sofinancirata Republika Slovenija in Evropska unija iz Evropskega socialnega sklada.<br>

<b>Avtorji programske opreme:</b><br>
	Uroš Hudomalj, Aleksander Marinšek, Miha Ambrož<br>
<b>Sodelavci pri razvoju in preizkušanju:</b><br>
	Simon Grum, Rok Kogovšek, Rok Štefančič, Patrik Tarfila, Luka Grgurič, Roman Kamnik, Juljiana Jamnik, Bajko Kulauzović, Mitja Pajek, Gašper Oman, Matija Mavrič, Robert Brozovič<br>

<b>Vsebina direktorijev:</b><br>
<ul>
	<li>conskidautodl - strežniška skripta za avtomatski prenos datotek z izmerki</li>
	<li>data_save - modul za shranjevanje izmerkov</li>
	<li>fifo - modul za delo z nizi prek FIFO bufferja</li>
	<li>gpio - modul za delo z GPIO vmesnikom na računalniku Raspberry Pi</li>
	<li>i2c - modul za delo z vmesnikom i2c na računalniku Raspberry Pi</li>
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


<b>The CONSKID project:</b>
	"<a href="http://kmtm.fs.uni-lj.si/conskid/">Connected system for acquisition and analysis of driving surface friction properties</a>"<br>
	Developed within the programe "<a href="http://www.sklad-kadri.si/si/razvoj-kadrov/po-kreativni-poti-do-znanja-pkp/">Creative Path to Knowledge 2017-2020</a>", co-funded by the Republic of Slovenia and the European Union from the European Social fund.<br>

<b>Authors of the software:</b><br>
	Uroš Hudomalj, Aleksander Marinšek, Miha Ambrož<br>
<b>Development and testing:</b><br>
	Simon Grum, Rok Kogovšek, Rok Štefančič, Patrik Tarfila, Luka Grgurič, Roman Kamnik, Juljiana Jamnik, Bajko Kulauzović, Mitja Pajek, Gašper Oman, Matija Mavrič, Robert Brozovič<br>

<b>Directory contents:</b><br>
<ul>
	<li>conskidautodl - server script for automatic data transfer</li>
	<li>data_save - data save module</li>
	<li>fifo - module for FIFO buffer string manipulation</li>
	<li>gpio - module for interfacing to GPIO on Raspberry Pi</li>
	<li>i2c - module for interfacing to i2c on Raspberry Pi</li>
	<li>measuement_flow - module for data flow control and monitoring</li>
	<li>mpu_9250 - modul for interfacing to a MPU-9250-based accelerometer</li>
	<li>requests - auxiliary functions for request creation and control</li>
	<li>rtos - auxiliary functions for real-time OS creation and control</li>
	<li>serial_read - module for working with serial interface</li>
	<li>systemd.service - system service for application automatic start</li>
	<li>tasks - auxiliary functions for task creation and control</li>
	<li>main.c - main program</li>
	<li>supervisor.h - control variable configuration file</li>
</ul>

<b>System requirements:</b><br>
<ul>
	<li>Raspberry Pi Zero W</li>
	<li>MPU-9250-based accelerometer</li>
	<li>NMEA 0183-compliant GNSS-receiver connected to USB (connection parameters defined in serial_read/serial_read.h)</li>
	<li>Two pushbuttons and two LEDs for user interface (connection paramters defined in gpio/gpio.h)</li>
	<li><a href="https://www.raspberrypi.org/downloads/raspbian/">Raspbian Lite</a></li>
</ul>
	
<b>Installation:</b><br>
	Includes makefile (compile with "make"), compiled executable runs from anywhere on the filesystem.<br>
