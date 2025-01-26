#include <iostream>

#include <SFML/Audio.hpp>

using std::cerr;
using std::cout;
using std::endl;

const int MAX_SOUNDS = 64;
int soundIdx = 0;
Sound sounds[MAX_SOUNDS];

SoundBuffer sfx[6];

const int SFX_ROCK = 0;
const int SFX_GOLD = 1;
const int SFX_CRYSTAL = 2;
const int SFX_CRYSTAL_B = 3;
const int SFX_WIN = 4;
const int SFX_DIE = 5;

void playSound(int id, double rate=1., double vol=1., double prob=1.) {
    if ((double)(rand()%1000)/1000. > prob) {
        return;
    }
    sounds[soundIdx].setBuffer(sfx[id]);
    sounds[soundIdx].setVolume(vol*100.);
    sounds[soundIdx].setPitch(rate + 0.05 * (((double)(rand() % 1000) / 1000.)-0.5));
    sounds[soundIdx].play();
    soundIdx = (soundIdx + 1) % MAX_SOUNDS;
}

bool loadSounds() {
    if (!sfx[SFX_ROCK].loadFromFile("sfx/rock.wav")) {
        cerr << "Unable to load sfx/rock.wav" << endl;
        return false;
    }
    if (!sfx[SFX_GOLD].loadFromFile("sfx/gold.wav")) {
        cerr << "Unable to load sfx/gold.wav" << endl;
        return false;
    }
    if (!sfx[SFX_CRYSTAL].loadFromFile("sfx/crystal.wav")) {
        cerr << "Unable to load sfx/crystal.wav" << endl;
        return false;
    }
    if (!sfx[SFX_CRYSTAL_B].loadFromFile("sfx/crystal-backwards.wav")) {
        cerr << "Unable to load sfx/crystal-backwards.wav" << endl;
        return false;
    }
    if (!sfx[SFX_WIN].loadFromFile("sfx/win.wav")) {
        cerr << "Unable to load sfx/win.wav" << endl;
        return false;
    }
    if (!sfx[SFX_DIE].loadFromFile("sfx/die.wav")) {
        cerr << "Unable to load sfx/die.wav" << endl;
        return false;
    }
    return true;
}