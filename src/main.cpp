#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <chrono>

struct PianoKey {
    sf::RectangleShape shape;
    sf::Text label;

    std::string note;
    std::string keyboardKey;

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


// ======================================================
// RECORDING
// ======================================================

enum class RecordingEventType {
    NoteOn,
    NoteOff,
    SustainOn,
    SustainOff,
    SoftOn,
    SoftOff,
    SostenutoOn,
    SostenutoOff
};

struct RecordingEvent {
    RecordingEventType type;
    std::string note;
    float time;
};


// ======================================================
// NOTE -> MIDI
// ======================================================

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


// ======================================================
// FIND BEST SAMPLE
// ======================================================

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
            std::abs(
                targetMidi - sampleMidi
            );

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
                targetMidi -
                noteToMidi(bestNote)
            ) / 12.0f
        );

    return {
        bestBuffer,
        pitch
    };
}


// ======================================================
// PLAY NOTE
// ======================================================

void playNote(
    const std::string& note,
    sf::Keyboard::Key key,
    const std::map<std::string, sf::SoundBuffer>& buffers,
    std::vector<PlayingNote>& activeNotes,
    bool softPedal
) {
    auto result =
        findBestSample(
            note,
            buffers
        );

    if (!result.first)
        return;

    auto sound =
        std::make_unique<sf::Sound>();

    sound->setBuffer(
        *result.first
    );

    sound->setPitch(
        result.second
    );

    sound->setVolume(
        softPedal
        ? 55.f
        : 100.f
    );

    sound->play();

    std::cout
        << note
        << " -> pitch="
        << result.second;

    if (softPedal)
        std::cout
            << " | SOFT";

    std::cout
        << std::endl;

    PlayingNote playing;

    playing.sound =
        std::move(sound);

    playing.note =
        note;

    playing.key =
        key;

    playing.keyHeld =
        true;

    playing.sostenutoCaptured =
        false;

    activeNotes.push_back(
        std::move(playing)
    );
}


// ======================================================
// MAIN
// ======================================================

int main() {

    // ==================================================
    // WINDOW
    // ==================================================

    sf::RenderWindow window(
        sf::VideoMode(1500, 700),
        "Piano Simulator"
    );

    window.setFramerateLimit(60);


    // ==================================================
    // FONT
    // ==================================================

    sf::Font font;

    if (!font.loadFromFile(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    )) {

        std::cerr
            << "Failed to load font."
            << std::endl;

        return 1;
    }


    // ==================================================
    // COLORS
    // ==================================================

    sf::Color background(
        18, 18, 18
    );

    sf::Color bodyColor(
        55, 32, 20
    );

    sf::Color bodyDark(
        30, 18, 12
    );

    sf::Color gold(
        190, 150, 70
    );

    sf::Color whiteKey(
        245, 243, 235
    );

    sf::Color whitePressed(
        100, 170, 220
    );

    sf::Color blackKey(
        20, 20, 22
    );

    sf::Color blackPressed(
        80, 150, 210
    );

    sf::Color trainingColor(
        255, 200, 70
    );


    // ==================================================
    // LOAD SAMPLES
    // ==================================================

    std::map<
        std::string,
        sf::SoundBuffer
    > buffers;

    std::vector<std::string>
        sampleNotes = {

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


    for (const auto& note :
         sampleNotes) {

        sf::SoundBuffer buffer;

        std::string path =
            "../sounds/" +
            note +
            ".wav";

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


    // ==================================================
    // PIANO BODY
    // ==================================================

    sf::RectangleShape pianoBody;

    pianoBody.setPosition(
        20.f,
        130.f
    );

    pianoBody.setSize(
        sf::Vector2f(
            1460.f,
            475.f
        )
    );

    pianoBody.setFillColor(
        bodyColor
    );

    pianoBody.setOutlineColor(
        gold
    );

    pianoBody.setOutlineThickness(
        3.f
    );


    // ==================================================
    // TOP PANEL
    // ==================================================

    sf::RectangleShape topPanel;

    topPanel.setPosition(
        20.f,
        130.f
    );

    topPanel.setSize(
        sf::Vector2f(
            1460.f,
            65.f
        )
    );

    topPanel.setFillColor(
        bodyDark
    );

    topPanel.setOutlineColor(
        gold
    );

    topPanel.setOutlineThickness(
        2.f
    );


    // ==================================================
    // TITLE
    // ==================================================

    sf::Text title;

    title.setFont(font);

    title.setString(
        "PIANO SIMULATOR"
    );

    title.setCharacterSize(
        30
    );

    title.setStyle(
        sf::Text::Bold
    );

    title.setFillColor(
        sf::Color::White
    );

    title.setPosition(
        40.f,
        35.f
    );


    // ==================================================
    // CURRENT NOTE
    // ==================================================

    sf::Text currentNote;

    currentNote.setFont(font);

    currentNote.setString(
        "Ready"
    );

    currentNote.setCharacterSize(
        20
    );

    currentNote.setFillColor(
        sf::Color(
            210,
            210,
            210
        )
    );

    currentNote.setPosition(
        40.f,
        90.f
    );


    // ==================================================
    // RECORDING STATUS
    // ==================================================

    sf::Text recordingStatus;

    recordingStatus.setFont(font);

    recordingStatus.setString(
        "F8: Training   F9: Record   F10: Play   F11: Stop"
    );

    recordingStatus.setCharacterSize(
        15
    );

    recordingStatus.setFillColor(
        sf::Color(
            180,
            180,
            180
        )
    );

    recordingStatus.setPosition(
        910.f,
        48.f
    );


    // ==================================================
    // BRAND
    // ==================================================

    sf::Text brand;

    brand.setFont(font);

    brand.setString(
        "MOJTABAFAR"
    );

    brand.setCharacterSize(
        22
    );

    brand.setStyle(
        sf::Text::Bold
    );

    brand.setFillColor(
        gold
    );

    brand.setPosition(
        660.f,
        150.f
    );


    // ==================================================
    // TRAINING MODE
    // ==================================================

    bool trainingMode = false;

    size_t trainingIndex = 0;

    bool trainingWrong = false;

    sf::Clock trainingFeedbackClock;

    std::vector<std::string>
        trainingSong = {

        "C4",
        "C4",
        "G4",
        "G4",
        "A4",
        "A4",
        "G4",

        "F4",
        "F4",
        "E4",
        "E4",
        "D4",
        "D4",
        "C4"
    };


    sf::Text trainingText;

    trainingText.setFont(font);

    trainingText.setString(
        "TRAINING: OFF"
    );

    trainingText.setCharacterSize(
        18
    );

    trainingText.setStyle(
        sf::Text::Bold
    );

    trainingText.setFillColor(
        sf::Color(
            180,
            180,
            180
        )
    );

    trainingText.setPosition(
        520.f,
        90.f
    );


    // ==================================================
    // TRAINING TARGET FRAME
    // ==================================================

    sf::RectangleShape trainingTarget;

    trainingTarget.setFillColor(
        sf::Color(
            255,
            200,
            70,
            35
        )
    );

    trainingTarget.setOutlineColor(
        trainingColor
    );

    trainingTarget.setOutlineThickness(
        4.f
    );


    // ==================================================
    // PIANO KEYS
    // ==================================================

    std::vector<PianoKey> keys;

    float startX = 40.f;
    float startY = 195.f;

    float whiteWidth = 95.f;
    float whiteHeight = 350.f;


    // ==================================================
    // WHITE NOTES
    // ==================================================

    std::vector<std::string>
        whiteNotes = {

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


    // ==================================================
    // WHITE KEY MAPPING
    // ==================================================

    std::vector<std::string>
        whiteKeyboardKeys = {

        "A",
        "S",
        "D",
        "F",
        "G",
        "H",
        "J",

        "K",
        "L",
        "Z",
        "X",
        "C",
        "V",
        "B"
    };


    // ==================================================
    // CREATE WHITE KEYS
    // ==================================================

    for (size_t i = 0;
         i < whiteNotes.size();
         ++i) {

        PianoKey key;

        key.shape.setSize(
            sf::Vector2f(
                whiteWidth - 3.f,
                whiteHeight
            )
        );

        key.shape.setPosition(
            startX +
            i * whiteWidth,
            startY
        );

        key.shape.setFillColor(
            whiteKey
        );

        key.shape.setOutlineColor(
            sf::Color(
                50, 50, 50
            )
        );

        key.shape.setOutlineThickness(
            2.f
        );

        key.note =
            whiteNotes[i];

        key.keyboardKey =
            whiteKeyboardKeys[i];

        key.black =
            false;

        key.pressed =
            false;


        key.label.setFont(
            font
        );

        key.label.setString(
            whiteKeyboardKeys[i] +
            "\n" +
            whiteNotes[i]
        );

        key.label.setCharacterSize(
            14
        );

        key.label.setStyle(
            sf::Text::Bold
        );

        key.label.setFillColor(
            sf::Color(
                70, 70, 70
            )
        );

        sf::FloatRect bounds =
            key.label.getLocalBounds();

        key.label.setOrigin(
            bounds.left +
            bounds.width / 2.f,
            bounds.top +
            bounds.height / 2.f
        );

        key.label.setPosition(
            startX +
            i * whiteWidth +
            whiteWidth / 2.f,
            startY +
            whiteHeight -
            45.f
        );

        keys.push_back(
            key
        );
    }


    // ==================================================
    // BLACK KEYS
    // ==================================================

    std::vector<
        std::tuple<
            int,
            std::string,
            std::string,
            sf::Keyboard::Key
        >
    > blackKeyData = {

        {
            0,
            "C#4",
            "W",
            sf::Keyboard::W
        },

        {
            1,
            "D#4",
            "E",
            sf::Keyboard::E
        },

        {
            3,
            "F#4",
            "T",
            sf::Keyboard::T
        },

        {
            4,
            "G#4",
            "Y",
            sf::Keyboard::Y
        },

        {
            5,
            "A#4",
            "U",
            sf::Keyboard::U
        },

        {
            7,
            "C#5",
            "O",
            sf::Keyboard::O
        },

        {
            8,
            "D#5",
            "P",
            sf::Keyboard::P
        },

        {
            10,
            "F#5",
            "2",
            sf::Keyboard::Num2
        },

        {
            11,
            "G#5",
            "3",
            sf::Keyboard::Num3
        },

        {
            12,
            "A#5",
            "4",
            sf::Keyboard::Num4
        }
    };


    // ==================================================
    // CREATE BLACK KEYS
    // ==================================================

    for (const auto& data :
         blackKeyData) {

        int index =
            std::get<0>(data);

        std::string note =
            std::get<1>(data);

        std::string keyboardLabel =
            std::get<2>(data);

        PianoKey key;

        float blackWidth = 58.f;
        float blackHeight = 215.f;

        key.shape.setSize(
            sf::Vector2f(
                blackWidth,
                blackHeight
            )
        );

        key.shape.setPosition(
            startX +
            (index + 1) *
            whiteWidth -
            blackWidth / 2.f,
            startY
        );

        key.shape.setFillColor(
            blackKey
        );

        key.shape.setOutlineColor(
            sf::Color::Black
        );

        key.shape.setOutlineThickness(
            2.f
        );

        key.note =
            note;

        key.keyboardKey =
            keyboardLabel;

        key.black =
            true;

        key.pressed =
            false;


        key.label.setFont(
            font
        );

        key.label.setString(
            keyboardLabel +
            "\n" +
            note
        );

        key.label.setCharacterSize(
            11
        );

        key.label.setStyle(
            sf::Text::Bold
        );

        key.label.setFillColor(
            sf::Color(
                220,
                220,
                220
            )
        );

        sf::FloatRect bounds =
            key.label.getLocalBounds();

        key.label.setOrigin(
            bounds.left +
            bounds.width / 2.f,
            bounds.top +
            bounds.height / 2.f
        );

        key.label.setPosition(
            startX +
            (index + 1) *
            whiteWidth,
            startY +
            blackHeight -
            30.f
        );

        keys.push_back(
            key
        );
    }


    // ==================================================
    // COMPLETE KEYBOARD MAP
    // ==================================================

    std::map<
        sf::Keyboard::Key,
        std::string
    > keyboardMap = {

        {sf::Keyboard::A, "C4"},
        {sf::Keyboard::W, "C#4"},
        {sf::Keyboard::S, "D4"},
        {sf::Keyboard::E, "D#4"},
        {sf::Keyboard::D, "E4"},
        {sf::Keyboard::F, "F4"},
        {sf::Keyboard::T, "F#4"},
        {sf::Keyboard::G, "G4"},
        {sf::Keyboard::Y, "G#4"},
        {sf::Keyboard::H, "A4"},
        {sf::Keyboard::U, "A#4"},
        {sf::Keyboard::J, "B4"},

        {sf::Keyboard::K, "C5"},
        {sf::Keyboard::O, "C#5"},
        {sf::Keyboard::L, "D5"},
        {sf::Keyboard::P, "D#5"},
        {sf::Keyboard::Z, "E5"},
        {sf::Keyboard::X, "F5"},
        {sf::Keyboard::Num2, "F#5"},
        {sf::Keyboard::C, "G5"},
        {sf::Keyboard::Num3, "G#5"},
        {sf::Keyboard::V, "A5"},
        {sf::Keyboard::Num4, "A#5"},
        {sf::Keyboard::B, "B5"}
    };


    // ==================================================
    // PEDALS
    // ==================================================

    std::vector<PianoPedal>
        pedals;

    std::vector<std::string>
        pedalNames = {

        "SOFT",
        "SOSTENUTO",
        "SUSTAIN"
    };


    float pedalX = 570.f;
    float pedalY = 585.f;


    for (int i = 0; i < 3; ++i) {

        PianoPedal pedal;

        float width =
            (i == 1)
            ? 130.f
            : 100.f;

        float x =
            (i == 0)
            ? pedalX
            : (
                i == 1
                ? pedalX + 110.f
                : pedalX + 250.f
            );

        pedal.shape.setSize(
            sf::Vector2f(
                width,
                45.f
            )
        );

        pedal.shape.setPosition(
            x,
            pedalY
        );

        pedal.shape.setFillColor(
            bodyDark
        );

        pedal.shape.setOutlineColor(
            gold
        );

        pedal.shape.setOutlineThickness(
            2.f
        );

        pedal.label.setFont(
            font
        );

        pedal.label.setString(
            pedalNames[i]
        );

        pedal.label.setCharacterSize(
            13
        );

        pedal.label.setFillColor(
            gold
        );

        sf::FloatRect bounds =
            pedal.label.getLocalBounds();

        pedal.label.setOrigin(
            bounds.left +
            bounds.width / 2.f,
            bounds.top +
            bounds.height / 2.f
        );

        pedal.label.setPosition(
            x + width / 2.f,
            pedalY + 22.f
        );

        pedal.pressed =
            false;

        pedals.push_back(
            pedal
        );
    }


    // ==================================================
    // PEDAL STATES
    // ==================================================

    bool sustainPedal = false;
    bool softPedal = false;
    bool sostenutoPedal = false;


    // ==================================================
    // ACTIVE NOTES
    // ==================================================

    std::vector<PlayingNote>
        activeNotes;


    // ==================================================
    // RECORDING STATE
    // ==================================================

    bool recording = false;
    bool playingRecording = false;

    std::vector<RecordingEvent>
        recordingEvents;

    sf::Clock recordingClock;
    sf::Clock playbackClock;

    size_t playbackIndex = 0;

    bool playbackSustain = false;
    bool playbackSoft = false;
    bool playbackSostenuto = false;


    // ==================================================
    // HELPER - RECORD EVENT
    // ==================================================

    auto recordEvent =
        [&](RecordingEventType type,
            const std::string& note = "") {

            if (!recording)
                return;

            RecordingEvent event;

            event.type = type;
            event.note = note;
            event.time =
                recordingClock.getElapsedTime()
                    .asSeconds();

            recordingEvents.push_back(
                event
            );
        };


    // ==================================================
    // HELPER - TRAINING NOTE
    // ==================================================

    auto handleTrainingNote =
        [&](const std::string& note) {

            if (!trainingMode)
                return;

            if (trainingIndex >= trainingSong.size())
                return;

            const std::string& target =
                trainingSong[trainingIndex];

            if (note == target) {

                trainingWrong = false;

                trainingIndex++;

                if (trainingIndex >= trainingSong.size()) {

                    trainingText.setString(
                        "TRAINING: COMPLETE!"
                    );

                    trainingText.setFillColor(
                        sf::Color(
                            90,
                            220,
                            120
                        )
                    );

                    currentNote.setString(
                        "Lesson Complete!"
                    );

                } else {

                    trainingText.setString(
                        "NEXT: " +
                        trainingSong[trainingIndex]
                    );

                    trainingText.setFillColor(
                        trainingColor
                    );

                    currentNote.setString(
                        "Correct!  Next: " +
                        trainingSong[trainingIndex]
                    );
                }

            } else {

                trainingWrong = true;

                trainingFeedbackClock.restart();

                trainingText.setString(
                    "NEXT: " +
                    target
                );

                trainingText.setFillColor(
                    sf::Color(
                        255,
                        100,
                        100
                    )
                );

                currentNote.setString(
                    "Wrong! Play: " +
                    target
                );
            }
        };


    // ==================================================
    // MAIN LOOP
    // ==================================================

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {


            // ==================================================
            // CLOSE
            // ==================================================

            if (event.type ==
                sf::Event::Closed) {

                window.close();
            }


            // ==================================================
            // KEY PRESSED
            // ==================================================

            if (event.type ==
                sf::Event::KeyPressed) {

                auto key =
                    event.key.code;


                // ==================================================
                // F8 - TRAINING
                // ==================================================

                if (key == sf::Keyboard::F8) {

                    trainingMode =
                        !trainingMode;

                    trainingIndex = 0;

                    trainingWrong = false;

                    if (trainingMode) {

                        trainingText.setString(
                            "NEXT: " +
                            trainingSong[0]
                        );

                        trainingText.setFillColor(
                            trainingColor
                        );

                        currentNote.setString(
                            "Training Mode Started"
                        );

                    } else {

                        trainingText.setString(
                            "TRAINING: OFF"
                        );

                        trainingText.setFillColor(
                            sf::Color(
                                180,
                                180,
                                180
                            )
                        );

                        currentNote.setString(
                            "Ready"
                        );
                    }

                    continue;
                }


                // ==================================================
                // F9 - RECORD
                // ==================================================

                if (key == sf::Keyboard::F9) {

                    if (!recording) {

                        recording =
                            true;

                        playingRecording =
                            false;

                        recordingEvents.clear();

                        recordingClock.restart();

                        currentNote.setString(
                            "RECORDING..."
                        );

                        recordingStatus.setString(
                            "● RECORDING"
                        );

                        recordingStatus.setFillColor(
                            sf::Color(
                                220,
                                70,
                                70
                            )
                        );

                        std::cout
                            << "Recording started."
                            << std::endl;

                    } else {

                        recording =
                            false;

                        currentNote.setString(
                            "Recording stopped"
                        );

                        recordingStatus.setString(
                            "Recording saved: " +
                            std::to_string(
                                recordingEvents.size()
                            ) +
                            " events"
                        );

                        recordingStatus.setFillColor(
                            sf::Color(
                                180,
                                180,
                                180
                            )
                        );

                        std::cout
                            << "Recording stopped. Events: "
                            << recordingEvents.size()
                            << std::endl;
                    }

                    continue;
                }


                // ==================================================
                // F10 - PLAY RECORDING
                // ==================================================

                if (key == sf::Keyboard::F10) {

                    if (
                        !recordingEvents.empty() &&
                        !recording
                    ) {

                        playingRecording =
                            true;

                        playbackIndex =
                            0;

                        playbackClock.restart();

                        playbackSustain = false;
                        playbackSoft = false;
                        playbackSostenuto = false;

                        currentNote.setString(
                            "PLAYING RECORDING..."
                        );

                        recordingStatus.setString(
                            "▶ PLAYBACK"
                        );

                        recordingStatus.setFillColor(
                            sf::Color(
                                90,
                                180,
                                240
                            )
                        );

                        std::cout
                            << "Playback started."
                            << std::endl;

                    } else if (recordingEvents.empty()) {

                        currentNote.setString(
                            "No recording"
                        );
                    }

                    continue;
                }


                // ==================================================
                // F11 - STOP PLAYBACK
                // ==================================================

                if (key == sf::Keyboard::F11) {

                    if (playingRecording) {

                        playingRecording =
                            false;

                        currentNote.setString(
                            "Playback stopped"
                        );

                        recordingStatus.setString(
                            "F8: Training   F9: Record   F10: Play   F11: Stop"
                        );

                        recordingStatus.setFillColor(
                            sf::Color(
                                180,
                                180,
                                180
                            )
                        );
                    }

                    continue;
                }


                // ==================================================
                // RIGHT SHIFT - SUSTAIN
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::RShift
                ) {

                    if (!sustainPedal) {

                        sustainPedal =
                            true;

                        pedals[2].pressed =
                            true;

                        pedals[2].shape.move(
                            0.f,
                            6.f
                        );

                        recordEvent(
                            RecordingEventType::SustainOn
                        );
                    }

                    continue;
                }


                // ==================================================
                // LEFT SHIFT - SOFT
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::LShift
                ) {

                    if (!softPedal) {

                        softPedal =
                            true;

                        pedals[0].pressed =
                            true;

                        pedals[0].shape.move(
                            0.f,
                            6.f
                        );

                        recordEvent(
                            RecordingEventType::SoftOn
                        );
                    }

                    continue;
                }


                // ==================================================
                // SPACE - SOSTENUTO
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::Space
                ) {

                    if (!sostenutoPedal) {

                        sostenutoPedal =
                            true;

                        pedals[1].pressed =
                            true;

                        pedals[1].shape.move(
                            0.f,
                            6.f
                        );

                        for (
                            auto& playing :
                            activeNotes
                        ) {

                            if (
                                playing.sound &&
                                playing.sound
                                    ->getStatus()
                                ==
                                sf::Sound::Playing
                            ) {

                                playing
                                    .sostenutoCaptured
                                    =
                                    true;
                            }
                        }

                        recordEvent(
                            RecordingEventType::SostenutoOn
                        );
                    }

                    continue;
                }


                // ==================================================
                // PIANO KEY
                // ==================================================

                auto mapIt =
                    keyboardMap.find(
                        key
                    );

                if (
                    mapIt !=
                    keyboardMap.end()
                ) {

                    bool alreadyHeld =
                        false;


                    for (
                        const auto& playing :
                        activeNotes
                    ) {

                        if (
                            playing.key ==
                                key &&
                            playing.keyHeld
                        ) {

                            alreadyHeld =
                                true;

                            break;
                        }
                    }


                    if (alreadyHeld)
                        continue;


                    for (
                        auto& pianoKey :
                        keys
                    ) {

                        if (
                            pianoKey.note ==
                            mapIt->second
                        ) {

                            pianoKey.pressed =
                                true;


                            if (
                                pianoKey.black
                            ) {

                                pianoKey.shape
                                    .setFillColor(
                                        blackPressed
                                    );

                                pianoKey.label
                                    .setFillColor(
                                        sf::Color::White
                                    );

                            } else {

                                pianoKey.shape
                                    .setFillColor(
                                        whitePressed
                                    );

                                pianoKey.label
                                    .setFillColor(
                                        sf::Color::White
                                    );
                            }
                        }
                    }


                    playNote(
                        mapIt->second,
                        key,
                        buffers,
                        activeNotes,
                        softPedal
                    );


                    recordEvent(
                        RecordingEventType::NoteOn,
                        mapIt->second
                    );


                    handleTrainingNote(
                        mapIt->second
                    );


                    currentNote.setString(
                        "Playing: " +
                        mapIt->second
                    );
                }
            }


            // ==================================================
            // KEY RELEASED
            // ==================================================

            if (event.type ==
                sf::Event::KeyReleased) {

                auto key =
                    event.key.code;


                // ==================================================
                // RIGHT SHIFT
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::RShift
                ) {

                    sustainPedal =
                        false;

                    pedals[2].pressed =
                        false;

                    pedals[2].shape.move(
                        0.f,
                        -6.f
                    );

                    recordEvent(
                        RecordingEventType::SustainOff
                    );


                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            !it->keyHeld &&
                            !it->sostenutoCaptured
                        ) {

                            it->sound->stop();

                            it =
                                activeNotes.erase(
                                    it
                                );

                        } else {

                            ++it;
                        }
                    }

                    continue;
                }


                // ==================================================
                // LEFT SHIFT
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::LShift
                ) {

                    softPedal =
                        false;

                    pedals[0].pressed =
                        false;

                    pedals[0].shape.move(
                        0.f,
                        -6.f
                    );

                    recordEvent(
                        RecordingEventType::SoftOff
                    );

                    continue;
                }


                // ==================================================
                // SPACE
                // ==================================================

                if (
                    key ==
                    sf::Keyboard::Space
                ) {

                    sostenutoPedal =
                        false;

                    pedals[1].pressed =
                        false;

                    pedals[1].shape.move(
                        0.f,
                        -6.f
                    );

                    recordEvent(
                        RecordingEventType::SostenutoOff
                    );


                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            it->sostenutoCaptured
                        ) {

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

                    continue;
                }


                // ==================================================
                // NORMAL PIANO KEY RELEASE
                // ==================================================

                auto mapIt =
                    keyboardMap.find(
                        key
                    );

                if (
                    mapIt !=
                    keyboardMap.end()
                ) {

                    for (
                        auto& pianoKey :
                        keys
                    ) {

                        if (
                            pianoKey.note ==
                            mapIt->second
                        ) {

                            pianoKey.pressed =
                                false;


                            if (
                                pianoKey.black
                            ) {

                                pianoKey.shape
                                    .setFillColor(
                                        blackKey
                                    );

                                pianoKey.label
                                    .setFillColor(
                                        sf::Color(
                                            220,
                                            220,
                                            220
                                        )
                                    );

                            } else {

                                pianoKey.shape
                                    .setFillColor(
                                        whiteKey
                                    );

                                pianoKey.label
                                    .setFillColor(
                                        sf::Color(
                                            70,
                                            70,
                                            70
                                        )
                                    );
                            }
                        }
                    }


                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            it->key == key &&
                            it->keyHeld
                        ) {

                            it->keyHeld =
                                false;


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


                    recordEvent(
                        RecordingEventType::NoteOff,
                        mapIt->second
                    );
                }
            }


            // ==================================================
            // MOUSE PRESSED
            // ==================================================

            if (
                event.type ==
                    sf::Event::MouseButtonPressed &&
                event.mouseButton.button ==
                    sf::Mouse::Left
            ) {

                sf::Vector2f mousePos(
                    event.mouseButton.x,
                    event.mouseButton.y
                );

                bool clicked =
                    false;


                // Black keys first

                for (
                    auto& pianoKey :
                    keys
                ) {

                    if (
                        pianoKey.black &&
                        pianoKey.shape
                            .getGlobalBounds()
                            .contains(mousePos)
                    ) {

                        pianoKey.pressed =
                            true;

                        pianoKey.shape
                            .setFillColor(
                                blackPressed
                            );

                        pianoKey.label
                            .setFillColor(
                                sf::Color::White
                            );


                        playNote(
                            pianoKey.note,
                            sf::Keyboard::Unknown,
                            buffers,
                            activeNotes,
                            softPedal
                        );


                        recordEvent(
                            RecordingEventType::NoteOn,
                            pianoKey.note
                        );


                        handleTrainingNote(
                            pianoKey.note
                        );


                        currentNote.setString(
                            "Playing: " +
                            pianoKey.note
                        );

                        clicked =
                            true;

                        break;
                    }
                }


                // White keys

                if (!clicked) {

                    for (
                        auto& pianoKey :
                        keys
                    ) {

                        if (
                            !pianoKey.black &&
                            pianoKey.shape
                                .getGlobalBounds()
                                .contains(
                                    mousePos
                                )
                        ) {

                            pianoKey.pressed =
                                true;

                            pianoKey.shape
                                .setFillColor(
                                    whitePressed
                                );

                            pianoKey.label
                                .setFillColor(
                                    sf::Color::White
                                );


                            playNote(
                                pianoKey.note,
                                sf::Keyboard::Unknown,
                                buffers,
                                activeNotes,
                                softPedal
                            );


                            recordEvent(
                                RecordingEventType::NoteOn,
                                pianoKey.note
                            );


                            handleTrainingNote(
                                pianoKey.note
                            );


                            currentNote.setString(
                                "Playing: " +
                                pianoKey.note
                            );

                            break;
                        }
                    }
                }
            }


            // ==================================================
            // MOUSE RELEASED
            // ==================================================

            if (
                event.type ==
                    sf::Event::MouseButtonReleased &&
                event.mouseButton.button ==
                    sf::Mouse::Left
            ) {

                for (
                    auto& pianoKey :
                    keys
                ) {

                    pianoKey.pressed =
                        false;


                    if (
                        pianoKey.black
                    ) {

                        pianoKey.shape
                            .setFillColor(
                                blackKey
                            );

                        pianoKey.label
                            .setFillColor(
                                sf::Color(
                                    220,
                                    220,
                                    220
                                )
                            );

                    } else {

                        pianoKey.shape
                            .setFillColor(
                                whiteKey
                            );

                        pianoKey.label
                            .setFillColor(
                                sf::Color(
                                    70,
                                    70,
                                    70
                                )
                            );
                    }
                }
            }
        }


        // ==================================================
        // PLAY RECORDED EVENTS
        // ==================================================

        if (
            playingRecording &&
            playbackIndex <
                recordingEvents.size()
        ) {

            float currentTime =
                playbackClock
                    .getElapsedTime()
                    .asSeconds();


            while (
                playbackIndex <
                recordingEvents.size() &&
                recordingEvents[
                    playbackIndex
                ].time <= currentTime
            ) {

                const auto& recorded =
                    recordingEvents[
                        playbackIndex
                    ];


                // NOTE ON

                if (
                    recorded.type ==
                    RecordingEventType::NoteOn
                ) {

                    playNote(
                        recorded.note,
                        sf::Keyboard::Unknown,
                        buffers,
                        activeNotes,
                        playbackSoft
                    );

                    currentNote.setString(
                        "Playback: " +
                        recorded.note
                    );
                }


                // NOTE OFF

                else if (
                    recorded.type ==
                    RecordingEventType::NoteOff
                ) {

                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            it->note ==
                            recorded.note
                        ) {

                            it->keyHeld =
                                false;

                            if (
                                !playbackSustain &&
                                !playbackSostenuto
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


                // SUSTAIN ON

                else if (
                    recorded.type ==
                    RecordingEventType::SustainOn
                ) {

                    playbackSustain =
                        true;
                }


                // SUSTAIN OFF

                else if (
                    recorded.type ==
                    RecordingEventType::SustainOff
                ) {

                    playbackSustain =
                        false;

                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            !it->keyHeld &&
                            !playbackSostenuto
                        ) {

                            it->sound->stop();

                            it =
                                activeNotes.erase(
                                    it
                                );

                            continue;
                        }

                        ++it;
                    }
                }


                // SOFT ON

                else if (
                    recorded.type ==
                    RecordingEventType::SoftOn
                ) {

                    playbackSoft =
                        true;
                }


                // SOFT OFF

                else if (
                    recorded.type ==
                    RecordingEventType::SoftOff
                ) {

                    playbackSoft =
                        false;
                }


                // SOSTENUTO ON

                else if (
                    recorded.type ==
                    RecordingEventType::SostenutoOn
                ) {

                    playbackSostenuto =
                        true;

                    for (
                        auto& playing :
                        activeNotes
                    ) {

                        if (
                            playing.sound &&
                            playing.sound
                                ->getStatus()
                            ==
                            sf::Sound::Playing
                        ) {

                            playing
                                .sostenutoCaptured
                                =
                                true;
                        }
                    }
                }


                // SOSTENUTO OFF

                else if (
                    recorded.type ==
                    RecordingEventType::SostenutoOff
                ) {

                    playbackSostenuto =
                        false;

                    for (
                        auto it =
                            activeNotes.begin();
                        it !=
                            activeNotes.end();
                    ) {

                        if (
                            it->sostenutoCaptured
                        ) {

                            it->sostenutoCaptured =
                                false;

                            if (
                                !it->keyHeld &&
                                !playbackSustain
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


                playbackIndex++;
            }


            // PLAYBACK FINISHED

            if (
                playbackIndex >=
                recordingEvents.size()
            ) {

                playingRecording =
                    false;

                playbackSustain = false;
                playbackSoft = false;
                playbackSostenuto = false;

                currentNote.setString(
                    "Playback finished"
                );

                recordingStatus.setString(
                    "F8: Training   F9: Record   F10: Play   F11: Stop"
                );

                recordingStatus.setFillColor(
                    sf::Color(
                        180,
                        180,
                        180
                    )
                );
            }
        }


        // ==================================================
        // REMOVE FINISHED SOUNDS
        // ==================================================

        for (
            auto it =
                activeNotes.begin();
            it !=
                activeNotes.end();
        ) {

            if (
                it->sound->getStatus()
                ==
                sf::Sound::Stopped
            ) {

                it =
                    activeNotes.erase(
                        it
                    );

            } else {

                ++it;
            }
        }


        // ==================================================
        // DRAW
        // ==================================================

        window.clear(
            background
        );


        window.draw(
            pianoBody
        );

        window.draw(
            topPanel
        );

        window.draw(
            title
        );

        window.draw(
            currentNote
        );

        window.draw(
            recordingStatus
        );

        window.draw(
            brand
        );

        window.draw(
            trainingText
        );


        // ==================================================
        // TRAINING TARGET
        // ==================================================

        if (
            trainingMode &&
            trainingIndex < trainingSong.size()
        ) {

            const std::string& targetNote =
                trainingSong[trainingIndex];

            for (
                const auto& pianoKey :
                keys
            ) {

                if (
                    pianoKey.note ==
                    targetNote
                ) {

                    trainingTarget.setPosition(
                        pianoKey.shape.getPosition()
                    );

                    trainingTarget.setSize(
                        pianoKey.shape.getSize()
                    );

                    if (pianoKey.black) {

                        trainingTarget.setOutlineThickness(
                            5.f
                        );

                    } else {

                        trainingTarget.setOutlineThickness(
                            4.f
                        );
                    }

                    window.draw(
                        trainingTarget
                    );

                    break;
                }
            }
        }


        // ==================================================
        // WHITE KEYS
        // ==================================================

        for (
            const auto& pianoKey :
            keys
        ) {

            if (!pianoKey.black) {

                window.draw(
                    pianoKey.shape
                );

                window.draw(
                    pianoKey.label
                );
            }
        }


        // ==================================================
        // BLACK KEYS
        // ==================================================

        for (
            const auto& pianoKey :
            keys
        ) {

            if (pianoKey.black) {

                window.draw(
                    pianoKey.shape
                );

                window.draw(
                    pianoKey.label
                );
            }
        }


        // ==================================================
        // DRAW TRAINING TARGET AGAIN
        // ==================================================
        //
        // Target is drawn again so it stays visible
        // above the piano keys.
        //

        if (
            trainingMode &&
            trainingIndex < trainingSong.size()
        ) {

            const std::string& targetNote =
                trainingSong[trainingIndex];

            for (
                const auto& pianoKey :
                keys
            ) {

                if (
                    pianoKey.note ==
                    targetNote
                ) {

                    trainingTarget.setPosition(
                        pianoKey.shape.getPosition()
                    );

                    trainingTarget.setSize(
                        pianoKey.shape.getSize()
                    );

                    window.draw(
                        trainingTarget
                    );

                    break;
                }
            }
        }


        // ==================================================
        // PEDALS
        // ==================================================

        for (
            const auto& pedal :
            pedals
        ) {

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