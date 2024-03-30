# VUT IPK 1. projekt 2023-2024

## Obsah
1. [Návrh](#Návrh)
2. [Implementácia](#Implementácia)
3. [Testovanie](#Testovanie)
4. [Problémy](#Problémy)
5. [Zhrnutie](#Zhrnutie)
6. [Odkazy](#Odkazy)

## Návrh

Základom celého návrhu je trieda Controller. Táto trieda má dve varianty TCPController a UDPController pre rôzne protokoly. Účelom tejto triedy je riadiť celú komunikáciu pomocou konečného stavového automatu. Ďalšou triedou je Socket, ten zabezpečuje pripojenie k serveru rovnako ako odosielanie a príjmanie Packetov. Socket je jednotný pre UDP aj TCP. O použitom protokole je rozhodnuté parametrom. Trieda Packet má rovnako ako Controller UDP aj TCP variantu. Packet udržíava informácie o packete a umožnuje zostrojiť správu pre server. O spracuvávanie vstupov a ich verifikáciu, či už príkazov z klávesnice alebo prichádzajúcivh správ zo servera sa stará Parser.

![Diagram tried]()

## Implementácia

Najprv bolo naimplenetované správanie aplikácie pre protokol TCP s ohľadom na znovupoužiteľnosťi v UDP. Po otestovaní funkčnosti TCP bola aplikácia rozšírená o UDP variantu. S touto variantou prišla potreba rozšíriť projekt o ďalšiu funkcionalitu, čo obnášalo prerobenie niektorých Tried pre TCP. Nakoniec bola aplikácia upravená aby spĺňala špecifikáciu projektu, teda upravenie Makefile a validné spracuvávanie argumentov aplikácie. 

## Testovanie

Testovanie aplikácie bolo vykonávané prevažne manuálne pomocou aplikácií Wireshark a netcat. Kontrolované boli vstupné dáta od uživateľa a taktiež prichadzajúce dáta zo servera. Referenčnými vstupmy a výtupmi bolo zadanie projketu a súbory so zachytenou komunikáciou v ňom. Taktiež boli ako vstupy v testovaní použité vymyslené nevalidné a náhodné dáta. Toto testovanie slúžilo primárne na zistenie odolnosti aplikácie voči nevalidným vstupom. Pri kontole UDP bol použitý python script slúžiaci na otestovanie niektorých problémov spojených s variantou UDP. Tento script není úplne automatický, slúži len na čiastočné overenie funkcionality programátorom. Bohužiaľ nebolo možné testami pokryť všetky situácie, ktoré môžu nastať, teda akákoľvek situácia, ktorá nie je v zadaní projektu jednoznačne definovaná môže viesť k nešpecifikovanému správaniu.

## Problémy

Najväčším problémom bolo pokrytie čo najväčšieho množstva zadaním nedefinovaných situácií, ktoré môžu nastať pri komunikácii so vzdialeným serverom a ich následné spracovanie. Problematickým bolo tiež testovanie UDP varianty, keďže tá narozdiel od TCP pracovala s binárnimy dátami, ktoré bol problém zobraziť spolu s tými textovými. Vysoká náročnosť projketu si vyžadovala dôkladný návrh a väščiu námahu premysleniu pred samotným písaním kódu. V poslendom rade sa medzi problémami ocitla aj neskúsenosť s jazykom C++ a nevyužitie jeho plného potenciálu, čo malo za následok zníženú znovupoužiteľno kódu v objektovo orientovanom princípe a menšiu všeobecnosť.

## Zhrnutie

Pre úplné pochopenie projektu bolo potrebné naštudovať ako funguje komunikácia medzi zariadeniami a taktiež odlišnosti medzi protokolmi TCP a UDP. Zaujímavím zistením boli rozdiely v týchto protokoloch a všetky výhody aj nevýhody ktoré obnášajú. Ďalšou výzvou bolo zabezpečenie aplikácie voči zamrznutiu, keďže je nutné naraz byť schopný čítať dáta zo vstupu aj zo serveru. Veľkou výzvou bolo tiež testovanie UDP varianty, keď bolo potrebné simulovať problémy, ktoré môžu pri UDP nastať a tiež ich správne spracovanie v aplikácii.

## Odkazy

[Zadanie projektu](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%201)

[C++ dokumentácia](https://en.cppreference.com/w/)

[Wireshark](https://www.wireshark.org/)

[NetCat](https://nc110.sourceforge.io/)
