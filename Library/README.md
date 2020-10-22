# DeviceLib voor Arduino

Kopieer alle bestande uit deze folder en de Arduion-folder naar
de map Documente\Arduino\libraries\DeviceLib op een lokale computer.
De Arduino-IDE zal de classes van DeviceLib automatisch herkennen.

# DeviceLib voor de Raspberry

Voor de Raspberry bestaan er twee versies van DeviceLib.
De ene werkt op de standaard linux-manier via het bestandsysteem.
De andere werkt met de bcm2835-library.

Standaard linux
---------------
Kopieer alle bestanden uit deze folder en de "Raspberry/FileSysteem"-folder naar
de map "/home/pi/DeviceLib" op de Raspberry. Ga in de commando-shell naar deze
map en start "./buildLib.sh". Kopieer het bestand "libDeviceLib.so" naar de map
"/usr/lib/" en alle header files naar de map "/usr/include/DeviceLib/".
Start tenslotte het shell-commando "ldconfig".

Wanneer nu een programma met DeviceLib wordt gebouwd, heeft het geen extra
rechten nodig. Daardoor kan het eventueel remote worden gestart
(als bijv. een CGI-applicatie).

Via Bcm2835
-----------
Zorg ervoor dan de bcm2835-library op de Raspberry is geïnstalleerd.
Kopieer alle bestanden uit deze folder en de "Raspberry/Bcm2835"-folder naar
de map "/home/pi/DeviceLib" op de Raspberry. Ga in de commando-shell naar deze
map en start "./buildLib.sh". Kopieer het bestand "libDeviceLib.so" naar de map
"/usr/lib/" en alle header files naar de map "/usr/include/DeviceLib/".
Start tenslotte het shell-commando "ldconfig".

Wanneer nu een programma met DeviceLib wordt gebouwd, heeft het superuser
rechten nodig. Start een programma in de commando-shell door er "sudo" voor te
schrijven. Programma's met de bcm2835-versie van DeviceLib zijn ietsje sneller
en de class IButton werkt op dit moment alleen in deze versie.

Geen WiringPi
-------------
Aangezien het onderhoud aan WiringPi is gestaakt voor de open-source community,
bestaat er geen versie van DeviceLib, die daarop is gebaseerd.
