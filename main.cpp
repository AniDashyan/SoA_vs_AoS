#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "kaizen.h"

size_t NUM_PARTICLES = 1000000;
size_t NUM_ITERATIONS = 1000;

struct ParticleAoS {
    double pos_x, pos_y, pos_z;
    double mom_x, mom_y, mom_z;
    int spin; // -1 or 1
};

struct ParticleSoA {
    std::vector<double> position_x, position_y, position_z;
    std::vector<double> momentum_x, momentum_y, momentum_z;
    std::vector<int> spin;

    ParticleSoA(size_t n) {
        position_x.resize(n); position_y.resize(n); position_z.resize(n);
        momentum_x.resize(n); momentum_y.resize(n); momentum_z.resize(n);
        spin.resize(n);
    }
};

void initializeAoS(std::vector<ParticleAoS>& particles) {
    for (auto& p : particles) {
        p.pos_x = zen::random_int(-10, 10);
        p.pos_y = zen::random_int(-10, 10);
        p.pos_z = zen::random_int(-10, 10);
        p.mom_x = zen::random_int(-5, 5);
        p.mom_y = zen::random_int(-5, 5);
        p.mom_z = zen::random_int(-5, 5);
        p.spin = zen::random_int(0, 1) == 0 ? -1 : 1;
    }
}


void initializeSoA(ParticleSoA& particles, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        particles.position_x[i] = zen::random_int(-10, 10);
        particles.position_y[i] = zen::random_int(-10, 10);
        particles.position_z[i] = zen::random_int(-10, 10);
        particles.momentum_x[i] = zen::random_int(-5, 5);
        particles.momentum_y[i] = zen::random_int(-5, 5);
        particles.momentum_z[i] = zen::random_int(-5, 5);
        particles.spin[i] = zen::random_int(0, 1) == 0 ? -1 : 1;
    }
}

double computePhaseAoS(std::vector<ParticleAoS>& particles, size_t num_iterations) {
    double total_phase = 0.0;
    for (size_t iter = 0; iter < num_iterations; ++iter) {
        total_phase = 0.0;
        for (const auto& p : particles) {
            double phase = p.pos_x * p.mom_x + p.pos_y * p.mom_y + p.pos_z * p.mom_z;
            total_phase += phase * p.spin;
        }
    }
    return (total_phase);
}


double computePhaseSoA(ParticleSoA& particles, size_t n, size_t num_iterations) {
    double total_phase = 0.0;
    for (size_t iter = 0; iter < num_iterations; ++iter) {
        total_phase = 0.0; // Reset for each iteration
        for (size_t i = 0; i < n; ++i) {
            double phase = particles.position_x[i] * particles.momentum_x[i] +
                           particles.position_y[i] * particles.momentum_y[i] +
                           particles.position_z[i] * particles.momentum_z[i];
            total_phase += phase * particles.spin[i];
        }
    }
    return (total_phase);
}

double computeEnergyAoS(const std::vector<ParticleAoS>& particles, size_t num_iterations) {
    double total_energy = 0.0;
    for (size_t iter = 0; iter < num_iterations; ++iter) {
        total_energy = 0.0;
        for (const auto& p : particles) {
            double ke = (p.mom_x * p.mom_x + p.mom_y * p.mom_y + p.mom_z * p.mom_z) / 2.0;
            total_energy += ke;
        }
    }
    return (total_energy);
}


double computeEnergySoA(const ParticleSoA& particles, size_t n, size_t num_iterations) {
    double total_energy = 0.0;
    for (size_t iter = 0; iter < num_iterations; ++iter) {
        total_energy = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double ke = (particles.momentum_x[i] * particles.momentum_x[i] +
                         particles.momentum_y[i] * particles.momentum_y[i] +
                         particles.momentum_z[i] * particles.momentum_z[i]) / 2.0;
            total_energy += ke;
        }
    }
    return (total_energy);
}

int main(int argc, char** argv) {
    zen::cmd_args args(argv, argc);
    auto num_opts = args.get_options("--n");
    auto iter_opts = args.get_options("--i");

    if (num_opts.empty() || iter_opts.empty()) {
        zen::log(zen::color::yellow("None of the arguments were provided. Using default values. particles: " + std::to_string(NUM_PARTICLES) + " iterations: " +  std::to_string(NUM_ITERATIONS)));
    }

    NUM_PARTICLES = num_opts.empty() ? NUM_PARTICLES : std::stoul(num_opts[0]);
    NUM_ITERATIONS = iter_opts.empty() ? NUM_ITERATIONS : std::stoul(iter_opts[0]);

    std::cout << "Simulating " << NUM_PARTICLES << " particles over " << NUM_ITERATIONS << " iterations\n";

    std::vector<ParticleAoS> aos_particles(NUM_PARTICLES);
    ParticleSoA soa_particles(NUM_PARTICLES);
    initializeAoS(aos_particles);
    initializeSoA(soa_particles, NUM_PARTICLES);

    // Memory usage
    size_t aos_memory = NUM_PARTICLES * sizeof(ParticleAoS);
    size_t soa_memory = NUM_PARTICLES * (6 * sizeof(double) + sizeof(int)) + 7 * sizeof(size_t);

    zen::timer t;
    double aos_phase, soa_phase, aos_energy, soa_energy;

    t.start();
    aos_phase = computePhaseAoS(aos_particles, NUM_ITERATIONS);
    t.stop();
    double aos_phase_time = t.duration<zen::timer::msec>().count() / static_cast<double>(NUM_ITERATIONS);
    
    t.start();
    aos_energy = computeEnergyAoS(aos_particles, NUM_ITERATIONS);
    t.stop();
    double aos_energy_time = t.duration<zen::timer::msec>().count() / static_cast<double>(NUM_ITERATIONS);

    t.start();
    soa_phase = computePhaseSoA(soa_particles, NUM_PARTICLES, NUM_ITERATIONS);
    t.stop();
    double soa_phase_time = t.duration<zen::timer::msec>().count() / static_cast<double>(NUM_ITERATIONS);
    
    t.start();
    soa_energy = computeEnergySoA(soa_particles, NUM_PARTICLES, NUM_ITERATIONS);
    t.stop();
    double soa_energy_time = t.duration<zen::timer::msec>().count() / static_cast<double>(NUM_ITERATIONS);

    // Total time
    double aos_total_time = aos_phase_time + aos_energy_time;
    double soa_total_time = soa_phase_time + soa_energy_time;

    std::cout << "\nResults Summary:\n";
    std::cout << std::left << std::setw(10) << "Layout "
              << std::setw(20) << "Phase Time (ms) "
              << std::setw(20) << "Energy Time (ms) "
              << std::setw(20) << "Total Phase "
              << std::setw(20) << "Total Energy "
              << std::setw(20) << "Memory Usage (MB)" << "\n";
    
    std::cout << std::string(125, '-') << "\n";
    std::cout << std::setw(10) << "AoS"
              << std::fixed << std::setprecision(2)
              << std::setw(20) << aos_phase_time
              << std::setw(20) << aos_energy_time
              << std::setw(20) << aos_phase
              << std::setw(20) << aos_energy
              << std::setw(20) << aos_memory / (1024.0 * 1024.0) << "\n";
    
    std::cout << std::setw(10) << "SoA"
              << std::setw(20) << soa_phase_time
              << std::setw(20) << soa_energy_time
              << std::setw(20) << soa_phase
              << std::setw(20) << soa_energy
              << std::setw(20) << soa_memory / (1024.0 * 1024.0) << "\n";
    
    // Relative performance differences for easy comparison
    std::cout << "\nRelative Differences (SoS / AoS):\n";
    std::cout << "Phase Time Speedup: " << std::fixed << std::setprecision(2)
              << (aos_phase_time > 0 ? soa_phase_time / aos_phase_time : 0.0) << "x\n";
    std::cout << "Energy Time Speedup: " << (aos_energy_time > 0 ? soa_energy_time / aos_energy_time : 0.0) << "x\n";
    
    std::cout << "\nTotal Memory Usage Comparison:\n";
    std::cout << "AoS Memory: " << aos_memory / (1024.0 * 1024.0) << " MB\n";
    std::cout << "SoA Memory: " << soa_memory / (1024.0 * 1024.0) << " MB\n";
    
    std::cout << "\nTotal Time Difference (AoS - SoA): "
              << soa_total_time - aos_total_time << " ms\n";
    
    return (0);
}