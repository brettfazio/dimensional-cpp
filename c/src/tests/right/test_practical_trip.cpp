#include <vector>
#include <numeric>
#include <iostream>
#include <cmath>

#define M [[clang::annotate("unit:miles")]]
#define KM [[clang::annotate("unit:km")]]
#define H [[clang::annotate("unit:hour")]]
#define MIN [[clang::annotate("unit:minute")]]
#define SEC [[clang::annotate("unit:second")]]
#define MPH [[clang::annotate("unit:miles/hour")]]
#define KPH [[clang::annotate("unit:km/hour")]]

#define EXPLICIT [[clang::annotate("explicit_unit_annotation")]]

struct Miles {
    M double value;
};

struct Hours {
    H double value;
};

Miles as_miles_t(double value) { return (Miles){value}; }

Hours as_hours_t(double value) { return (Hours){value}; }

template <typename T>
MPH T to_mph(M T distance, H T time) { return distance / time; }

template <typename T>
KPH T to_kph(KM T distance, H T time) { return distance / time; }

// Function to calculate average speed over a trip
MPH double calculateAverageSpeed(const std::vector<Miles>& distances, 
                                 const std::vector<Hours>& times) {
    if (distances.size() != times.size() || distances.empty()) {
        throw std::invalid_argument("Distances and times must be non-empty and of the same size.");
    }

    // Compute total distance and time
    M EXPLICIT double totalDistance = 0.0;
    for (const auto& m : distances) {
        totalDistance += m.value;
    }
    H EXPLICIT double totalTime = 0.0;
    for (const auto& t : times) {
        totalTime += t.value;
    }

    // Validate units
    H EXPLICIT double zeroHours = 0.0;
    if (totalTime <= zeroHours) {
        throw std::logic_error("Total time must be positive.");
    }

    // Compute average speed
    MPH double avgSpeed = to_mph(totalDistance, totalTime);

    // Print debug info
    std::cout << "Total Distance: " << totalDistance << " miles\n";
    std::cout << "Total Time: " << totalTime << " hours\n";
    std::cout << "Average Speed: " << avgSpeed << " mph\n";

    return avgSpeed;
}

int main() {
    try {
        // Example trip data
        std::vector<Miles> distances = {as_miles_t(100.0), as_miles_t(50.0), as_miles_t(150.0)};
        std::vector<Hours> times = {as_hours_t(2.0), as_hours_t(1.5), as_hours_t(3.0)};

        MPH double avgSpeed = calculateAverageSpeed(distances, times);

        // Verify result
        std::cout << "Calculated Average Speed: " << avgSpeed << " mph\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
