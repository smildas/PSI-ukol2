# Popis implementace

Jednoduchý vícevláknový http server. Vrací na přijaté požadavky, **HTTP OK (200)** když přijde validní HTTP GET zpráva a pošle data s jednoduchou stránkou s nápisem a výpisem časových údajů. 
Pokud přijde nevalidní HTTP get zpráva posílá **HTTP Bad Request (400)** jako odpověď.

## Překlad

Překlad zdrojových soubrů se provadí pomocí nástroje make v adresáři, kde jsou umístěné i zdrojové soubory.

## Spuštení

Program je spustitelný pouze na systémech s linuxovým jádrem.

Program se spouští z příkazové řádky následujícím způsobem.


```shell
./simpleHttp <ip address> <port>
```

* ip address - IPv4 adresa (stroje) na které bude server komunikovat
* port - celé číslo z rozsahu 1 - 65535.

U obou parametrů se předpokladá že hodnota bude řetězec, proto je potřeba umístit oba parametry do uvozovek *"*.
