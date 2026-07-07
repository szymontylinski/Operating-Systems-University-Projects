#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>

// port, na którym komisja centralna nasłuchuje komisji lokalnych
#define PORT 5555

// liczba kandydatów w wyborach
#define CANDIDATES 3

// mutex chroniący dostęp do wspólnych danych
std::mutex mtx;

// globalna tablica wyników – suma głosów z wszystkich komisji
std::vector<int> globalVotes(CANDIDATES, 0);

// suma wszystkich głosów odebranych ze wszystkich komisji
int totalReceived = 0;


// funkcja obsługująca jedną komisję lokalną
// działa w osobnym wątku
void handleClient(int clientSock)
{
    int localSum = 0;                       // suma głosów z jednej komisji
    int buffer[CANDIDATES + 1];            // bufor na dane z komisji
                                           // ostatnia komórka to suma lokalna

    // odbiór pakietu danych z komisji lokalnej
    recv(clientSock, buffer, sizeof(buffer), 0);

    // blokujemy mutex, żeby bezpiecznie aktualizować wspólne dane
    std::lock_guard<std::mutex> lock(mtx);

    // dodajemy gło#include <unistd.h>
#include <iostream>

    int main() {
        for (int i = 0; i < 5; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                execl("./local", "local", nullptr);
            }
        }
        return 0;
    }
    sy z tej komisji
    do wyników globalnych
    for(int i = 0; i < CANDIDATES; i++)
    {
        globalVotes[i] += buffer[i];
        localSum += buffer[i];
    }

    // aktualizujemy globalną sumę głosów
    totalReceived += buffer[CANDIDATES];

    std::cout << "Odebrano komisję, głosy: " << localSum << std::endl;

    // zamykamy połączenie z komisją lokalną
    close(clientSock);
}

int main()
{
    // tworzymy gniazdo TCP
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    // struktura adresu serwera
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);        // port serwera
    addr.sin_addr.s_addr = INADDR_ANY;  // nasłuch na wszystkich interfejsach

    // przypisanie adresu do gniazda
    bind(serverSock, (sockaddr*)&addr, sizeof(addr));

    // uruchomienie nasłuchiwania
    listen(serverSock, 10);

    std::cout << "Komisja centralna czeka na komisje...\n";

    std::vector<std::thread> threads;

    // oczekujemy na 5 komisji lokalnych
    for(int i = 0; i < 5; i++)
    {
        int client = accept(serverSock, nullptr, nullptr);

        // każdą komisję obsługujemy w osobnym wątku
        threads.emplace_back(handleClient, client);
    }

    // czekamy aż wszystkie wątki zakończą pracę
    for(auto &t : threads) t.join();

    // wyświetlenie wyników
    std::cout << "\n=== WYNIKI WYBORÓW ===\n";
    for(int i = 0; i < CANDIDATES; i++)
        std::cout << "Kandydat " << i << ": " << globalVotes[i] << std::endl;

    std::cout << "Łącznie odebranych głosów: " << totalReceived << std::endl;

    // Weryfikacja poprawności danych – suma kontrolna
    int control = 0;
    for(int v : globalVotes)
        control += v;

    std::cout << "\n=== weryfikacja poprawnosci ===\n";
    std::cout << "Suma z kandydatów: " << control << std::endl;
    std::cout << "Suma raportowana: " << totalReceived << std::endl;

    if(control == totalReceived)
        std::cout << "STATUS: Wszystkie głosy zostały poprawnie zliczone\n";
    else
        std::cout << "STATUS: utracono  dane!\n";
}
