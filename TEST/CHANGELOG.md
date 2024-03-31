# Change Log

### funkcionalita

* Aplikácia by mala spĺňať všetky implementačné požiadavky zadania s vínimkou správania na neočakávané vstupy (z klávesnice/serveru), ktoré neboli otestované

### limitácie

* Aplikácia nie je odolná voči neočakávaným vstupom zo vzdialeného serveru, ktoré nie su špecifikované v zadaní
* Aplikácia v režime UDP očakáva po každom odoslanom packete, ako prvý prichádzajúci packet CONFIRM
* Aplikácia nie je odolná voči útokom zo siete