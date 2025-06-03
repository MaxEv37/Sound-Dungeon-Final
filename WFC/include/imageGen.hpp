#pragma once
#include <string>
#ifndef IMAGE_GEN_H
#define IMAGE_GEN_H

#ifdef _WIN32
#ifdef BUILD_IMAGE_GEN_LIB
#define IMAGE_GEN_API __declspec(dllexport)
#else
#define IMAGE_GEN_API __declspec(dllimport)
#endif
#else
#define IMAGE_GEN_API
#endif

IMAGE_GEN_API void genImage(std::string image_path, std::string imageName);

#endif // IMAGE_GEN_H

#include <iostream>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_set>

#include "time.h"

#include "../include/utils.hpp"
#include "../include/image.hpp"
#include "../include/external/rapidxml.hpp"
#include "../include/rapidxml_utils.hpp"

#include "../src/wave.hpp"
#include "../src/utils/array_3d.hpp"
#include "../src/overlapping_wfc.hpp"



using namespace rapidxml;
using namespace std;

/**
 * Get a random seed.
 * This function use random_device on linux, but use the C rand function for
 * other targets. This is because, for instance, windows don't implement
 * random_device non-deterministically.
 */
int get_random_seed() {
#ifdef __linux__
    return random_device()();
#else
    return rand();
#endif
}

Wave::Heuristic to_heuristic(const string& heuristic_string) {
#define CASE(enum) \
    if (heuristic_string == #enum) return Wave::Heuristic::enum

    CASE(Scanline);
    CASE(Entropy);
    CASE(MRV);

    throw runtime_error("Invalid Heuristic: " + heuristic_string);
#undef CASE
}

void genImage(string image_path, string imageName) {
    // Initialize rand for non-linux targets
#ifndef __linux__
    srand(time(nullptr));
#endif

    auto m = read_image(image_path);

    if (!m.has_value()) {
        throw runtime_error("Error while loading " + image_path);
    }

    auto& img = m.value();

    OverlappingWFC::Options options = {
        .periodic_input = false,
        .periodic_output = false,
        .i_W = img.MX,
        .i_H = img.MY,
        .o_W = 64,
        .o_H = 64,
        .symmetry = uint32_t((1 << 8) - 1),
        .pattern_size = 3,
        .heuristic = to_heuristic("Entropy"),
        .ground = true,
    };


    using namespace chrono;

    time_point<system_clock> start, end;
    start = system_clock::now();

    //read_config_file("samples.xml");
    OverlappingWFC wfc(options, img);
    for (size_t i = 0; i < 1; i++)
    {
        for (uint32_t test = 0; test < 10; test++) {
            int seed = get_random_seed();
            bool success = wfc.run(seed);

            cout << "> ";
            if (success) {
                cout << "DONE" << endl;
                write_image_png("../Sound_Dungeon/build/results/" + imageName + to_string(i) + ".png",
                    wfc.get_output());
                break;
            }
            else {
                cout << "CONTRADICTION" << endl;
            }
        }
    }

    end = system_clock::now();
    auto elapsed_ms = duration_cast<milliseconds>(end - start).count();
    cout << "Completed in " << elapsed_ms << "ms.\n";
}