# De library

In deze folder bevinden zich de code-bestanden van de library.
Een aantal bestanden zijn specifiek voor de arduino en aantal zijn
alleen voor de Raspberry. Hoewel het mogelijk is de bestanden uit deze
folder te downloaden en alle voorbereiding verder handmatig te doen
(dit wordt beschreven in de handleiding), is het aan te raden de
gecomprimeerde library in de folder 'Installeren' te gebruiken.

#Raspberry of Arduino

In pricipe zijn de programma's voor de raspberry en de arduino uitwisselbaar,
maar er gelden enkele restricties. Op de raspberry draait een besturings-
systeem en op de arduino niet. Dit levert enkele kenmerkende verschillen op.
De library-versie voor de raspberry bevat een scherm-module om vensters
te maken en muis-klikken af te handelen. Dit is op de arduino niet mogelijk.
Verder is de module van de tcp-server alleen voor de raspberry gemaakt,
waarbij de werkelijke communicatie met de client aan het besturingssysteem
wordt overgelaten. Daarentegen staat het wifi-shield van de arduino maar één
connectie met een client toe, wat teveel van de raspberry-opzet afwijkt en
daarom niet in de library is opgenomen.

Behalve deze voordelen heeft een besturingssysteem ook een groot nadeel:
je kunt er niet real-time mee programmeren. Interrupts van het besturings-
systeem onderbreken een programma voortdurend. Dit heeft met name gevolgen
voor de pwm (pulse wide modulation) waarmee de helderheid van leds en de 
snelheid van motoren wordt geregeld. Omdat de arduino een mooie
gelijkmatige puls geeft, branden leds daar werkelijk minder fel, terwijl je
op de raspberry zo nu en dan geknipper waarneemt.