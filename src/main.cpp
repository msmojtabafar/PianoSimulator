#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

struct PianoKey {
    sf::RectangleShape shape;
    std::string note;
    bool black;
    bool pressed;
};

struct PianoPedal {
    sf::RectangleShape shape;
    sf::Text label;
    bool pressed;
};

struct PlayingNote {
    std::unique_ptr<sf::Sound> sound;
    std::string note;
    sf::Keyboard::Key key;

    bool keyHeld;
    bool sostenutoCaptured;
};

int noteToMidi(const std::string& note) {
    if (note.size() < 2)
        return -1;

    int semitone = 0;

    switch (note[0]) {
        case 'C': semitone = 0; break;
        case 'D': semitone = 2; break;
        case 'E': semitone = 4; break;
        case 'F': semitone = 5; break;
        case 'G': semitone = 7; break;
        case 'A': semitone = 9; break;
        case 'B': semitone = 11; break;
        default:
            return -1;
    }

    int index = 1;

    if (note[index] == '#') {
        semitone++;
        index++;
    }

    if (index >= static_cast<int>(note.size()))
        return -1;

    int octave = note[index] - '0';

    if (octave < 0 || octave > 9)
        return -1;

    return (octave + 1) * 12 + semitone;
}

std::pair<const sf::SoundBuffer*, float>
findBestSample(
    const std::string& targetNote,
    const std::map<std::string, sf::SoundBuffer>& buffers
) {
    int targetMidi = noteToMidi(targetNote);

    const sf::SoundBuffer* bestBuffer = nullptr;

    int bestDistance = 9999;
    std::string bestNote;

    for (const auto& [sampleNote, buffer] : buffers) {

        int sampleMidi = noteToMidi(sampleNote);

        if (sampleMidi < 0)
            continue;

        int distance =
            std::abs(targetMidi - sampleMidi);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestBuffer = &buffer;
            bestNote = sampleNote;
        }
    }

    if (!bestBuffer)
        return {nullptr, 1.0f};

    float pitch =
        std::pow(
            2.0f,
            static_cast<float>(
                targetMidi - noteToMidi(bestNote)
            ) / 12.0f
        );

    return {bestBuffer, pitch};
}

void playNote(
    const std::string& note,
    sf::Keyboard::Key key,
    const std::map<std::string, sf::SoundBuffer>& buffers,
    std::vector<PlayingNote>& activeNotes,
    bool softPedal
) {
    auto result =
        findBestSample(note, buffers);

    if (!result.first)
        return;

    auto sound =
        std::make_unique<sf::Sound>();

    sound->setBuffer(*result.first);

    sound->setPitch(result.second);

    if (softPedal)
        sound->setVolume(55.f);
    else
        sound->setVolume(100.f);

    sound->play();

    std::cout
        << note
        << " -> pitch="
        << result.second;

    if (softPedal)
        std::cout << " | SOFT";

    std::cout << std::endl;

    PlayingNote playing;

    playing.sound = std::move(sound);
    playing.note = note;
    playing.key = key;
    playing.keyHeld = true;
    playing.sostenutoCaptured = false;

    activeNotes.push_back(
        std::move(playing)
    );
}

int main() {

    // --------------------------------------------------
    // Window
    // --------------------------------------------------

    sf::RenderWindow window(
        sf::VideoMode(1500, 700),
        "Piano Simulator"
    );

    window.setFramerateLimit(60);


    // --------------------------------------------------
    // Font
    // --------------------------------------------------

    sf::Font font;

    if (!font.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    )) {
        std::cerr
            << "Failed to load font."
            << std::endl;

        return 1;
    }


    // --------------------------------------------------
    // Load samples
    // --------------------------------------------------

    std::map<std::string, sf::SoundBuffer> buffers;

    std::vector<std::string> sampleNotes = {

        "A0",
        "A1",
        "A2",
        "A3",
        "A4",
        "A5",
        "A6",
        "A7",

        "C1",
        "C2",
        "C3",
        "C4",
        "C5",
        "C6",
        "C7",
        "C8",

        "D#1",
        "D#2",
        "D#3",
        "D#4",
        "D#5",
        "D#6",
        "D#7",

        "F#1",
        "F#2",
        "F#3",
        "F#4",
        "F#5",
        "F#6",
        "F#7"
    };

    for (const auto& note : sampleNotes) {

        sf::SoundBuffer buffer;

        std::string path =
            "../sounds/" + note + ".wav";

        if (!buffer.loadFromFile(path)) {

            std::cerr
                << "Failed to load: "
                << path
                << std::endl;

            continue;
        }

        buffers.emplace(
            note,
            std::move(buffer)
        );
    }

    std::cout
        << "Loaded samples: "
        << buffers.size()
        << std::endl;


    // --------------------------------------------------
    // Piano keys
    // --------------------------------------------------

    std::vector<PianoKey> keys;

    float startX = 40.f;
    float startY = 170.f;

    float whiteWidth = 75.f;
    float whiteHeight = 400.f;

    std::vector<std::string> whiteNotes = {

        "C4",
        "D4",
        "E4",
        "F4",
        "G4",
        "A4",
        "B4",

        "C5",
        "D5",
        "E5",
        "F5",
        "G5",
        "A5",
        "B5"
    };


    // White keys

    for (size_t i = 0;
         i < whiteNotes.size();
         ++i) {

        PianoKey key;

        key.shape.setSize(
            sf::Vector2f(
                whiteWidth - 2.f,
                whiteHeight
            )
        );

        key.shape.setPosition(
            startX + i * whiteWidth,
            startY
        );

        key.shape.setFillColor(
            sf::Color(245, 245, 245)
        );

        key.shape.setOutlineColor(
            sf::Color::Black
        );

        key.shape.setOutlineThickness(
            1.f
        );

        key.note = whiteNotes[i];
        key.black = false;
        key.pressed = false;

        keys.push_back(key);
    }


    // Black keys

    std::vector<std::pair<int, std::string>>
        blackKeyData = {

        {0, "C#4"},
        {1, "D#4"},

        {3, "F#4"},
        {4, "G#4"},
        {5, "A#4"},

        {7, "C#5"},
        {8, "D#5"},

        {10, "F#5"},
        {11, "G#5"},
        {12, "A#5"}
    };

    for (const auto& [index, note] :
         blackKeyData) {

        PianoKey key;

        float blackWidth = 48.f;
        float blackHeight = 250.f;

        key.shape.setSize(
            sf::Vector2f(
                blackWidth,
                blackHeight
            )
        );

        key.shape.setPosition(
            startX
                + (index + 1) * whiteWidth
                - blackWidth / 2.f,
            startY
        );

        key.shape.setFillColor(
            sf::Color(25, 25, 25)
        );

        key.shape.setOutlineColor(
            sf::Color::Black
        );

        key.shape.setOutlineThickness(
            1.f
        );

        key.note = note;
        key.black = true;
        key.pressed = false;

        keys.push_back(key);
    }


    // --------------------------------------------------
    // Keyboard mapping
    // --------------------------------------------------

    std::map<
        sf::Keyboard::Key,
        std::string
    > keyboardMap = {

        {sf::Keyboard::A, "C4"},
        {sf::Keyboard::S, "D4"},
        {sf::Keyboard::D, "E4"},
        {sf::Keyboard::F, "F4"},
        {sf::Keyboard::G, "G4"},
        {sf::Keyboard::H, "A4"},
        {sf::Keyboard::J, "B4"},

        {sf::Keyboard::K, "C5"},
        {sf::Keyboard::L, "D5"},
        {sf::Keyboard::Z, "E5"},
        {sf::Keyboard::X, "F5"},
        {sf::Keyboard::C, "G5"},
        {sf::Keyboard::V, "A5"},
        {sf::Keyboard::B, "B5"}
    };


    // --------------------------------------------------
    // Pedals
    // --------------------------------------------------

    std::vector<PianoPedal> pedals;

    std::vector<std::string> pedalNames = {
        "LEFT",
        "MIDDLE",
        "RIGHT"
    };

    float pedalX = 610.f;
    float pedalY = 610.f;

    for (int i = 0; i < 3; ++i) {

        PianoPedal pedal;

        pedal.shape.setSize(
            sf::Vector2f(
                90.f,
                55.f
            )
        );

        pedal.shape.setPosition(
            pedalX + i * 95.f,
            pedalY
        );

        pedal.shape.setFillColor(
            sf::Color(80, 80, 80)
        );

        pedal.shape.setOutlineColor(
            sf::Color::Black
        );

        pedal.shape.setOutlineThickness(
            2.f
        );

        pedal.label.setFont(font);

        pedal.label.setString(
            pedalNames[i]
        );

        pedal.label.setCharacterSize(
            16
        );

        pedal.label.setFillColor(
            sf::Color::White
        );

        sf::FloatRect bounds =
            pedal.label.getLocalBounds();

        pedal.label.setOrigin(
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f
        );

        pedal.label.setPosition(
            pedalX
                + i * 95.f
                + 45.f,
            pedalY + 27.f
        );

        pedal.pressed = false;

        pedals.push_back(pedal);
    }


    // --------------------------------------------------
    // Text
    // --------------------------------------------------

    sf::Text title;

    title.setFont(font);
    title.setString("PIANO SIMULATOR");
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::White);
    title.setPosition(40.f, 30.f);


    sf::Text currentNote;

    currentNote.setFont(font);
    currentNote.setString("Ready");
    currentNote.setCharacterSize(22);
    currentNote.setFillColor(sf::Color::White);
    currentNote.setPosition(40.f, 90.f);


    // --------------------------------------------------
    // Pedal states
    // --------------------------------------------------

    bool sustainPedal = false;
    bool softPedal = false;
    bool sostenutoPedal = false;


    // --------------------------------------------------
    // Active sounds
    // --------------------------------------------------

    std::vector<PlayingNote>
        activeNotes;


    // --------------------------------------------------
    // Main loop
    // --------------------------------------------------

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            // ------------------------------------------
            // Close
            // ------------------------------------------

            if (event.type ==
                sf::Event::Closed) {

                window.close();
            }


            // ------------------------------------------
            // Keyboard pressed
            // ------------------------------------------

            if (event.type ==
                sf::Event::KeyPressed) {

                auto key =
                    event.key.code;


                // --------------------------------------
                // RIGHT SHIFT = SUSTAIN
                // --------------------------------------

                if (key ==
                    sf::Keyboard::RShift) {

                    if (!sustainPedal) {

                        sustainPedal = true;

                        pedals[2].pressed = true;

                        pedals[2].shape.move(
                            0.f,
                            8.f
                        );

                        std::cout
                            << "SUSTAIN ON"
                            << std::endl;
                    }

                    continue;
                }


                // --------------------------------------
                // LEFT SHIFT = SOFT
                // --------------------------------------

                if (key ==
                    sf::Keyboard::LShift) {

                    if (!softPedal) {

                        softPedal = true;

                        pedals[0].pressed = true;

                        pedals[0].shape.move(
                            0.f,
                            8.f
                        );

                        std::cout
                            << "SOFT ON"
                            << std::endl;
                    }

                    continue;
                }


                // --------------------------------------
                // SPACE = SOSTENUTO
                // --------------------------------------

                if (key ==
                    sf::Keyboard::Space) {

                    if (!sostenutoPedal) {

                        sostenutoPedal = true;

                        pedals[1].pressed = true;

                        pedals[1].shape.move(
                            0.f,
                            8.f
                        );

                        // Capture every note
                        // currently sounding.

                        for (auto& playing :
                             activeNotes) {

                            if (
                                playing.sound &&
                                playing.sound->getStatus()
                                == sf::Sound::Playing
                            ) {

                                playing.sostenutoCaptured =
                                    true;
                            }
                        }

                        std::cout
                            << "SOSTENUTO ON"
                            << std::endl;
                    }

                    continue;
                }


                // --------------------------------------
                // Piano keys
                // --------------------------------------

                auto mapIt =
                    keyboardMap.find(key);

                if (mapIt != keyboardMap.end()) {

                    // Ignore auto-repeat

                    bool alreadyHeld = false;

                    for (const auto& playing :
                         activeNotes) {

                        if (
                            playing.key == key &&
                            playing.keyHeld
                        ) {

                            alreadyHeld = true;
                            break;
                        }
                    }

                    if (alreadyHeld)
                        continue;


                    // Find matching visual key

                    for (auto& pianoKey : keys) {

                        if (
                            pianoKey.note ==
                            mapIt->second
                        ) {

                            pianoKey.pressed = true;

                            if (pianoKey.black)
                                pianoKey.shape.setFillColor(
                                    sf::Color(80, 80, 80)
                                );
                            else
                                pianoKey.shape.setFillColor(
                                    sf::Color(180, 180, 180)
                                );
                        }
                    }


                    playNote(
                        mapIt->second,
                        key,
                        buffers,
                        activeNotes,
                        softPedal
                    );

                    currentNote.setString(
                        mapIt->second
                    );
                }
            }


            // ------------------------------------------
            // Keyboard released
            // ------------------------------------------

            if (event.type ==
                sf::Event::KeyReleased) {

                auto key =
                    event.key.code;


                // --------------------------------------
                // RIGHT SHIFT RELEASE
                // --------------------------------------

                if (key ==
                    sf::Keyboard::RShift) {

                    sustainPedal = false;

                    pedals[2].pressed = false;

                    pedals[2].shape.setPosition(
                        pedalX + 2 * 95.f,
                        pedalY
                    );

                    // Stop notes that are no longer
                    // physically held and are not
                    // protected by Sostenuto.

                    for (auto it =
                         activeNotes.begin();
                         it != activeNotes.end();) {

                        if (
                            !it->keyHeld &&
                            !it->sostenutoCaptured
                        ) {

                            it->sound->stop();

                            it =
                                activeNotes.erase(it);

                        } else {

                            ++it;
                        }
                    }

                    std::cout
                        << "SUSTAIN OFF"
                        << std::endl;

                    continue;
                }


                // --------------------------------------
                // LEFT SHIFT RELEASE
                // --------------------------------------

                if (key ==
                    sf::Keyboard::LShift) {

                    softPedal = false;

                    pedals[0].pressed = false;

                    pedals[0].shape.setPosition(
                        pedalX,
                        pedalY
                    );

                    std::cout
                        << "SOFT OFF"
                        << std::endl;

                    continue;
                }


                // --------------------------------------
                // SPACE RELEASE
                // --------------------------------------

                if (key ==
                    sf::Keyboard::Space) {

                    sostenutoPedal = false;

                    pedals[1].pressed = false;

                    pedals[1].shape.setPosition(
                        pedalX + 95.f,
                        pedalY
                    );

                    // Release all notes captured
                    // by Sostenuto.

                    for (auto it =
                         activeNotes.begin();
                         it != activeNotes.end();) {

                        if (it->sostenutoCaptured) {

                            it->sostenutoCaptured =
                                false;

                            if (
                                !it->keyHeld &&
                                !sustainPedal
                            ) {

                                it->sound->stop();

                                it =
                                    activeNotes.erase(
                                        it
                                    );

                                continue;
                            }
                        }

                        ++it;
                    }

                    std::cout
                        << "SOSTENUTO OFF"
                        << std::endl;

                    continue;
                }


                // --------------------------------------
                // Piano key release
                // --------------------------------------

                auto mapIt =
                    keyboardMap.find(key);

                if (mapIt != keyboardMap.end()) {

                    // Visual key release

                    for (auto& pianoKey : keys) {

                        if (
                            pianoKey.note ==
                            mapIt->second
                        ) {

                            pianoKey.pressed = false;

                            if (pianoKey.black)
                                pianoKey.shape.setFillColor(
                                    sf::Color(25, 25, 25)
                                );
                            else
                                pianoKey.shape.setFillColor(
                                    sf::Color(245, 245, 245)
                                );
                        }
                    }


                    // Find active note

                    for (auto it =
                         activeNotes.begin();
                         it != activeNotes.end();) {

                        if (
                            it->key == key &&
                            it->keyHeld
                        ) {

                            it->keyHeld = false;


                            // Stop only when neither
                            // Sustain nor Sostenuto
                            // is holding the note.

                            if (
                                !sustainPedal &&
                                !it->sostenutoCaptured
                            ) {

                                it->sound->stop();

                                it =
                                    activeNotes.erase(
                                        it
                                    );

                                continue;
                            }
                        }

                        ++it;
                    }
                }
            }


            // ------------------------------------------
            // Mouse pressed
            // ------------------------------------------

            if (event.type ==
                sf::Event::MouseButtonPressed) {

                if (
                    event.mouseButton.button ==
                    sf::Mouse::Left
                ) {

                    sf::Vector2f mousePos(
                        event.mouseButton.x,
                        event.mouseButton.y
                    );


                    // Check black keys first

                    bool clicked = false;

                    for (auto& pianoKey : keys) {

                        if (
                            pianoKey.black &&
                            pianoKey.shape.getGlobalBounds()
                                .contains(mousePos)
                        ) {

                            pianoKey.pressed = true;

                            pianoKey.shape.setFillColor(
                                sf::Color(80, 80, 80)
                            );

                            playNote(
                                pianoKey.note,
                                sf::Keyboard::Unknown,
                                buffers,
                                activeNotes,
                                softPedal
                            );

                            currentNote.setString(
                                pianoKey.note
                            );

                            clicked = true;

                            break;
                        }
                    }


                    // Then white keys

                    if (!clicked) {

                        for (auto& pianoKey : keys) {

                            if (
                                !pianoKey.black &&
                                pianoKey.shape
                                    .getGlobalBounds()
                                    .contains(mousePos)
                            ) {

                                pianoKey.pressed = true;

                                pianoKey.shape.setFillColor(
                                    sf::Color(180, 180, 180)
                                );

                                playNote(
                                    pianoKey.note,
                                    sf::Keyboard::Unknown,
                                    buffers,
                                    activeNotes,
                                    softPedal
                                );

                                currentNote.setString(
                                    pianoKey.note
                                );

                                break;
                            }
                        }
                    }
                }
            }


            // ------------------------------------------
            // Mouse released
            // ------------------------------------------

            if (event.type ==
                sf::Event::MouseButtonReleased) {

                if (
                    event.mouseButton.button ==
                    sf::Mouse::Left
                ) {

                    for (auto& pianoKey : keys) {

                        pianoKey.pressed = false;

                        if (pianoKey.black)
                            pianoKey.shape.setFillColor(
                                sf::Color(25, 25, 25)
                            );
                        else
                            pianoKey.shape.setFillColor(
                                sf::Color(245, 245, 245)
                            );
                    }
                }
            }
        }


        // --------------------------------------------------
        // Remove finished sounds
        // --------------------------------------------------

        for (auto it = activeNotes.begin();
             it != activeNotes.end();) {

            if (
                it->sound->getStatus()
                == sf::Sound::Stopped
            ) {

                it =
                    activeNotes.erase(it);

            } else {

                ++it;
            }
        }


        // --------------------------------------------------
        // Draw
        // --------------------------------------------------

        window.clear(
            sf::Color(35, 35, 35)
        );


        // Title

        window.draw(title);

        window.draw(currentNote);


        // Piano keys

        // White keys first

        for (const auto& pianoKey : keys) {

            if (!pianoKey.black)
                window.draw(
                    pianoKey.shape
                );
        }


        // Black keys second

        for (const auto& pianoKey : keys) {

            if (pianoKey.black)
                window.draw(
                    pianoKey.shape
                );
        }


        // Pedals

        for (const auto& pedal : pedals) {

            window.draw(
                pedal.shape
            );

            window.draw(
                pedal.label
            );
        }


        window.display();
    }


    return 0;
}