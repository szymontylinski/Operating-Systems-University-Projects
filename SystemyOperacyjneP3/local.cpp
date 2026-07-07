#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>

#define PORT 5555
#define CANDIDATES 3

// czas działania komisji (stały)
#define COMMISSION_TIME 10

std::vector<int> votes(CANDIDATES, 0);
std::mutex mtx;

int total = 0;

// liczba głosów do oddania w tej komisji
int votesToCast = 0;

// generator losowy
std::mt19937 gen(std::random_device{}());

// ===============================
// funkcja jednego wyborcy
// ===============================
void vote()
{
    while (true)
    {
        int id;

        {
            std::lock_guard<std::mutex> lock(mtx);

            if (total >= votesToCast)
                return;

            id = total++;
        }

        int c = gen() % CANDIDATES;

        {
            std::lock_guard<std::mutex> lock(mtx);
            votes[c]++;
        }
    }
}

int main()
{
    // losowanie parametrów komisji
    std::random_device rd;
    std::mt19937 gen(rd() ^ getpid());

    std::uniform_int_distribution<int> votersDist(300, 900);
    std::uniform_real_distribution<double> turnoutDist(0.1, 1.0);

    int voters = votersDist(gen);
    double turnout = turnoutDist(gen);

    votesToCast = static_cast<int>(voters * turnout);

    std::cout << "\nfrekwencja: " << turnout * 100 << "%\n";
    std::cout << "liczba wyborców: " << voters << std::endl;
    std::cout << "głosów do oddania: " << votesToCast << "\n\n";

    // start wyborów

    int THREADS = 4;
    std::vector<std::thread> votersThreads;

    for(int i = 0; i < THREADS; i++)
        votersThreads.emplace_back(vote);

    // komisja działa określony czas
    std::this_thread::sleep_for(std::chrono::seconds(COMMISSION_TIME));

    for(auto &t : votersThreads)
        t.join();

    // wysyłka do centrali

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);

    connect(sock,(sockaddr*)&addr,sizeof(addr));

    int buffer[CANDIDATES + 1];

    for(int i = 0; i < CANDIDATES; i++)
        buffer[i] = votes[i];

    buffer[CANDIDATES] = total;

    send(sock, buffer, sizeof(buffer), 0);
    close(sock);
}
