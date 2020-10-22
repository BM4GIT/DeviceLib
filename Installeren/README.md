# Installeer DeviceLib op de Raspberry


Om de bcm2538-versie te gebruiken:
----------------------------------
1. Download het bestand 'DeviceLib_BCM.tar' en pak het uit.
2. Er verschijnen twee mappen:  "home-pi" en "usr".
3. Zorg dat ook de libraries voor 'bcm2538', 'mysql-client' en 'opencv' zijn geïnstalleerd

Om de linux filesystem-versie te gebruiken:
-------------------------------------------
1. Download het bestand 'DeviceLib_SYS' en pak het uit.
2. Er verschijnen twee mappen:  "home-pi" en "usr".
3. Zorg dat ook de libraries voor 'mysql-client' en 'opencv' zijn geïnstalleerd

DeviceLib installeren:
----------------------
1. Open de commando-shell.
2. Type het commando 'sudo pcmanfm'. De verkenner opent met superuser-rechten.
3. Kopieer de inhoud van de uitgepakte map "usr" naar de map '/usr/' van de raspberry.
4. Kopieer de inhoud van de uitgepakte map "home-pi" naar de map '/home/pi/' van de raspberry.
5. type het commando 'sudo chmod 777 ~/Projecten'.
6. type het commando 'sudo chmod 777 ~/Projecten/OnzeApp'.
7. Type het commando 'sudo ldconfig'.
8. Sluit de verkenner.
9. Sluit de commando-shell.

Klaar om te programmeren:
--------------------
1. Start de verkenner.
2. Ga naar de map '/home/pi/Projecten'.
3. Klik met de rechter muisknop op het bestand 'OnzeApp.geany' en kies 'Geany'


# Installeer DeviceLib in de Arduino IDE

DeviceLib installeren:
------------------
1. Download het bestand 'DeviceLibArduino.zip' en pak het uit.
2. Er verschijnt een map 'DeviceLib'.
3. Kopieer de map 'DeviceLib' naar 'Documenten\Arduino\libraries'.

Klaar om te programmeren:
-------------------------
1. Start de 'Arduino IDE'.
