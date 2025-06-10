#include "HighScores.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

HighScore::HighScore(const std::string& filename) : filename(filename) {
    loadScores();  // Tự động load khi khởi tạo
}

void HighScore::addScore(const std::string& name, int score) {
    scores.emplace_back(name, score);

    // Sắp xếp giảm dần theo điểm
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });

    // Giữ tối đa 10 điểm cao nhất
    if (scores.size() > 10) {
        scores.resize(10);
    }
}

std::vector<std::pair<std::string, int>> HighScore::getTopScores(int count) const {
    if (count >= scores.size()) return scores;
    return std::vector<std::pair<std::string, int>>(scores.begin(), scores.begin() + count);
}

void HighScore::saveScores() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename << "\n";
        return;
    }

    for (const auto& entry : scores) {
        outFile << entry.first << " " << entry.second << "\n";
    }
    outFile.close();
}

void HighScore::loadScores() {
    scores.clear();
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Unable to open file for reading: " << filename << "\n";
        return;
    }
    
    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string name;
        int score;
        if (iss >> name >> score) {
            scores.emplace_back(name, score);
        }
    }

    inFile.close();

    // Sắp xếp lại
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });
}
