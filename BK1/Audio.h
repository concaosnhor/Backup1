#ifndef AUDIO_H
#define AUDIO_H

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class Audio {
private:
    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::unordered_map<std::string, sf::Sound> sounds;

    sf::Music backgroundMusic;  // Nhạc nền

public:
    Audio() = default;
    ~Audio() = default;

    bool loadSound(const std::string& name, const std::string& filename);
    void playSound(const std::string& name);
    void stopSound(const std::string& name);
    void stopAllSounds();

    // Nhạc nền
    bool playMusic(const std::string& filename, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume); // 0 - 100
};

#endif // AUDIO_H
