//
//  main.cpp
//  LW3(Mikozi)
//
//  Created by Никита on 3/11/22.
//

#include <iostream>
#include <vector>
#include <functional>
#include <fstream>

class GaluaLFSR {
private:
    int64_t state_;
    size_t size_;
    int64_t mask_;

public:
    GaluaLFSR(int32_t state, size_t size, int32_t polynomial): state_{ state }, size_{ size }, mask_{ polynomial } {}
    bool Next() {
        if (state_ & 1) {
            state_ = ((state_ ^ mask_) >> 1) | (1 << (size_ - 1));
            return 1;
        }
        state_ >>= 1;
        return 0;
    }
};

class GeffeGenerator {
private:
    std::vector <GaluaLFSR> registers_;
    std::function <bool(std::vector<bool>)> func_;

public:
    GeffeGenerator(std::function<bool(std::vector<bool>)> func, const std::vector<GaluaLFSR>& gs)
        : func_{ func }, registers_{ gs } {}

    bool next() {
        std::vector <bool> reg_outs(registers_.size());
        for (size_t i = 0; i < registers_.size(); ++i) {
            reg_outs[i] = registers_[i].Next();
        }
        return func_(reg_outs);
    }
};

void writeToFile(GeffeGenerator gen, std::string filename, size_t sample_size) {
    if (sample_size % 8) {
        sample_size += 8 - (sample_size % 8);
    }
    std::ofstream fout(filename, std::ios_base::binary);
    char ch;
    for (size_t i = 0; i < sample_size / 8; ++i) {
        ch = 0;
        for (size_t j = 0; j < 7; ++j) {
            bool g = gen.next();
            ch = (ch + g) << 1;
        }
        ch += gen.next();
        fout.write(&ch, 1);
    }
}

std::vector <GaluaLFSR> makeVectorGalua() {
    std::vector<GaluaLFSR> galuaVector;
    galuaVector.emplace_back(0x6152E3, 23, 0x400010);
    galuaVector.emplace_back(0x1E82F0A9, 29, 0x10000002);
    galuaVector.emplace_back(0x7817BECE, 31, 0x40000004);
    return galuaVector;
}

int main() {
    std::vector<GaluaLFSR> galuaVector = makeVectorGalua();
    GeffeGenerator g_gen([](const std::vector<bool>& bits) {
        return (bits[0] & bits[1]) ^ ((bits[0] ^ 1) & bits[2]); }, galuaVector);
    writeToFile(g_gen, "/Users/nikita/Developer/Hometask/Programming/C++/LW3(Mikozi)/generated.bin", 256);
}
