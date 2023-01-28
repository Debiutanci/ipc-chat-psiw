### Tworzenie kolejki komunikatów:  

```cq = msgget(0x123456, 0644 | IPC_CREAT);```  

Aplikacja korzysta z jednej kolejki komunikatów, struktura za pomocą której komunikuje się serwer z klientem zawiera informację o typie zapytania REQUEST|RESPONSE oraz o nadawcy|odbiorcy przez co infrastruktura decydująca o dalszym działaniu aplikacji może zwalidować czy request/response który przyszedł jest rzeczywiście tym który chcieliśmy otrzymać.

### Używane struktury:

- Group - struktura odpowiadająca za przechowywanie informacji o grupie (nazwa, identyfikator)
- User - struktura wykorzystywana do przechowywania informacji o użytkowniku, wykorzystywana jest przy odczycie z pliku (z bazy danych (txt))
- Request - struktura do komunikacji pomiędzy klientem i serwerem
- Message - struktura pomocnicza do przechowywania wiadomości w innych strukturach
- UserGetter - struktura pomocnicza do znajdywania użytkownika za pomocą identyfikatora lub adresu email
- UsersMessage - struktura odpowiedzialna za przechowywanie wiadomości w pamięci serwera
- UsersToGroups - struktura przechowująca informację o przynależności do grup
