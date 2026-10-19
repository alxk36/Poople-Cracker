// Author: alxk36

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Normalize dictionary entries and user input so case does not affect lookup.
std::string lowercase(std::string word) {
    for (char& ch : word) {
        ch = static_cast<char>(
            std::tolower(static_cast<unsigned char>(ch))
        );
    }
    return word;
}

// Display the discovered path in the uppercase style used by the game.
std::string uppercase(std::string word) {
    for (char& ch : word) {
        ch = static_cast<char>(
            std::toupper(static_cast<unsigned char>(ch))
        );
    }
    return word;
}

// Return the shortest one-letter-change path, or an empty path if none exists.
std::vector<std::string> findShortestPath(
    const std::unordered_set<std::string>& dictionary,
    const std::string& start,
    const std::string& target) {
    std::queue<std::string> pending;
    std::unordered_map<std::string, std::string> parent;

    pending.push(start);
    parent[start] = "";

    while (!pending.empty()) {
        std::string current = pending.front();
        pending.pop();

        // Generate every possible word one letter away from the current word.
        for (int i = 0; i < 4; ++i) {
            char original = current[i];

            for (char letter = 'a'; letter <= 'z'; ++letter) {
                if (letter == original) continue;

                std::string next = current;
                next[i] = letter;

                if (dictionary.contains(next) && !parent.contains(next)) {
                    parent[next] = current;

                    if (next == target) {
                        // Follow parent links back to the start, then reverse them
                        // to return the route from the start to the target.
                        std::vector<std::string> path;
                        for (std::string at = target; !at.empty(); at = parent[at]) {
                            path.push_back(at);
                        }
                        std::reverse(path.begin(), path.end());
                        return path;
                    }

                    pending.push(next);
                }
            }
        }
    }

    return {};
}

int main() {
    // The bundled dictionary is the program's word source. Its upstream
    // provenance is not recorded; see README.md for that limitation.
    std::ifstream file("dictionary.txt");
    if (!file) {
        std::cerr << "Could not open dictionary.txt\n";
        return 1;
    }

    // Keep only unique, alphabetic four-letter entries for fast membership tests.
    std::unordered_set<std::string> dictionary;
    std::string word;

    while (file >> word) {
        word = lowercase(word);
        if (word.size() == 4 &&
            std::all_of(word.begin(), word.end(),
                        [](unsigned char ch) { return std::isalpha(ch); })) {
            dictionary.insert(word);
        }
    }

    // Read and validate the starting word supplied by the player.
    std::string start;
    std::cout << "Enter a 4-letter word: ";
    std::cin >> start;
    start = lowercase(start);

    if (start.size() != 4 ||
        !std::all_of(start.begin(), start.end(),
                     [](unsigned char ch) { return std::isalpha(ch); })) {
        std::cerr << "Please enter exactly four letters.\n";
        return 1;
    }

    // This helper targets POOP; it does not connect to or automate the website.
    const std::string target = "poop";

    if (start == target) {
        std::cout << uppercase(start) << '\n';
        return 0;
    }

    if (!dictionary.contains(target)) {
        std::cerr << "The dictionary must contain \"poop\".\n";
        return 1;
    }

    // BFS explores paths in increasing number of changes, so its result is shortest.
    const std::vector<std::string> path =
        findShortestPath(dictionary, start, target);
    if (path.empty()) {
        std::cout << "No path found.\n";
        return 0;
    }

    std::cout << "Shortest path (" << path.size() - 1 << " changes):\n";
    for (const auto& step : path) {
        std::cout << uppercase(step) << '\n';
    }
    return 0;
}