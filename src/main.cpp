#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

// ------------------------------------------------------------
// Piano Key
// ------------------------------------------------------------

struct PianoKey
{
    sf::RectangleShape shape;
    std::string note;
    bool black;
    bool pressed;
};

// ------------------------------------------------------------
// Piano Pedal
// ------------------------------------------------------------

struct PianoPedal
{
    sf::RectangleShape shape;
    sf::Text label;
    bool pressed;
};

// ------------------------------------------------------------
// Convert note name -> MIDI number
// ------------------------------------------------------------

int noteToMidi(const std::string& note)
{
    if (note.size() < 2)
        return -1;

    std::string name = note.substr(0, note.size() - 1);

    int octave = note.back() - '0';

    int semitone = -1;

    if (name == "C")  semitone = 0;
    if (name == "C#") semitone = 1;
    if (name == "D")  semitone = 2;
    if (name == "D#") semitone = 3;
    if (name == "E")  semitone = 4;
    if (name == "F")  semitone = 5;
    if (name == "F#") semitone = 6;
    if (name == "G")  semitone = 7;
    if (name == "G#") semitone = 8;
    if (name == "A")  semitone = 9;
    if (name == "A#") semitone = 10;
    if (name == "B")  semitone = 11;

    if (semitone == -1)
        return -1;

    return (octave + 1) * 12 + semitone;
}

// ------------------------------------------------------------
// MIDI -> note name
// ------------------------------------------------------------

std::string midiToNote(int midi)
{
    static const std::vector<std::string> names =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    int octave = (midi / 12) - 1;
    int note = midi % 12;

    return names[note] + std::to_string(octave);
}

// ------------------------------------------------------------
// Find nearest available sample
// ------------------------------------------------------------

std::pair<std::string, float> findBestSample(
    const std::string& targetNote,
    const std::map<std::string, sf::SoundBuffer>& buffers)
{
    int targetMidi = noteToMidi(targetNote);

    if (targetMidi < 0)
        return {"", 1.0f};

    std::string bestSample;
    int bestDistance = 9999;
    int bestMidi = 0;

    for (const auto& pair : buffers)
    {
        int sampleMidi = noteToMidi(pair.first);

        if (sampleMidi < 0)
            continue;

        int distance = std::abs(targetMidi - sampleMidi);

        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestSample = pair.first;
            bestMidi = sampleMidi;
        }
    }

    if (bestSample.empty())
        return {"", 1.0f};

    float pitch =
        std::pow(
            2.0f,
            static_cast<float>(targetMidi - bestMidi) / 12.0f
        );

    return {bestSample, pitch};
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main()
{
    sf::RenderWindow window(
        sf::VideoMode(1500, 700),
        "Piano Simulator"
    );

    window.setFramerateLimit(60);

    // --------------------------------------------------------
    // Load samples
    // --------------------------------------------------------

    const std::vector<std::string> sampleNotes =
    {
        "A0","A1","A2","A3","A4","A5","A6","A7",

        "C1","C2","C3","C4","C5","C6","C7","C8",

        "D#1","D#2","D#3","D#4","D#5","D#6","D#7",

        "F#1","F#2","F#3","F#4","F#5","F#6","F#7"
    };

    std::map<std::string, sf::SoundBuffer> buffers;

    for (const auto& note : sampleNotes)
    {
        sf::SoundBuffer buffer;

        std::string path =
            "../sounds/" + note + ".wav";

        if (!buffer.loadFromFile(path))
        {
            std::cerr
                << "Failed to load "
                << path
                << std::endl;

            continue;
        }

        buffers.emplace(note, std::move(buffer));
    }

    std::cout
        << "Loaded samples: "
        << buffers.size()
        << std::endl;

    // --------------------------------------------------------
    // Active sounds
    // --------------------------------------------------------

    std::vector<std::unique_ptr<sf::Sound>> activeSounds;

    // --------------------------------------------------------
    // Play note
    // --------------------------------------------------------

    auto playNote =
        [&](const std::string& note)
    {
        auto result =
            findBestSample(note, buffers);

        const std::string& sample = result.first;
        float pitch = result.second;

        if (sample.empty())
        {
            std::cerr
                << "No sample available for "
                << note
                << std::endl;

            return;
        }

        auto it = buffers.find(sample);

        if (it == buffers.end())
            return;

        auto sound =
            std::make_unique<sf::Sound>();

        sound->setBuffer(it->second);
        sound->setPitch(pitch);
        sound->setVolume(100.f);
        sound->play();

        activeSounds.push_back(
            std::move(sound)
        );

        std::cout
            << note
            << " -> "
            << sample
            << " pitch="
            << pitch
            << std::endl;
    };

    // --------------------------------------------------------
    // Piano
    // --------------------------------------------------------

    std::vector<PianoKey> whiteKeys;
    std::vector<PianoKey> blackKeys;

    const float whiteWidth = 75.f;
    const float whiteHeight = 400.f;

    const float blackWidth = 45.f;
    const float blackHeight = 250.f;

    const float startX = 40.f;
    const float startY = 170.f;

    // --------------------------------------------------------
    // White keys
    // --------------------------------------------------------

    std::vector<std::string> whiteNotes =
    {
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

    for (size_t i = 0; i < whiteNotes.size(); ++i)
    {
        PianoKey key;

        key.note = whiteNotes[i];
        key.black = false;
        key.pressed = false;

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

        key.shape.setOutlineThickness(1.f);

        whiteKeys.push_back(key);
    }

    // --------------------------------------------------------
    // Black keys
    // --------------------------------------------------------

    struct BlackInfo
    {
        int position;
        std::string note;
    };

    std::vector<BlackInfo> blackInfo =
    {
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

    for (const auto& info : blackInfo)
    {
        PianoKey key;

        key.note = info.note;
        key.black = true;
        key.pressed = false;

        key.shape.setSize(
            sf::Vector2f(
                blackWidth,
                blackHeight
            )
        );

        key.shape.setPosition(
            startX
                + (info.position + 1)
                    * whiteWidth
                - blackWidth / 2.f,
            startY
        );

        key.shape.setFillColor(
            sf::Color(25, 25, 25)
        );

        key.shape.setOutlineColor(
            sf::Color::Black
        );

        key.shape.setOutlineThickness(2.f);

        blackKeys.push_back(key);
    }

    // --------------------------------------------------------
    // Font
    // --------------------------------------------------------

    sf::Font font;

    bool fontLoaded =
        font.loadFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        );

    // --------------------------------------------------------
    // Title
    // --------------------------------------------------------

    sf::Text title;

    if (fontLoaded)
    {
        title.setFont(font);
        title.setString("PIANO SIMULATOR");
        title.setCharacterSize(32);
        title.setFillColor(sf::Color::White);
        title.setPosition(40.f, 35.f);
    }

    // --------------------------------------------------------
    // Current note
    // --------------------------------------------------------

    sf::Text currentNote;

    if (fontLoaded)
    {
        currentNote.setFont(font);
        currentNote.setString("Press a key");
        currentNote.setCharacterSize(22);
        currentNote.setFillColor(sf::Color::White);
        currentNote.setPosition(40.f, 90.f);
    }

    // --------------------------------------------------------
    // Pedals
    // --------------------------------------------------------

    PianoPedal leftPedal;
    PianoPedal middlePedal;
    PianoPedal rightPedal;

    const float pedalWidth = 90.f;
    const float pedalHeight = 55.f;

    const float pedalY = 610.f;

    // Left pedal
    leftPedal.shape.setSize(
        sf::Vector2f(
            pedalWidth,
            pedalHeight
        )
    );

    leftPedal.shape.setPosition(
        610.f,
        pedalY
    );

    leftPedal.shape.setFillColor(
        sf::Color(80, 80, 90)
    );

    leftPedal.shape.setOutlineColor(
        sf::Color::White
    );

    leftPedal.shape.setOutlineThickness(2.f);

    leftPedal.pressed = false;

    // Middle pedal
    middlePedal.shape.setSize(
        sf::Vector2f(
            pedalWidth,
            pedalHeight
        )
    );

    middlePedal.shape.setPosition(
        705.f,
        pedalY
    );

    middlePedal.shape.setFillColor(
        sf::Color(80, 80, 90)
    );

    middlePedal.shape.setOutlineColor(
        sf::Color::White
    );

    middlePedal.shape.setOutlineThickness(2.f);

    middlePedal.pressed = false;

    // Right pedal
    rightPedal.shape.setSize(
        sf::Vector2f(
            pedalWidth,
            pedalHeight
        )
    );

    rightPedal.shape.setPosition(
        800.f,
        pedalY
    );

    rightPedal.shape.setFillColor(
        sf::Color(80, 80, 90)
    );

    rightPedal.shape.setOutlineColor(
        sf::Color::White
    );

    rightPedal.shape.setOutlineThickness(2.f);

    rightPedal.pressed = false;

    // --------------------------------------------------------
    // Pedal labels
    // --------------------------------------------------------

    if (fontLoaded)
    {
        leftPedal.label.setFont(font);
        leftPedal.label.setString("LEFT");
        leftPedal.label.setCharacterSize(16);
        leftPedal.label.setFillColor(sf::Color::White);

        middlePedal.label.setFont(font);
        middlePedal.label.setString("MIDDLE");
        middlePedal.label.setCharacterSize(16);
        middlePedal.label.setFillColor(sf::Color::White);

        rightPedal.label.setFont(font);
        rightPedal.label.setString("RIGHT");
        rightPedal.label.setCharacterSize(16);
        rightPedal.label.setFillColor(sf::Color::White);

        leftPedal.label.setPosition(625.f, 625.f);
        middlePedal.label.setPosition(713.f, 625.f);
        rightPedal.label.setPosition(815.f, 625.f);
    }

    // --------------------------------------------------------
    // Keyboard mapping
    // --------------------------------------------------------

    std::map<sf::Keyboard::Key, std::string> keyboardMap =
    {
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

    // --------------------------------------------------------
    // Main loop
    // --------------------------------------------------------

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // ------------------------------------------------
            // Keyboard pressed
            // ------------------------------------------------

            if (event.type == sf::Event::KeyPressed)
            {
                // --------------------------------------------
                // LEFT SHIFT -> LEFT PEDAL
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::LShift)
                {
                    leftPedal.pressed = true;

                    leftPedal.shape.setPosition(
                        610.f,
                        pedalY + 8.f
                    );

                    leftPedal.shape.setFillColor(
                        sf::Color(130, 130, 150)
                    );
                }

                // --------------------------------------------
                // SPACE -> MIDDLE PEDAL
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::Space)
                {
                    middlePedal.pressed = true;

                    middlePedal.shape.setPosition(
                        705.f,
                        pedalY + 8.f
                    );

                    middlePedal.shape.setFillColor(
                        sf::Color(130, 130, 150)
                    );
                }

                // --------------------------------------------
                // RIGHT SHIFT -> RIGHT PEDAL
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::RShift)
                {
                    rightPedal.pressed = true;

                    rightPedal.shape.setPosition(
                        800.f,
                        pedalY + 8.f
                    );

                    rightPedal.shape.setFillColor(
                        sf::Color(130, 130, 150)
                    );
                }

                // --------------------------------------------
                // Piano keys
                // --------------------------------------------

                auto it =
                    keyboardMap.find(
                        event.key.code
                    );

                if (it != keyboardMap.end())
                {
                    playNote(it->second);

                    if (fontLoaded)
                    {
                        currentNote.setString(
                            "Playing: " + it->second
                        );
                    }

                    for (auto& key : whiteKeys)
                    {
                        if (key.note == it->second)
                        {
                            key.pressed = true;

                            key.shape.setFillColor(
                                sf::Color(180, 220, 255)
                            );
                        }
                    }

                    for (auto& key : blackKeys)
                    {
                        if (key.note == it->second)
                        {
                            key.pressed = true;

                            key.shape.setFillColor(
                                sf::Color(80, 120, 160)
                            );
                        }
                    }
                }
            }

            // ------------------------------------------------
            // Keyboard released
            // ------------------------------------------------

            if (event.type == sf::Event::KeyReleased)
            {
                // --------------------------------------------
                // LEFT SHIFT
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::LShift)
                {
                    leftPedal.pressed = false;

                    leftPedal.shape.setPosition(
                        610.f,
                        pedalY
                    );

                    leftPedal.shape.setFillColor(
                        sf::Color(80, 80, 90)
                    );
                }

                // --------------------------------------------
                // SPACE
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::Space)
                {
                    middlePedal.pressed = false;

                    middlePedal.shape.setPosition(
                        705.f,
                        pedalY
                    );

                    middlePedal.shape.setFillColor(
                        sf::Color(80, 80, 90)
                    );
                }

                // --------------------------------------------
                // RIGHT SHIFT
                // --------------------------------------------

                if (event.key.code == sf::Keyboard::RShift)
                {
                    rightPedal.pressed = false;

                    rightPedal.shape.setPosition(
                        800.f,
                        pedalY
                    );

                    rightPedal.shape.setFillColor(
                        sf::Color(80, 80, 90)
                    );
                }

                // --------------------------------------------
                // Piano keys
                // --------------------------------------------

                auto it =
                    keyboardMap.find(
                        event.key.code
                    );

                if (it != keyboardMap.end())
                {
                    for (auto& key : whiteKeys)
                    {
                        if (key.note == it->second)
                        {
                            key.pressed = false;

                            key.shape.setFillColor(
                                sf::Color(245, 245, 245)
                            );
                        }
                    }

                    for (auto& key : blackKeys)
                    {
                        if (key.note == it->second)
                        {
                            key.pressed = false;

                            key.shape.setFillColor(
                                sf::Color(25, 25, 25)
                            );
                        }
                    }
                }
            }

            // ------------------------------------------------
            // Mouse
            // ------------------------------------------------

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button ==
                    sf::Mouse::Left)
                {
                    sf::Vector2f mouse =
                        window.mapPixelToCoords(
                            sf::Vector2i(
                                event.mouseButton.x,
                                event.mouseButton.y
                            )
                        );

                    bool found = false;

                    // Black keys first
                    for (auto& key : blackKeys)
                    {
                        if (key.shape.getGlobalBounds()
                                .contains(mouse))
                        {
                            playNote(key.note);

                            if (fontLoaded)
                            {
                                currentNote.setString(
                                    "Playing: "
                                    + key.note
                                );
                            }

                            key.pressed = true;

                            key.shape.setFillColor(
                                sf::Color(80, 120, 160)
                            );

                            found = true;

                            break;
                        }
                    }

                    // White keys
                    if (!found)
                    {
                        for (auto& key : whiteKeys)
                        {
                            if (key.shape.getGlobalBounds()
                                    .contains(mouse))
                            {
                                playNote(key.note);

                                if (fontLoaded)
                                {
                                    currentNote.setString(
                                        "Playing: "
                                        + key.note
                                    );
                                }

                                key.pressed = true;

                                key.shape.setFillColor(
                                    sf::Color(180, 220, 255)
                                );

                                break;
                            }
                        }
                    }
                }
            }

            // ------------------------------------------------
            // Mouse release
            // ------------------------------------------------

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button ==
                    sf::Mouse::Left)
                {
                    for (auto& key : whiteKeys)
                    {
                        key.pressed = false;

                        key.shape.setFillColor(
                            sf::Color(245, 245, 245)
                        );
                    }

                    for (auto& key : blackKeys)
                    {
                        key.pressed = false;

                        key.shape.setFillColor(
                            sf::Color(25, 25, 25)
                        );
                    }
                }
            }
        }

        // ----------------------------------------------------
        // Draw
        // ----------------------------------------------------

        window.clear(
            sf::Color(30, 35, 45)
        );

        if (fontLoaded)
        {
            window.draw(title);
            window.draw(currentNote);
        }

        // White keys
        for (auto& key : whiteKeys)
        {
            window.draw(key.shape);
        }

        // Black keys
        for (auto& key : blackKeys)
        {
            window.draw(key.shape);
        }

        // Pedals
        window.draw(leftPedal.shape);
        window.draw(middlePedal.shape);
        window.draw(rightPedal.shape);

        if (fontLoaded)
        {
            window.draw(leftPedal.label);
            window.draw(middlePedal.label);
            window.draw(rightPedal.label);
        }

        window.display();
    }

    return 0;
}

