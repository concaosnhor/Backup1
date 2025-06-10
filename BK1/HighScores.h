#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <string>
#include <vector>
#include <utility>

class HighScore {
private:
    std::string filename;
    std::vector<std::pair<std::string, int>> scores;

public:
    // Constructor: nhận tên file lưu điểm
    HighScore(const std::string& filename);

    // Thêm điểm mới
    void addScore(const std::string& name, int score);

    // Trả về top N điểm cao nhất
    std::vector<std::pair<std::string, int>> getTopScores(int count) const;

    // Lưu điểm hiện tại xuống file
    void saveScores() const;

    // Tải điểm từ file lên vector
    void loadScores();
};

#endif // HIGHSCORES_H
