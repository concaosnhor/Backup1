#include "HighScores.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>

HighScore::HighScore(const std::string& filename) : filename(filename) {
    loadScores();  // Load dữ liệu khi khởi tạo
}

void HighScore::addScore(int score) {
    scores.push_back(score);

    // Sắp xếp giảm dần theo điểm
    std::sort(scores.begin(), scores.end(), std::greater<int>());

    // Giữ tối đa 10 điểm cao nhất
    if (scores.size() > 10) {
        scores.resize(10);
    }
}

std::vector<int> HighScore::getTopScores(int count) const {
    if (count >= scores.size()) return scores;
    return std::vector<int>(scores.begin(), scores.begin() + count);
}

void HighScore::saveScores() const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename << "\n";
        return;
    }

    for (int score : scores) {
        outFile << score << "\n";
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

    int score;
    while (inFile >> score) {
        scores.push_back(score);
    }

    inFile.close();

    // Sắp xếp giảm dần
    std::sort(scores.begin(), scores.end(), std::greater<int>());
}
