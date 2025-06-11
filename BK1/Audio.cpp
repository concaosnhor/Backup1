#include "Audio.h"
#include <iostream>

bool Audio::loadSound(const std::string& name, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound file: " << filename << std::endl;
        return false;
    }

    buffers[name] = buffer;

    sf::Sound sound;
    sound.setBuffer(buffers[name]);
    sounds[name] = sound;

    return true;
}

void Audio::playSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.play();
    }
    else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}

void Audio::stopSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.stop();
    }
}

void Audio::stopAllSounds() {
    for (auto& pair : sounds) {
        pair.second.stop();
    }
}

bool Audio::playMusic(const std::string& filename, bool loop) {
    if (!backgroundMusic.openFromFile(filename)) {
        std::cerr << "Failed to load music file: " << filename << std::endl;
        return false;
    }
    backgroundMusic.setLoop(loop);
    backgroundMusic.play();
    return true;
}

void Audio::stopMusic() {
    backgroundMusic.stop();
}

void Audio::pauseMusic() {
    backgroundMusic.pause();
}

void Audio::resumeMusic() {
    backgroundMusic.play();
}

void Audio::setMusicVolume(float volume) {
    backgroundMusic.setVolume(volume); // volume từ 0 đến 100
}
