# ipc-chat-psiw

### Kompilacja

Należy wykonać w terminalu komendę ```make```  


### Instrukcja uruchomienia

Aby uruchomić server należy wykonać komendę ```./server.out``` natomiast aby uruchomić client'a należy wykonać komendę ```./client.out```. Serwer należy uruchamiać w pierwszej kolejności ponieważ client jest wyposażony w funkcjonalność sprawdzania połączenia z serverem, więc jeżeli sprawdzi, że nie może nawiązać połączenia automatycznie się wyłączy.  
Chcąc się zalogować trzeba sprawdzić dane logowania użytkowników z bazy danych (plik users.txt)

### Krótki opis plików

- server.c - Plik zawierający implementację serwera  
- client.c - Plik zawierający implementację klienta  
