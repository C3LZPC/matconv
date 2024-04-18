#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "convolution.h"

void read_input(const std::string& data_file, parameters& p, std::vector<float>& IPs, std::vector<float>& Fs, std::vector<float>& Bs, std::vector<float>& OPs);
void read_reference_output(const std::string& ref_fname, std::vector<float>& OPs);
float validate_result(const std::vector<float>& refOPs, const std::vector<float>& OPs);

int main()
{
    using namespace std;
    using namespace std::chrono;
    using clock = steady_clock;

    std::vector<float> IPs, Fs, Bs, OPs, refOPs;

    parameters p = {0};

    static const float error_threshold = 0.0f;

    read_input("input.dat", p, IPs, Fs, Bs, OPs);
    read_reference_output("reference_output.dat", refOPs);

    // Create the convolution
    convolution* c = convolution_new(&p);

    // The benchmark parameters
    unsigned int N_iter = 1;

    clock::duration total_time = clock::duration::zero();
    for (unsigned int i = 0; i < N_iter; i++) {
        auto start_time = clock::now();

        // run the convolution
        convolution_run(c);

        total_time += (clock::now() - start_time);
        if (i % 50 == 0) {
            // update the result in p.OPs
            convolution_result(c);
            float total_error = validate_result(refOPs, OPs);
            std::cout << "Validated: " << std::boolalpha << (total_error <= error_threshold) << std::endl;
        }
    }
    std::cout << "Running time: " << duration_cast<milliseconds>(total_time).count() / (double)N_iter << " ms" << std::endl;

    // destroy the convolution
    convolution_destroy(c);

    // Returning
    return 0;
}

void read_input(const std::string& data_file, parameters& p, std::vector<float>& IPs, std::vector<float>& Fs, std::vector<float>& Bs, std::vector<float>& OPs)
{
    FILE* f = fopen(data_file.c_str(), "rb");
    if (f == 0) {
        std::cerr << "Cannot open data file: " << data_file << std::endl;
        exit(-1);
    }
    fread((char*)&p.IP_N, sizeof(p.IP_N), 1, f);
    fread((char*)&p.IP_w, sizeof(p.IP_w), 1, f);
    fread((char*)&p.IP_h, sizeof(p.IP_h), 1, f);
    IPs.resize(p.IP_N * p.IP_w * p.IP_h);
    fread((char*)IPs.data(), IPs.size() * sizeof(float), 1, f);
    p.IPs = IPs.data();

    fread((char*)&p.F_N, sizeof(p.F_N), 1, f);
    fread((char*)&p.F_w, sizeof(p.F_w), 1, f);
    fread((char*)&p.F_h, sizeof(p.F_h), 1, f);
    fread((char*)&p.F_d, sizeof(p.F_d), 1, f);
    Fs.resize(p.F_N * p.F_w * p.F_h * p.F_d);
    fread((char*)Fs.data(), Fs.size() * sizeof(float), 1, f);
    p.Fs = Fs.data();

    Bs.resize(p.F_N);
    fread((char*)Bs.data(), Bs.size() * sizeof(float), 1, f);
    p.Bs = Bs.data();

    fread((char*)&p.S_w, sizeof(p.S_w), 1, f);
    fread((char*)&p.S_h, sizeof(p.S_h), 1, f);

    p.OP_N = p.F_N;
    p.OP_w = (p.IP_w - p.F_w) / p.S_w + 1;
    p.OP_h = (p.IP_h - p.F_h) / p.S_h + 1;
    OPs.resize(p.OP_N * p.OP_w * p.OP_h);
    p.OPs = OPs.data();

    fclose(f);
}

void read_reference_output(const std::string& ref_fname, std::vector<float>& OPs)
{
    FILE* f = fopen("reference_output.dat", "rb");

    if (f == 0) {
        std::cerr << "Cannot open reference file: " << ref_fname << std::endl;
        exit(-1);
    }

    unsigned rOP_N, rOP_w, rOP_h;
    fread(&rOP_N, sizeof(rOP_N), 1, f);
    fread(&rOP_w, sizeof(rOP_w), 1, f);
    fread(&rOP_h, sizeof(rOP_h), 1, f);
    OPs.resize(rOP_N * rOP_w * rOP_h);
    fread((char*)OPs.data(), OPs.size() * sizeof(float), 1, f);
    fclose(f);
}

float validate_result(const std::vector<float>& refOPs, const std::vector<float>& OPs)
{
    assert(refOPs.size() == OPs.size() && "Wrong output size!");
    float error = 0;
    for (size_t i = 0; i < refOPs.size(); ++i) {
        error += std::fabs(refOPs[i] - OPs[i]);
    }
    return error;
}
