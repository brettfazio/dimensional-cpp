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

struct Miles {
    M double value;
};

struct Hours {
    H double value;
};

template <typename T>
M T as_miles(T value) { return value; }

Miles as_miles_t(double value) { return (Miles){value}; }

template <typename T>
KM T as_km(T value) { return value; }

template <typename T>
H T as_hours(T value) { return value; }

Hours as_hours_t(double value) { return (Hours){value}; }

template <typename T>
MIN T as_minutes(T value) { return value; }

template <typename T>
SEC T as_seconds(T value) { return value; }

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
    M double totalDistance = as_miles(0.0);
    for (const auto& m : distances) {
        totalDistance += m.value;
    }
    H double totalTime = as_hours(0);
    for (const auto& t : times) {
        totalTime += t.value;
    }

    // Validate units
    if (totalTime <= as_hours(0.0)) {
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
