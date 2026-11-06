/*
yafth.h v1.0.0 - MIT or public domain - https://github.com/gshigin/yafth.h
SPDX-License-Identifier: MIT OR Unlicense

Fallout-style terminal hacking for C and C++. The caller owns the game
state and field buffer. The engine uses no dynamic allocation.

Example (one C or C++ source file):
    #include <stdio.h>

    #define YAFTH_IMPLEMENTATION
    #include "yafth.h"

    int main(void) {
        const uint64_t seed = 42;
        yafth_state state;
        char field[YAFTH_BUFFER_SIZE];
        if (yafth_init(&state, YAFTH_AVERAGE, 50, seed) != YAFTH_OK) {
            return 1;
        }
        yafth_fill_buffer(&state, field);

        const char border[] = "+-----------------------------------------------------+";
        const int width = (int)sizeof(border) - 3;
        const unsigned attempts = (unsigned)yafth_attempts_left(&state);
        char status[sizeof(border)];
        snprintf(status, sizeof(status), "%u ATTEMPT(S) LEFT : %.*s",
                 attempts, (int)(2 * attempts), "# # # # ");

        puts(border);
        printf("|%-*s|\n", width, "ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL");
        printf("|%-*s|\n", width, "ENTER PASSWORD NOW");
        printf("|%-*s|\n", width, "");
        printf("|%-*s|\n", width, status);
        printf("|%-*s|\n", width, "");

        const unsigned base = (unsigned)(seed & 0xffffu);
        const unsigned column_size = YAFTH_BUFFER_SIZE / 2;
        for (unsigned left = 0; left < column_size; left += YAFTH_ROW_WIDTH) {
            const unsigned right = column_size + left;
            const char *prompt = right + YAFTH_ROW_WIDTH == YAFTH_BUFFER_SIZE ? ">" : "";
            // Limit each %s to one row. The field has no null terminator.
            printf("|0x%04X %.*s 0x%04X %.*s %-*s|\n",
                   (base + left) & 0xffffu, YAFTH_ROW_WIDTH, field + left,
                   (base + right) & 0xffffu, YAFTH_ROW_WIDTH, field + right,
                   YAFTH_ROW_WIDTH + 1, prompt);
        }
        puts(border);
        return 0;
    }

Optional macros, defined before including this file:
    YAFTH_DEF                 Prefix for public functions. Default is empty.
                             Use static inline in a single translation unit.
    YAFTH_ASSERT(condition)  Assertion hook. Defaults to assert(condition).

Requires little-endian byte order, 8-bit bytes, and ASCII.
See the end of this file for the changelog and license.
*/

#ifndef YAFTH_H_INCLUDED
#define YAFTH_H_INCLUDED

#include <stdint.h>

#ifndef YAFTH_DEF
#define YAFTH_DEF
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
    YAFTH_ROW_WIDTH = 12,
    YAFTH_BUFFER_SIZE = 408
};

typedef enum yafth_difficulty {
    YAFTH_VERY_EASY,
    YAFTH_EASY,
    YAFTH_AVERAGE,
    YAFTH_HARD,
    YAFTH_VERY_HARD
} yafth_difficulty;

typedef enum yafth_init_result {
    YAFTH_OK,
    YAFTH_INVALID_DIFFICULTY,
    YAFTH_INVALID_SCIENCE,
    YAFTH_INSUFFICIENT_SCIENCE
} yafth_init_result;

typedef enum yafth_game_status {
    YAFTH_ACTIVE,
    YAFTH_WON,
    YAFTH_LOCKED
} yafth_game_status;

typedef enum yafth_click_kind {
    YAFTH_IGNORED,
    YAFTH_DUD_REMOVED,
    YAFTH_ALLOWANCE_REPLENISHED,
    YAFTH_ENTRY_DENIED,
    YAFTH_LOCKOUT,
    YAFTH_EXACT_MATCH
} yafth_click_kind;

// Highlighted range [begin, end).
typedef struct yafth_range {
    uint32_t begin;
    uint32_t end;
} yafth_range;

// The counts are set for YAFTH_ENTRY_DENIED and YAFTH_LOCKOUT.
typedef struct yafth_click_result {
    yafth_click_kind kind;
    yafth_range selected_range;     // Range in the field before the click.
    uint32_t matching_letter_count; // Letters matching the answer at the same positions.
    uint32_t word_length;
} yafth_click_result;

// Caller-owned, copyable 64-byte state. Treat its fields as private.
typedef struct yafth_state {
    uint32_t field_seed;
    uint32_t bracket_rng;
    uint16_t word_entries[20]; // 4 bits for likeness, 6 for ID, 6 for offset, high to low.
    uint8_t used_openings[9];  // Two bits per row, one for each opener.
    uint8_t removed_words[3];  // One bit per word slot.
    uint8_t word_parameters;   // Length minus 4 in the high nibble, count minus 5 in the low nibble.
    uint8_t slot_width;
    uint8_t attempts_and_flags;
    uint8_t answer_slot;
} yafth_state;

// Start a game with the given difficulty, Science (0 to 100), and seed.
// The same inputs reproduce the game within this version. On error, state is unchanged.
YAFTH_DEF yafth_init_result yafth_init(yafth_state *state, yafth_difficulty difficulty, uint32_t science, uint64_t seed);

// Return the range of a live word or a selectable bracket pair at its opener.
// Otherwise return the range of the single character at position.
// Require position < YAFTH_BUFFER_SIZE.
YAFTH_DEF yafth_range yafth_look_at(const yafth_state *state, uint32_t position);

// Clicks after a win or lockout are ignored.
// Clicking a selectable opener marks it used, even if no reward remains
// and the result is YAFTH_IGNORED.
// Require position < YAFTH_BUFFER_SIZE.
YAFTH_DEF yafth_click_result yafth_click_at(yafth_state *state, uint32_t position);

// Write YAFTH_BUFFER_SIZE bytes without \0. The buffer must not overlap state.
YAFTH_DEF void yafth_fill_buffer(const yafth_state *state, char buffer[YAFTH_BUFFER_SIZE]);

YAFTH_DEF uint32_t yafth_attempts_left(const yafth_state *state);
YAFTH_DEF yafth_game_status yafth_status(const yafth_state *state);

#ifdef __cplusplus
}
#endif
#endif // YAFTH_H_INCLUDED

// Implementation
#if defined(YAFTH_IMPLEMENTATION) && !defined(YAFTH_IMPLEMENTED)
#define YAFTH_IMPLEMENTED

#include <limits.h>
#include <stddef.h>
#include <string.h>

#ifndef YAFTH_ASSERT
#include <assert.h>
#define YAFTH_ASSERT assert
#endif

#if defined(__cplusplus)
#define YAFTH_STATIC_ASSERT(condition, message) static_assert(condition, #message)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define YAFTH_STATIC_ASSERT(condition, message) _Static_assert(condition, #message)
#else
#define YAFTH_STATIC_ASSERT(condition, message) typedef char yafth_static_assert_##message[(condition) ? 1 : -1]
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
YAFTH_STATIC_ASSERT(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, requires_little_endian);
#elif !defined(_WIN32) || !(defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC))
#error "yafth requires a known little-endian target"
#endif

YAFTH_STATIC_ASSERT(CHAR_BIT == 8 && 'A' == 65 && 'Z' == 90 && '.' == 46, requires_8_bit_bytes_and_ascii);
YAFTH_STATIC_ASSERT(sizeof(yafth_state) == 64, state_must_be_64_bytes);

enum {
    YAFTH_WORD_OFFSET_BITS = 6,
    YAFTH_WORD_OFFSET_MASK = (1u << (unsigned)YAFTH_WORD_OFFSET_BITS) - 1u,
    YAFTH_DICTIONARY_ID_BITS = 6,
    YAFTH_DICTIONARY_ID_MASK = (1u << (unsigned)YAFTH_DICTIONARY_ID_BITS) - 1u,
    YAFTH_DICTIONARY_ID_COUNT = YAFTH_DICTIONARY_ID_MASK + 1,
    YAFTH_ID_DRAW_LIMIT = 64,
    YAFTH_LIKENESS_SHIFT = YAFTH_WORD_OFFSET_BITS + YAFTH_DICTIONARY_ID_BITS,
    YAFTH_MIN_WORD_LENGTH = 4,
    YAFTH_MAX_WORD_LENGTH = 12,
    YAFTH_MIN_WORD_COUNT = 5,
    YAFTH_MAX_WORD_COUNT = sizeof(((yafth_state *)0)->word_entries) / sizeof(uint16_t),
    YAFTH_ATTEMPT_MASK = 0x07,
    YAFTH_COMPLETED_FLAG = 0x08,
    YAFTH_REFILLED_FLAG = 0x10,
    YAFTH_MAX_ATTEMPTS = 4
};

// Field geometry
YAFTH_STATIC_ASSERT(YAFTH_BUFFER_SIZE % 8 == 0, filler_chunks_must_fill_field);
YAFTH_STATIC_ASSERT(YAFTH_BUFFER_SIZE % YAFTH_ROW_WIDTH == 0, rows_must_fill_field);
YAFTH_STATIC_ASSERT(YAFTH_BUFFER_SIZE / YAFTH_ROW_WIDTH <= sizeof(((yafth_state *)0)->used_openings) * CHAR_BIT / 2u, opening_bits_must_fit);
YAFTH_STATIC_ASSERT(YAFTH_ROW_WIDTH <= 16, bracket_columns_must_fit);
YAFTH_STATIC_ASSERT(YAFTH_ROW_WIDTH <= (unsigned)YAFTH_BUFFER_SIZE / YAFTH_MAX_WORD_COUNT, row_must_cross_at_most_two_slots);
YAFTH_STATIC_ASSERT((unsigned)YAFTH_BUFFER_SIZE / YAFTH_MAX_WORD_COUNT >= YAFTH_MAX_WORD_LENGTH + 2, slot_must_fit_word_and_fillers);

// Packed fields
YAFTH_STATIC_ASSERT(YAFTH_BUFFER_SIZE <= UINT16_MAX, selection_positions_must_fit);
YAFTH_STATIC_ASSERT((unsigned)YAFTH_BUFFER_SIZE / YAFTH_MIN_WORD_COUNT <= UINT8_MAX, slot_width_must_fit);
YAFTH_STATIC_ASSERT(YAFTH_MAX_WORD_COUNT - YAFTH_MIN_WORD_COUNT <= 15, word_count_must_fit_parameter);
YAFTH_STATIC_ASSERT(YAFTH_MAX_WORD_LENGTH < (1u << (16u - YAFTH_LIKENESS_SHIFT)), likeness_must_fit_word_entry);
YAFTH_STATIC_ASSERT(YAFTH_MAX_WORD_COUNT <= 3 * CHAR_BIT, removed_word_mask_must_fit);
YAFTH_STATIC_ASSERT(YAFTH_MAX_ATTEMPTS <= YAFTH_ATTEMPT_MASK, attempts_must_fit_below_flags);

// Dictionary and word operations
YAFTH_STATIC_ASSERT(YAFTH_MIN_WORD_LENGTH >= 4 && YAFTH_MAX_WORD_LENGTH <= 12, word_lengths_must_fit_block_loads);
YAFTH_STATIC_ASSERT(YAFTH_DICTIONARY_ID_COUNT <= 64, chosen_id_mask_must_fit);
YAFTH_STATIC_ASSERT(YAFTH_MAX_WORD_COUNT <= YAFTH_DICTIONARY_ID_COUNT, dictionary_must_fill_all_slots);

static const char *yafth_dictionary_words(uint32_t word_length);

// Random streams
static inline uint32_t yafth_xorshift32_next(uint32_t *state) {
    uint32_t bits = *state;
    bits ^= bits << 13u;
    bits ^= bits >> 17u;
    bits ^= bits << 5u;
    return *state = bits;
}

static inline uint64_t yafth_xorshift64_next(uint64_t *state) {
    uint64_t bits = *state;
    bits ^= bits << 13u;
    bits ^= bits >> 7u;
    bits ^= bits << 17u;
    return *state = bits;
}

// Mix the input seed with one SplitMix64 step.
static uint32_t yafth_seed32_from_splitmix64(uint64_t seed) {
    seed += UINT64_C(0x9e3779b97f4a7c15);
    seed = (seed ^ (seed >> 30u)) * UINT64_C(0xbf58476d1ce4e5b9);
    seed = (seed ^ (seed >> 27u)) * UINT64_C(0x94d049bb133111eb);
    const uint32_t mixed = (uint32_t)(seed ^ (seed >> 31u));
    // Xorshift32 stays at zero if seeded with zero.
    return mixed == 0 ? 1u : mixed;
}

static inline uint64_t yafth_seed64_from_seed32(uint32_t seed) {
    // The XOR constant keeps the combined seed nonzero.
    return ((uint64_t)seed << 32u) | (seed ^ UINT32_C(0xe313a38d));
}

// Packed state access
static inline int yafth_test_bit(const uint8_t *mask, uint32_t bit_index) {
    return (mask[bit_index / 8u] & (1u << (bit_index % 8u))) != 0;
}

static inline void yafth_set_bit(uint8_t *mask, uint32_t bit_index) {
    mask[bit_index / 8u] |= (uint8_t)(1u << (bit_index % 8u));
}

static inline uint32_t yafth_load_mask24(const uint8_t *mask) {
    return (uint32_t)mask[0] | ((uint32_t)mask[1] << 8u) | ((uint32_t)mask[2] << 16u);
}

static inline uint32_t yafth_dictionary_id(uint32_t word_entry) {
    return (word_entry >> (unsigned)YAFTH_WORD_OFFSET_BITS) & (unsigned)YAFTH_DICTIONARY_ID_MASK;
}

static inline uint32_t yafth_word_offset(uint32_t word_entry) {
    return word_entry & (unsigned)YAFTH_WORD_OFFSET_MASK;
}

static inline uint32_t yafth_word_length(const yafth_state *state) {
    return YAFTH_MIN_WORD_LENGTH + (state->word_parameters >> 4u);
}

static inline uint32_t yafth_word_count(const yafth_state *state) {
    return YAFTH_MIN_WORD_COUNT + (state->word_parameters & 0x0fu);
}

// memcpy handles unaligned loads without an aliasing violation.
static inline uint32_t yafth_load_u32_le(const char *source) {
    uint32_t bits;
    memcpy(&bits, source, sizeof(bits));
    return bits;
}

static inline uint64_t yafth_load_u64_le(const char *source) {
    uint64_t bits;
    memcpy(&bits, source, sizeof(bits));
    return bits;
}

static inline const char *yafth_dictionary_word(const char *dictionary, uint32_t word_length, uint32_t word_entry) {
    return dictionary + (size_t)(yafth_dictionary_id(word_entry) * word_length);
}

// Word setup
// Count zero bytes in the XOR of uppercase ASCII letters.
static inline uint32_t yafth_count_matching_bytes(uint64_t xor_bytes) {
    // Each byte is at most 31, so adding 0x7f flags nonzero bytes without cross-byte carries.
    const uint64_t match_flags = (~(xor_bytes + UINT64_C(0x7f7f7f7f7f7f7f7f)) & UINT64_C(0x8080808080808080)) >> 7u;
    return (uint32_t)((match_flags * UINT64_C(0x0101010101010101)) >> 56u);
}

// Write word_count distinct dictionary IDs and return the advanced RNG state.
static inline uint32_t yafth_pick_word_ids(uint16_t *word_entries, uint32_t word_count, uint32_t rng) {
    // One RNG draw gives five 6-bit candidates.
    YAFTH_ASSERT(rng != 0);
    uint64_t chosen_id_mask = 0;
    uint32_t selected_count = 0;
    for (uint32_t draw = 0; draw < YAFTH_ID_DRAW_LIMIT && selected_count < word_count; ++draw) {
        uint32_t candidate_bits = yafth_xorshift32_next(&rng);
        for (uint32_t candidates_left = 32u / YAFTH_DICTIONARY_ID_BITS; candidates_left > 0 && selected_count < word_count; --candidates_left) {
            const uint32_t dictionary_id = candidate_bits & (unsigned)YAFTH_DICTIONARY_ID_MASK;
            candidate_bits >>= (unsigned)YAFTH_DICTIONARY_ID_BITS;
            const uint64_t id_mask = UINT64_C(1) << dictionary_id;
            if ((chosen_id_mask & id_mask) == 0) {
                chosen_id_mask |= id_mask;
                word_entries[selected_count++] = (uint16_t)(dictionary_id << (unsigned)YAFTH_WORD_OFFSET_BITS);
            }
        }
    }
    // Finish in ID order if the draw budget runs out.
    for (uint32_t dictionary_id = 0; dictionary_id < YAFTH_DICTIONARY_ID_COUNT && selected_count < word_count; ++dictionary_id) {
        if ((chosen_id_mask & (UINT64_C(1) << dictionary_id)) == 0) {
            word_entries[selected_count++] = (uint16_t)(dictionary_id << (unsigned)YAFTH_WORD_OFFSET_BITS);
        }
    }
    YAFTH_ASSERT(selected_count == word_count);
    return rng;
}

static inline uint32_t yafth_place_words(yafth_state *state, uint32_t word_count, uint32_t word_length, uint32_t rng) {
    // Reserve two filler cells at the end of each slot.
    YAFTH_ASSERT(state->slot_width >= word_length + 2u);
    const uint32_t offset_count_available = state->slot_width - word_length - 1u;
    const uint32_t offset_count = offset_count_available < YAFTH_WORD_OFFSET_MASK + 1u ? offset_count_available : YAFTH_WORD_OFFSET_MASK + 1u;
    uint32_t slot_index = 0;
    // Scale each 16-bit half to a slot offset. One draw places two words.
    for (; slot_index + 1u < word_count; slot_index += 2u) {
        const uint32_t offset_bits = yafth_xorshift32_next(&rng);
        state->word_entries[slot_index] |= (uint16_t)(((offset_bits & 0xffffu) * offset_count) >> 16u);
        state->word_entries[slot_index + 1u] |= (uint16_t)(((offset_bits >> 16u) * offset_count) >> 16u);
    }
    if (slot_index < word_count) {
        state->word_entries[slot_index] |= (uint16_t)(((yafth_xorshift32_next(&rng) & 0xffffu) * offset_count) >> 16u);
    }
    return rng;
}

static inline void yafth_cache_likeness(yafth_state *state, uint32_t word_count, uint32_t word_length) {
    const char *dictionary = yafth_dictionary_words(word_length);
    const char *answer = yafth_dictionary_word(dictionary, word_length, state->word_entries[state->answer_slot]);
    const uint32_t tail_offset = word_length - 4u;
    const uint32_t answer_tail = yafth_load_u32_le(answer + tail_offset);
    // The head and tail loads stay inside the word. Drop the overlapping bytes
    // and subtract the matches from zero padding.
    if (word_length <= 8u) {
        const uint32_t answer_head = yafth_load_u32_le(answer);
        const uint32_t overlap_bits = (8u - word_length) * 8u;
        for (uint32_t slot_index = 0; slot_index < word_count; ++slot_index) {
            const char *word = yafth_dictionary_word(dictionary, word_length, state->word_entries[slot_index]);
            const uint64_t head_xor = yafth_load_u32_le(word) ^ answer_head;
            const uint64_t tail_xor = (uint64_t)(yafth_load_u32_le(word + tail_offset) ^ answer_tail) >> overlap_bits;
            const uint32_t likeness = yafth_count_matching_bytes(head_xor | (tail_xor << 32u)) - (8u - word_length);
            state->word_entries[slot_index] |= (uint16_t)(likeness << (unsigned)YAFTH_LIKENESS_SHIFT);
        }
    } else {
        const uint64_t answer_head = yafth_load_u64_le(answer);
        const uint32_t overlap_bits = (12u - word_length) * 8u;
        for (uint32_t slot_index = 0; slot_index < word_count; ++slot_index) {
            const char *word = yafth_dictionary_word(dictionary, word_length, state->word_entries[slot_index]);
            const uint64_t head_xor = yafth_load_u64_le(word) ^ answer_head;
            const uint64_t tail_xor = (uint64_t)(yafth_load_u32_le(word + tail_offset) ^ answer_tail) >> overlap_bits;
            const uint32_t likeness = yafth_count_matching_bytes(head_xor) + yafth_count_matching_bytes(tail_xor) - (16u - word_length);
            state->word_entries[slot_index] |= (uint16_t)(likeness << (unsigned)YAFTH_LIKENESS_SHIFT);
        }
    }
}

YAFTH_DEF yafth_init_result yafth_init(yafth_state *state, yafth_difficulty difficulty, uint32_t science, uint64_t seed) {
    const uint32_t level = (uint32_t)difficulty;
    if (level > YAFTH_VERY_HARD) {
        return YAFTH_INVALID_DIFFICULTY;
    }
    if (science > 100u) {
        return YAFTH_INVALID_SCIENCE;
    }
    const uint32_t min_science = level == YAFTH_VERY_EASY ? 15u : level * 25u;
    if (science < min_science) {
        return YAFTH_INSUFFICIENT_SCIENCE;
    }

    memset(state, 0, sizeof(*state));
    uint32_t rng = yafth_seed32_from_splitmix64(seed);
    const uint32_t word_length = level == YAFTH_VERY_HARD ? (uint32_t)YAFTH_MAX_WORD_LENGTH : YAFTH_MIN_WORD_LENGTH + 2u * level + (rng & 0x01u);
    // Use a fixed count for Very Hard because its minimum Science is 100.
    // See https://geckwiki.com/index.php?title=Hacking_Word_Count
    const uint32_t word_count =
        level == YAFTH_VERY_HARD ? 12u : YAFTH_MIN_WORD_COUNT + (YAFTH_MAX_WORD_COUNT - YAFTH_MIN_WORD_COUNT) * (100u - science) / (100u - 25u * level);
    YAFTH_ASSERT(word_count >= YAFTH_MIN_WORD_COUNT);
    YAFTH_ASSERT(word_count <= YAFTH_MAX_WORD_COUNT);

    state->word_parameters = (uint8_t)(((word_length - YAFTH_MIN_WORD_LENGTH) << 4u) | (word_count - YAFTH_MIN_WORD_COUNT));
    state->slot_width = (uint8_t)(YAFTH_BUFFER_SIZE / word_count);
    state->attempts_and_flags = YAFTH_MAX_ATTEMPTS;

    rng = yafth_pick_word_ids(state->word_entries, word_count, rng);
    state->answer_slot = (uint8_t)(yafth_xorshift32_next(&rng) % word_count);
    rng = yafth_place_words(state, word_count, word_length, rng);

    // Keep the field seed fixed. Only bracket rewards advance bracket_rng.
    state->field_seed = rng;
    state->bracket_rng = rng;

    yafth_cache_likeness(state, word_count, word_length);
    return YAFTH_OK;
}

// Derive each row key directly so hover can regenerate just one row.
static const uint32_t yafth_row_key_step = UINT32_C(0x9e3779b9);
static const char yafth_bracket_symbols[] = "{}[]()<>";

// The low two nibbles hold the opener and closer columns. The bracket type follows.
static inline uint32_t yafth_bracket_opener_column(uint32_t pair) {
    return pair & 0x0fu;
}

static inline uint32_t yafth_bracket_closer_column(uint32_t pair) {
    return (pair >> 4u) & 0x0fu;
}

static inline uint32_t yafth_bracket_type(uint32_t pair) {
    return pair >> 8u;
}

static inline uint32_t yafth_bracket_pair_from_bits(uint32_t bits, int enabled) {
    const uint32_t column_a = bits & 0x0fu;
    const uint32_t column_b = (bits >> 4u) & 0x0fu;
    const uint32_t opener_column = column_a < column_b ? column_a : column_b;
    const uint32_t closer_column = column_a < column_b ? column_b : column_a;
    return enabled && closer_column < YAFTH_ROW_WIDTH && column_a != column_b ? (bits & 0x300u) | (closer_column << 4u) | opener_column : 0;
}

// Pack one pair per 16-bit half. A zero half means no pair.
static uint32_t yafth_row_brackets(uint32_t row_key) {
    yafth_xorshift32_next(&row_key);

    // Give the second candidate fewer chances so most rows have at most one pair.
    const uint32_t first_pair = yafth_bracket_pair_from_bits(row_key, (row_key & (UINT32_C(0x03) << 10u)) != 0);
    uint32_t second_pair = yafth_bracket_pair_from_bits(row_key >> 16u, (row_key & (UINT32_C(0x03) << 26u)) == 0);
    if (first_pair != 0 && second_pair != 0) {
        const uint32_t opener_first = yafth_bracket_opener_column(first_pair);
        const uint32_t closer_first = yafth_bracket_closer_column(first_pair);
        const uint32_t opener_second = yafth_bracket_opener_column(second_pair);
        const uint32_t closer_second = yafth_bracket_closer_column(second_pair);
        if (opener_first == opener_second || opener_first == closer_second || closer_first == opener_second) {
            second_pair = 0;
        } else if (closer_first == closer_second) {
            // A shared closer requires both openers to use its type.
            second_pair = (first_pair & 0x300u) | (second_pair & 0xffu);
        }
    }
    return first_pair | (second_pair << 16u);
}

// Filler pass
// Map eight indices in [0, 15], one per byte, to !"#$%&'*+,-./^_`.
static inline uint64_t yafth_filler_ascii(uint64_t indices) {
    // Start at '!' + index. Skip two ASCII gaps by adding 2 at index >= 7
    // and 46 at index >= 13. Adding 128 minus a threshold sets the high bit
    // of each qualifying byte, with no carry into the next byte.
    const uint64_t byte_ones = UINT64_C(0x0101010101010101);
    const uint64_t byte_high_mask = 0x80u * byte_ones;
    const uint64_t after_apostrophe = ((indices + (0x80u - 7u) * byte_ones) & byte_high_mask) >> 7u;
    const uint64_t after_slash = ((indices + (0x80u - 13u) * byte_ones) & byte_high_mask) >> 7u;
    return indices + 0x21u * byte_ones + 2u * after_apostrophe + 46u * after_slash;
}

static inline void yafth_write_filler(char buffer[YAFTH_BUFFER_SIZE], uint32_t field_seed) {
    uint64_t filler_rng = yafth_seed64_from_seed32(field_seed);
    const uint64_t low_nibble_mask = UINT64_C(0x0f0f0f0f0f0f0f0f);
    uint32_t position = 0;
    for (; position + 16u <= YAFTH_BUFFER_SIZE; position += 16u) {
        const uint64_t bits = yafth_xorshift64_next(&filler_rng);
        // Low nibbles give the first eight characters, high nibbles the next eight.
        const uint64_t first_chunk = yafth_filler_ascii(bits & low_nibble_mask);
        const uint64_t second_chunk = yafth_filler_ascii((bits >> 4u) & low_nibble_mask);
        memcpy(buffer + position, &first_chunk, sizeof(first_chunk));
        memcpy(buffer + position + 8u, &second_chunk, sizeof(second_chunk));
    }
    if (YAFTH_BUFFER_SIZE % 16u != 0) {
        // The field is a multiple of eight bytes, so at most one chunk remains.
        const uint64_t last_chunk = yafth_filler_ascii(yafth_xorshift64_next(&filler_rng) & low_nibble_mask);
        memcpy(buffer + position, &last_chunk, sizeof(last_chunk));
    }
}

// Bracket pass
static inline void yafth_write_bracket_pair(char *row, uint32_t pair) {
    if (pair != 0) {
        const uint32_t glyph_index = 2u * yafth_bracket_type(pair);
        row[yafth_bracket_opener_column(pair)] = yafth_bracket_symbols[glyph_index];
        row[yafth_bracket_closer_column(pair)] = yafth_bracket_symbols[glyph_index + 1u];
    }
}

static inline void yafth_overlay_brackets(char buffer[YAFTH_BUFFER_SIZE], uint32_t field_seed) {
    uint32_t row_key = field_seed + yafth_row_key_step;
    for (uint32_t row_begin = 0; row_begin < YAFTH_BUFFER_SIZE; row_begin += YAFTH_ROW_WIDTH, row_key += yafth_row_key_step) {
        const uint32_t pairs = yafth_row_brackets(row_key);
        char *row = buffer + row_begin;
        yafth_write_bracket_pair(row, pairs & UINT32_C(0xffff));
        yafth_write_bracket_pair(row, pairs >> 16u);
    }
}

// Word pass
static inline void yafth_copy_word(char *target, const char *source, uint32_t word_length) {
    // Use fixed-size copies so fortified builds can inline them.
    // The head and tail stores write the same bytes where they overlap.
    if (word_length < 8u) {
        const uint32_t head = yafth_load_u32_le(source);
        const uint32_t tail = yafth_load_u32_le(source + word_length - sizeof(head));
        memcpy(target, &head, sizeof(head));
        memcpy(target + word_length - sizeof(tail), &tail, sizeof(tail));
    } else {
        const uint64_t head = yafth_load_u64_le(source);
        const uint64_t tail = yafth_load_u64_le(source + word_length - sizeof(head));
        memcpy(target, &head, sizeof(head));
        memcpy(target + word_length - sizeof(tail), &tail, sizeof(tail));
    }
}

static inline void yafth_overlay_words(char buffer[YAFTH_BUFFER_SIZE], const yafth_state *state) {
    const uint32_t word_length = yafth_word_length(state);
    const char *dictionary = yafth_dictionary_words(word_length);
    uint32_t removed_words_mask = yafth_load_mask24(state->removed_words);
    const uint32_t word_count = yafth_word_count(state);
    for (uint32_t slot_index = 0, slot_begin = 0; slot_index < word_count; ++slot_index, slot_begin += state->slot_width, removed_words_mask >>= 1u) {
        const uint32_t word_entry = state->word_entries[slot_index];
        char *target = buffer + slot_begin + yafth_word_offset(word_entry);
        if ((removed_words_mask & 0x01u) != 0) {
            memset(target, '.', word_length);
        } else {
            const char *source = yafth_dictionary_word(dictionary, word_length, word_entry);
            yafth_copy_word(target, source, word_length);
        }
    }
}

YAFTH_DEF void yafth_fill_buffer(const yafth_state *state, char buffer[YAFTH_BUFFER_SIZE]) {
    // Brackets overwrite filler. Words and removed-word dots overwrite both.
    yafth_write_filler(buffer, state->field_seed);
    yafth_overlay_brackets(buffer, state->field_seed);
    yafth_overlay_words(buffer, state);
}

// Selection and clicks
// Return YAFTH_BUFFER_SIZE for a slot past the last word.
static inline uint32_t yafth_word_begin(const yafth_state *state, uint32_t slot_index) {
    return slot_index < yafth_word_count(state) ? slot_index * state->slot_width + yafth_word_offset(state->word_entries[slot_index])
                                                : (uint32_t)YAFTH_BUFFER_SIZE;
}

enum {
    YAFTH_SELECT_SYMBOL,
    YAFTH_SELECT_WORD,
    YAFTH_SELECT_BRACKET
};

typedef struct yafth_selection {
    uint16_t begin;
    uint16_t end;
    uint16_t index; // Word slot index or opening bit index.
    uint16_t kind;
} yafth_selection;

YAFTH_STATIC_ASSERT(sizeof(yafth_selection) == 8, selection_must_be_8_bytes);

// The opener is before the word. Live words block the span. Removed words
// block only a closer within their dots.
static inline int yafth_word_blocks_close(const uint8_t *removed_words, uint32_t slot_index, uint32_t word_begin, uint32_t word_length,
                                          uint32_t closer_position) {
    return closer_position >= word_begin && (closer_position < word_begin + word_length || !yafth_test_bit(removed_words, slot_index));
}

static yafth_selection yafth_select_at(const yafth_state *state, uint32_t position) {
    const yafth_selection single_symbol = {(uint16_t)position, (uint16_t)(position + 1u), 0, YAFTH_SELECT_SYMBOL};
    const uint32_t slot_index = position / state->slot_width;
    const uint32_t word_begin = yafth_word_begin(state, slot_index);
    const uint32_t word_length = yafth_word_length(state);
    const uint32_t word_end = word_begin + word_length;
    if (position >= word_begin && position < word_end) {
        const yafth_selection word = {(uint16_t)word_begin, (uint16_t)word_end, (uint16_t)slot_index, YAFTH_SELECT_WORD};
        return yafth_test_bit(state->removed_words, slot_index) ? single_symbol : word;
    }

    // Regenerate one row. The pair index selects the opener's used bit.
    const uint32_t row_index = position / YAFTH_ROW_WIDTH;
    const uint32_t column = position % YAFTH_ROW_WIDTH;
    const uint32_t pair_bits = yafth_row_brackets(state->field_seed + (row_index + 1u) * yafth_row_key_step);
    const uint32_t first_pair = pair_bits & UINT32_C(0xffff);
    const uint32_t pair_index = first_pair != 0 && yafth_bracket_opener_column(first_pair) == column ? 0u : 1u;
    const uint32_t opening_pair = pair_index == 0 ? first_pair : pair_bits >> 16u;
    const uint32_t opening_bit_index = 2u * row_index + pair_index;
    if (opening_pair == 0 || yafth_bracket_opener_column(opening_pair) != column || yafth_test_bit(state->used_openings, opening_bit_index)) {
        return single_symbol;
    }

    // Slots are at least a row wide, so only this slot and the next can block a closer.
    const uint32_t row_begin = row_index * YAFTH_ROW_WIDTH;
    const uint32_t next_word_begin =
        (slot_index + 1u) * state->slot_width < row_begin + YAFTH_ROW_WIDTH ? yafth_word_begin(state, slot_index + 1u) : (uint32_t)YAFTH_BUFFER_SIZE;
    uint32_t nearest_close = YAFTH_BUFFER_SIZE;

    // Either pair can supply the nearest matching closer.
    for (uint32_t candidate_bits = pair_bits; candidate_bits != 0; candidate_bits >>= 16u) {
        const uint32_t candidate_pair = candidate_bits & UINT32_C(0xffff);
        const uint32_t closer_position = row_begin + yafth_bracket_closer_column(candidate_pair);
        if (candidate_pair == 0 || yafth_bracket_type(candidate_pair) != yafth_bracket_type(opening_pair) || closer_position <= position ||
            closer_position >= nearest_close) {
            continue;
        }
        if (position < word_begin && yafth_word_blocks_close(state->removed_words, slot_index, word_begin, word_length, closer_position)) {
            continue;
        }
        if (yafth_word_blocks_close(state->removed_words, slot_index + 1u, next_word_begin, word_length, closer_position)) {
            continue;
        }
        nearest_close = closer_position;
    }
    const yafth_selection bracket = {(uint16_t)position, (uint16_t)(nearest_close + 1u), (uint16_t)opening_bit_index, YAFTH_SELECT_BRACKET};
    return nearest_close == YAFTH_BUFFER_SIZE ? single_symbol : bracket;
}

YAFTH_DEF yafth_range yafth_look_at(const yafth_state *state, uint32_t position) {
    YAFTH_ASSERT(position < YAFTH_BUFFER_SIZE);
    const yafth_selection selection = yafth_select_at(state, position);
    const yafth_range range = {selection.begin, selection.end};
    return range;
}

YAFTH_DEF uint32_t yafth_attempts_left(const yafth_state *state) {
    return state->attempts_and_flags & (unsigned)YAFTH_ATTEMPT_MASK;
}

YAFTH_DEF yafth_game_status yafth_status(const yafth_state *state) {
    if ((state->attempts_and_flags & (unsigned)YAFTH_COMPLETED_FLAG) == 0) {
        return YAFTH_ACTIVE;
    }
    return yafth_attempts_left(state) == 0 ? YAFTH_LOCKED : YAFTH_WON;
}

// Count bits within each byte, then sum the four counts.
static inline uint32_t yafth_popcount(uint32_t bits) {
    bits -= (bits >> 1u) & UINT32_C(0x55555555);
    bits = (bits & UINT32_C(0x33333333)) + ((bits >> 2u) & UINT32_C(0x33333333));
    bits = (bits + (bits >> 4u)) & UINT32_C(0x0f0f0f0f);
    return (bits * UINT32_C(0x01010101)) >> 24u;
}

YAFTH_DEF yafth_click_result yafth_click_at(yafth_state *state, uint32_t position) {
    YAFTH_ASSERT(position < YAFTH_BUFFER_SIZE);
    if ((state->attempts_and_flags & (unsigned)YAFTH_COMPLETED_FLAG) != 0) {
        const yafth_click_result result = {YAFTH_IGNORED, {position, position + 1u}, 0, 0};
        return result;
    }

    const yafth_selection selection = yafth_select_at(state, position);
    const yafth_range selected_range = {selection.begin, selection.end};
    if (selection.kind == YAFTH_SELECT_WORD) {
        if (selection.index == state->answer_slot) {
            state->attempts_and_flags |= (unsigned)YAFTH_COMPLETED_FLAG;
            const yafth_click_result result = {YAFTH_EXACT_MATCH, selected_range, 0, 0};
            return result;
        }
        // Active games have attempts left, so the decrement leaves the flags intact.
        --state->attempts_and_flags;
        const uint32_t matching_letter_count = (uint32_t)state->word_entries[selection.index] >> (unsigned)YAFTH_LIKENESS_SHIFT;
        const uint32_t word_length = yafth_word_length(state);
        if (yafth_attempts_left(state) == 0) {
            state->attempts_and_flags |= (unsigned)YAFTH_COMPLETED_FLAG;
            const yafth_click_result result = {YAFTH_LOCKOUT, selected_range, matching_letter_count, word_length};
            return result;
        }
        const yafth_click_result result = {YAFTH_ENTRY_DENIED, selected_range, matching_letter_count, word_length};
        return result;
    }
    if (selection.kind == YAFTH_SELECT_SYMBOL) {
        const yafth_click_result result = {YAFTH_IGNORED, selected_range, 0, 0};
        return result;
    }

    // Mark the opener as used even when there is no reward.
    yafth_set_bit(state->used_openings, selection.index);
    const uint32_t removed_words_mask = yafth_load_mask24(state->removed_words);
    const uint32_t word_mask = (UINT32_C(1) << yafth_word_count(state)) - 1u;
    const uint32_t answer_mask = UINT32_C(1) << state->answer_slot;
    uint32_t dud_mask = word_mask & ~removed_words_mask & ~answer_mask;
    const int refill_available = (state->attempts_and_flags & (unsigned)YAFTH_REFILLED_FLAG) == 0;
    // With no duds left, a guaranteed refill uses no RNG draw.
    if (refill_available && (dud_mask == 0 || (yafth_xorshift32_next(&state->bracket_rng) & 0x03u) == 0)) {
        state->attempts_and_flags = (unsigned)YAFTH_MAX_ATTEMPTS | (unsigned)YAFTH_REFILLED_FLAG;
        const yafth_click_result result = {YAFTH_ALLOWANCE_REPLENISHED, selected_range, 0, 0};
        return result;
    }
    if (dud_mask == 0) {
        const yafth_click_result result = {YAFTH_IGNORED, selected_range, 0, 0};
        return result;
    }
    // Skip lower-ranked duds by clearing the lowest set bit each time.
    for (uint32_t dud_rank = yafth_xorshift32_next(&state->bracket_rng) % yafth_popcount(dud_mask); dud_rank != 0; --dud_rank) {
        dud_mask &= dud_mask - 1u;
    }
    // The XOR sets bits through the lowest dud bit. Shift it out and count the rest.
    const uint32_t dud_slot = yafth_popcount((dud_mask ^ (dud_mask - 1u)) >> 1u);
    yafth_set_bit(state->removed_words, dud_slot);
    const yafth_click_result result = {YAFTH_DUD_REMOVED, selected_range, 0, 0};
    return result;
}

// Each array has 64 words of one length, in eight chains of eight words.
// Neighbours differ in one to three positions, at most two for four-letter words.
static const char yafth_words_4[] =
    "WANT" "WHAT" "THAT" "THIS" "THEY" "THEM" "THEN" "WHEN"
    "WITH" "WILL" "TELL" "WELL" "WERE" "HERE" "HAVE" "TAKE"
    "DONE" "COME" "MORE" "SOME" "LOVE" "LIKE" "TIME" "HOME"
    "BEST" "JUST" "MUST" "MOST" "LAST" "REST" "LOST" "LOOK"
    "TURN" "SURE" "SAME" "MAKE" "MADE" "CAME" "CARE" "NAME"
    "GOOD" "TOOK" "DOOR" "YOUR" "POOR" "FOOD" "TOLD" "TALK"
    "KNOW" "KNEW" "KEEP" "BEEN" "NEED" "FEEL" "WEEK" "SEEN"
    "HAND" "FIND" "KIND" "MINE" "LIFE" "GIVE" "LINE" "FINE";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_4) == YAFTH_DICTIONARY_ID_COUNT * 4 + 1, dictionary_length_4_must_match);

static const char yafth_words_5[] =
    "THINK" "THERE" "THESE" "THANK" "THEIR" "THING" "THOSE" "WHERE"
    "SOUND" "WOULD" "COULD" "WORLD" "FOUND" "DOING" "GOING" "BEING"
    "RIGHT" "FIRST" "FIGHT" "MIGHT" "NIGHT" "SIGHS" "SINCE" "PLACE"
    "YEARS" "READY" "HEART" "LEAVE" "MEANT" "START" "HEARD" "MEANS"
    "AFTER" "NEVER" "UNDER" "OTHER" "ORDER" "LATER" "WATER" "WATCH"
    "WHOLE" "WHICH" "WHILE" "STILL" "CHILD" "SHALL" "SMALL" "STOLE"
    "MONTH" "MONEY" "TODAY" "WORRY" "HONEY" "SORRY" "EVERY" "HURRY"
    "LYING" "WRONG" "BRING" "ALONG" "ABOUT" "ALONE" "FRONT" "DRINK";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_5) == YAFTH_DICTIONARY_ID_COUNT * 5 + 1, dictionary_length_5_must_match);

static const char yafth_words_6[] =
    "MASTER" "BETTER" "FATHER" "MATTER" "MOTHER" "EITHER" "SISTER" "LISTEN"
    "ENOUGH" "THOUGH" "THINGS" "THANKS" "CHANCE" "CHANGE" "CHARGE" "COURSE"
    "CRYING" "COMING" "TRYING" "HAVING" "ASKING" "MAKING" "SAYING" "TAKING"
    "PRISON" "PERSON" "REASON" "REALLY" "DEEPLY" "PEOPLE" "SIMPLE" "LITTLE"
    "WALKED" "CALLED" "WANTED" "WORKED" "TALKED" "KILLED" "KILLER" "DINNER"
    "BELONG" "BEFORE" "RECORD" "SECOND" "BECOME" "BEYOND" "ANYONE" "ANYWAY"
    "FORGET" "FLIGHT" "FORGOT" "BOUGHT" "FOUGHT" "LAUGHS" "CAUGHT" "CANNOT"
    "NOBODY" "BLOODY" "SLOWLY" "SHOULD" "AROUND" "FRIEND" "GROUND" "GROANS";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_6) == YAFTH_DICTIONARY_ID_COUNT * 6 + 1, dictionary_length_6_must_match);

static const char yafth_words_7[] =
    "MARRIED" "WORRIED" "WORKING" "LOOKING" "NOTHING" "MORNING" "EVENING" "MEETING"
    "CLOTHES" "ANOTHER" "BROTHER" "WHETHER" "WEATHER" "NEITHER" "FURTHER" "FOREVER"
    "WALKING" "TALKING" "DARLING" "TELLING" "CALLING" "WAITING" "WEDDING" "GETTING"
    "CONTACT" "CONTROL" "CONTEST" "CORRECT" "PERFECT" "PROJECT" "PROTECT" "PRESENT"
    "INVITED" "EXCITED" "DECIDED" "BESIDES" "BECOMES" "BELONGS" "SECONDS" "RECORDS"
    "TROUBLE" "PROFILE" "PROMISE" "PROCESS" "PROPOSE" "SUPPOSE" "SUPPORT" "SUSPECT"
    "WRITTEN" "BRITAIN" "CERTAIN" "CAPTAIN" "EXPLAIN" "VILLAIN" "VILLAGE" "COLLEGE"
    "BREATHE" "GREATER" "CREATED" "DRESSED" "CROSSED" "DROPPED" "STOPPED" "STARTED";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_7) == YAFTH_DICTIONARY_ID_COUNT * 7 + 1, dictionary_length_7_must_match);

static const char yafth_words_8[] =
    "WATCHING" "LAUGHING" "LAUGHTER" "DAUGHTER" "GANGSTER" "MINISTER" "FINISHED" "VANISHED"
    "POSSIBLE" "HORRIBLE" "TERRIBLE" "TERRIBLY" "TERRIFIC" "HORRIFIC" "HORRIBLY" "POSSIBLY"
    "PLEASURE" "PRESSURE" "CREATURE" "CREATING" "SPEAKING" "BREAKING" "BRINGING" "THINKING"
    "MATERIAL" "IMPERIAL" "INTERNAL" "INTEREST" "INTERNET" "INTENDED" "INVENTED" "INVOLVED"
    "DISTRICT" "DISTANCE" "DISGRACE" "DISTRACT" "CONTRACT" "CONTRARY" "CONTRAST" "CONGRESS"
    "PROBABLY" "PROPERLY" "PROPERTY" "PROJECTS" "SUBJECTS" "SUSPECTS" "SUPPORTS" "SUPPOSED"
    "STRANGER" "STRAIGHT" "STRANGLE" "STRONGER" "STRENGTH" "STRONGLY" "STRICTLY" "DIRECTLY"
    "REMEMBER" "NOVEMBER" "DECEMBER" "DECEASED" "RECEIVED" "REALIZED" "REALISED" "RELEASED";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_8) == YAFTH_DICTIONARY_ID_COUNT * 8 + 1, dictionary_length_8_must_match);

static const char yafth_words_9[] =
    "WONDERFUL" "WONDERING" "BOTHERING" "GATHERING" "WANDERING" "MURDERING" "MURMURING" "MUTTERING"
    "PERFECTLY" "CORRECTLY" "CORRECTED" "CONNECTED" "CONVICTED" "COMMITTED" "COMMITTEE" "COMMENTED"
    "EDUCATION" "EXECUTION" "EXECUTIVE" "EXCLUSIVE" "EXPENSIVE" "OFFENSIVE" "EFFECTIVE" "DETECTIVE"
    "STATEMENT" "SENTIMENT" "CONTINENT" "CONTINUES" "CONVINCED" "CONTINUED" "CONFIRMED" "CONCERNED"
    "LAXATIVES" "RELATIVES" "RELATIONS" "RELIGIOUS" "DELICIOUS" "DELUSIONS" "DECISIONS" "DELIRIOUS"
    "DIRECTION" "INFECTION" "INTENTION" "ATTENTION" "AFFECTION" "OBJECTION" "INJECTION" "INVENTION"
    "SURPRISED" "SURPRISES" "SURPASSED" "SURPASSES" "SUCCESSES" "SUCCESSOR" "PROCESSOR" "PROFESSOR"
    "NATURALLY" "LITERALLY" "GENERALLY" "GENERATOR" "GENERATES" "DELEGATES" "DELIGHTED" "DELIVERED";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_9) == YAFTH_DICTIONARY_ID_COUNT * 9 + 1, dictionary_length_9_must_match);

static const char yafth_words_10[] =
    "PRETENDING" "PROTECTING" "PROTECTION" "PROFESSION" "POSSESSION" "CONFESSION" "CONNECTION" "CONVENTION"
    "ABSOLUTELY" "ABSOLUTION" "REVOLUTION" "DEMOLITION" "DEFINITION" "DEFINITELY" "INFINITELY" "INTIMATELY"
    "CLATTERING" "FLATTERING" "CHATTERING" "SPUTTERING" "FLUTTERING" "SHATTERING" "STUTTERING" "STAMMERING"
    "CONFIDENCE" "CONSCIENCE" "CONFERENCE" "DIFFERENCE" "PREFERENCE" "PREFERABLE" "PROFITABLE" "PROFITABLY"
    "INDUSTRIAL" "INDUSTRIES" "INJUSTICES" "INTESTINES" "INTENTIONS" "INFECTIONS" "DIRECTIONS" "INJECTIONS"
    "IMPRESSIVE" "IMPRESSION" "DEPRESSION" "OPPRESSION" "EXPRESSION" "REPRESSION" "AGGRESSION" "AGGRESSIVE"
    "UNEXPECTED" "UNDETECTED" "UNDEFEATED" "UNDERWATER" "UNDERCOVER" "UNDERTAKER" "UNDERRATED" "INTEGRATED"
    "REASONABLE" "REASONABLY" "PERSONALLY" "TERMINALLY" "TERMINATED" "ELIMINATED" "ORIGINATED" "ORIGINALLY";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_10) == YAFTH_DICTIONARY_ID_COUNT * 10 + 1, dictionary_length_10_must_match);

static const char yafth_words_11[] =
    "PREMONITION" "PROPOSITION" "COMPOSITION" "CORPORATION" "COOPERATION" "COMPETITION" "COMPETITORS" "COMPETITIVE"
    "INTERESTING" "INTERFERING" "INTERVENING" "INTERACTING" "INTERACTION" "INSTRUCTION" "DESTRUCTION" "DESTINATION"
    "TRANSFERRED" "TRANSFORMED" "TRANSPORTED" "TRANSMITTED" "TRANSMITTER" "TRANSPORTER" "TRANSPONDER" "TRANSFORMER"
    "CONCLUSIONS" "CONCESSIONS" "CONNECTIONS" "CONFESSIONS" "POSSESSIONS" "PROFESSIONS" "PROJECTIONS" "PRODUCTIONS"
    "INSTITUTION" "INSPIRATION" "DESPERATION" "DECLARATION" "DESECRATION" "CELEBRATION" "CELEBRATING" "CELEBRITIES"
    "DISTRIBUTED" "DISTRIBUTOR" "CONTRIBUTOR" "CONTRIBUTED" "CONTRIBUTES" "CONSTITUTES" "SUBSTITUTES" "SUBSTITUTED"
    "TENTATIVELY" "SENSITIVELY" "SENSITIVITY" "SENSIBILITY" "POSSIBILITY" "FEASIBILITY" "FLEXIBILITY" "CREDIBILITY"
    "EXCLUSIVITY" "EXCLUSIVELY" "EXTENSIVELY" "EXCESSIVELY" "OBSESSIVELY" "OBJECTIVELY" "OBJECTIVITY" "OBJECTIFIED";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_11) == YAFTH_DICTIONARY_ID_COUNT * 11 + 1, dictionary_length_11_must_match);

static const char yafth_words_12[] =
    "DISTRIBUTION" "CONTRIBUTION" "CONSTITUTION" "CONSTRUCTION" "CONSTRUCTIVE" "CONSERVATIVE" "CONSERVATION" "CONVERSATION"
    "RESURRECTION" "INSURRECTION" "INTERSECTION" "INTERVENTION" "INTERRUPTION" "INTERRUPTING" "INTERPRETING" "INTERVIEWING"
    "TRANSFORMING" "TRANSFERRING" "TRANSPORTING" "TRANSMITTING" "TRANSMITTERS" "TRANSPORTERS" "TRANSFORMERS" "TRANSPONDERS"
    "CONTRACTIONS" "DISTRACTIONS" "INSTRUCTIONS" "INSTITUTIONS" "INSPIRATIONS" "INCLINATIONS" "IMPLICATIONS" "APPLICATIONS"
    "ANTICIPATION" "INTIMIDATION" "INTOXICATION" "INTOXICATING" "INTIMIDATING" "ANTICIPATING" "ANNIHILATING" "ANNIHILATION"
    "CARELESSNESS" "HOMELESSNESS" "HOPELESSNESS" "HELPLESSNESS" "RECKLESSNESS" "RESTLESSNESS" "FEARLESSNESS" "SELFLESSNESS"
    "NEUROLOGICAL" "ASTROLOGICAL" "MYTHOLOGICAL" "PATHOLOGICAL" "RADIOLOGICAL" "SOCIOLOGICAL" "COSMOLOGICAL" "COSMOLOGISTS"
    "PREPARATIONS" "PREPOSITIONS" "PREMONITIONS" "PROPOSITIONS" "COMPOSITIONS" "CORPORATIONS" "COMPUTATIONS" "COMPETITIONS";
YAFTH_STATIC_ASSERT(sizeof(yafth_words_12) == YAFTH_DICTIONARY_ID_COUNT * 12 + 1, dictionary_length_12_must_match);

static const char *const yafth_dictionary_by_length[] = {yafth_words_4, yafth_words_5,  yafth_words_6,  yafth_words_7, yafth_words_8,
                                                         yafth_words_9, yafth_words_10, yafth_words_11, yafth_words_12};
static const char *yafth_dictionary_words(uint32_t word_length) {
    return yafth_dictionary_by_length[word_length - YAFTH_MIN_WORD_LENGTH];
}

#undef YAFTH_STATIC_ASSERT
#endif // YAFTH_IMPLEMENTATION

/*
Changelog

    1.0.0 - First single-header version.

*/

/*
This software is available under 2 licenses -- choose whichever you prefer.

Alternative A - MIT License

Copyright (c) 2026 Gleb Shigin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Alternative B - Public Domain (The Unlicense)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
*/
