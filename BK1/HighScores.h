#pragma once
#include <vector>
#include <string>

class HighScore {
private:
    std::string filename;
    std::vector<int> scores;

public:
    HighScore(const std::string& filename);
    void addScore(int score);
    std::vector<int> getTopScores(int count) const;
    void saveScores() const;
    void loadScores();
};
