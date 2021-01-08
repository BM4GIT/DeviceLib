# Installeer DeviceLib op de Raspberry

DeviceLib installeren:
------------------
1. Zorg dat ook de libraries voor 'bcm2538', 'mysql-client', 'gtk-3' en 'opencv' zijn geïnstalleerd
2. Download het bestand "DeviceLib_Raspberry.tar" en pak het uit.
3. Er verschijnen twee mappen:  "home-pi" en "usr".
4. Open de commando-shell.
5. Type het commando 'sudo pcmanfm'. De verkenner opent met superuser-rechten.
6. Kopieer de inhoud van de uitgepakte map "usr" naar de map "/usr/" van de raspberry.
7. Kopieer de inhoud van de uitgepakte map "home-pi" naar de map "/home/pi/" van de raspberry.
8. Sluit de verkenner.
9. type het commando 'sudo chmod 777 ~/Projecten'.
10. type het commando 'sudo chmod 777 ~/Projecten/OnzeApp'.
11. Type het commando 'sudo ldconfig'.
12. Sluit de commando-shell.

Klaar om te programmeren:
--------------------
1. Start de verkenner.
2. Ga naar de map "/home/pi/Projecten".
3. Klik met de rechter muisknop op het bestand "OnzeApp.geany" en kies 'Geany'


# Installeer DeviceLib in de Arduino IDE

DeviceLib installeren:
------------------
1. Download het bestand "DeviceLib_Arduino.zip" en pak het uit.
2. Er verschijnt een map "DeviceLib".
3. Kopieer de map "DeviceLib" naar "Documenten\Arduino\libraries\".

Klaar om te programmeren:
--------------------
1. Start de 'Arduino IDE'.
