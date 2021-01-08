# DeviceLib

DeviceLib is een algemene C++ library voor de arduino en de raspberry.
De library bevat klassen voor de aansturing van diverse sensoren en actuatoren.
Alle klassen baseren zij zich op drie basis klassen: Device, Actuator en Sensor.
Daarmee wordt een standaard aangebracht voor het programmeren.
Sensoren worden uitgelezen met de routine 'read', waarna een meer specifieke
routine (bijvoorbeeld 'celcius' bij een temperatuursensor) de waarde teruggeeft.
Actuatoren worden eerst ingesteld met een specifieke routine (bijvoorbeeld 'setSpeed'
voor een motor) en daarna geactiveerd met de routine 'on'.
In de map 'Documenten' bevindt zich een uitgebreide handleiding.

DeviceLib is opgezet om 'maker'-projecten in het onderwijs te ondersteunen.
Hoewel het gebruik van sensoren en actuatoren de leerlingen sterk motiveert,
vormt het onoverzichtelijke aanbod van deze devices en de verwarrende
manieren van aansturen een obstakel om eraan te beginnen. Daarbij bestaan er
verschillende type mini-computertjes die op verschillende manieren kunnen worden
geprogrammeerd. DeviceLib voorziet in de broodnodige eenheid. Als C++ library
is hij geschikt voor zowel de Raspberry (met Geany) als de Arduino en compatible
boards (via de Arduino-IDE).

Behalve de standaard voor het aansturen van sensoren en actuatoren, hanteert
de library bovendien een standaard voor het opzetten van een programma.
Daarbij is de aanpak van de 'ino'-bestanden (Arduino-IDE) als leidraad genomen.
Ook op de Raspberry begint een programma met '#include <Arduino.h>' en bevat het
een 'setup'-routine en een 'loop'-routine. Deze structuur biedt de leerlingen
houvast bij het programmeren en versimpelt de program flow.
