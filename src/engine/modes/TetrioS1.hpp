#pragma once


// points callable concept is satisfied by the TetrioS1 struct
class TetrioS1 {
public:

    enum damage_type {
        classic_guideline,
        modern_guideline
    };
    static struct GarbageValues {
        static constexpr int SINGLE = 0;
        static constexpr int DOUBLE = 1;
        static constexpr int TRIPLE = 2;
        static constexpr int QUAD = 4;
        static constexpr int TSPIN_MINI = 0;
        static constexpr int TSPIN = 0;
        static constexpr int TSPIN_MINI_SINGLE = 0;
        static constexpr int TSPIN_SINGLE = 2;
        static constexpr int TSPIN_MINI_DOUBLE = 1;
        static constexpr int TSPIN_DOUBLE = 4;
        static constexpr int TSPIN_TRIPLE = 6;
        static constexpr int TSPIN_QUAD = 10;
        static constexpr int BACKTOBACK_BONUS = 1;
        static constexpr float BACKTOBACK_BONUS_LOG = .8f;
        static constexpr int COMBO_MINIFIER = 1;
        static constexpr float COMBO_MINIFIER_LOG = 1.25;
        static constexpr float COMBO_BONUS = .25;
        static constexpr int ALL_CLEAR = 10;
        static constexpr std::array<std::array<int, 13>, 2> combotable = { {{0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5},
                                                                {0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4}} };
    }GarbageValues;  // namespace GarbageValues


    // tetrio stuff
    u16 currentcombopower = 0;
    u16 currentbtbchainpower = 0;

    static struct options {
        static constexpr u8 garbagemultiplier = 1;
        static constexpr bool b2bchaining = true;
    } options;


    // points function
    std::size_t points(int linesCleared, spinType spin, bool pc, auto&combo, auto&b2b) {
        auto maintainsB2B = false;

        if (linesCleared) {
            combo++;
            if (4 == linesCleared || spin != spinType::null)
                maintainsB2B = true;

            if (maintainsB2B)
                b2b++;
            else
                b2b = 0;
        }
        else {
            combo = 0;
            currentcombopower = 0;
        }

        int garbage = 0;

        switch (linesCleared) {
        case 0:
            if (spinType::mini == spin)
                garbage = GarbageValues::TSPIN_MINI;
            else if (spinType::normal == spin)
                garbage = GarbageValues::TSPIN;
            break;
        case 1:
            if (spinType::mini == spin)
                garbage = GarbageValues::TSPIN_MINI_SINGLE;
            else if (spinType::normal == spin)
                garbage = GarbageValues::TSPIN_SINGLE;
            else
                garbage = GarbageValues::SINGLE;
            break;

        case 2:
            if (spinType::mini == spin)
                garbage = GarbageValues::TSPIN_MINI_DOUBLE;
            else if (spinType::normal == spin)
                garbage = GarbageValues::TSPIN_DOUBLE;
            else
                garbage = GarbageValues::DOUBLE;
            break;

        case 3:
            if (spin != spinType::null)
                garbage = GarbageValues::TSPIN_TRIPLE;
            else
                garbage = GarbageValues::TRIPLE;
            break;

        case 4:
            if (spin != spinType::null)
                garbage = GarbageValues::TSPIN_QUAD;

            else
                garbage = GarbageValues::QUAD;

            break;
        }

        if (linesCleared) {
            if (b2b > 1) {
                if (options.b2bchaining) {
                    const int b2bGarbage = GarbageValues::BACKTOBACK_BONUS *
                        (1 + std::log1p((b2b - 1) * GarbageValues::BACKTOBACK_BONUS_LOG) + (b2b - 1 <= 1 ? 0 : (1 + std::log1p((b2b - 1) * GarbageValues::BACKTOBACK_BONUS_LOG) - (int)std::log1p((b2b - 1) * GarbageValues::BACKTOBACK_BONUS_LOG)) / 3));

                    garbage += b2bGarbage;

                    if (b2bGarbage > currentbtbchainpower)
                        currentbtbchainpower = b2bGarbage;

                }
                else
                    garbage += GarbageValues::BACKTOBACK_BONUS;

            }
            else
                currentbtbchainpower = 0;

        }

        if (combo > 1)
            garbage *= 1 + GarbageValues::COMBO_BONUS * (combo - 1);  // Fucking broken ass multiplier :)


        if (combo > 2) 
            garbage = std::max((int)std::log1p(GarbageValues::COMBO_MINIFIER * (combo - 1) * GarbageValues::COMBO_MINIFIER_LOG), garbage);


        const int finalGarbage = garbage * options.garbagemultiplier;
        if (combo > 2)
            currentcombopower = std::max((int)currentcombopower, finalGarbage);

        const auto combinedGarbage = finalGarbage + (pc ? GarbageValues::ALL_CLEAR : 0);

        return combinedGarbage;
	}
};