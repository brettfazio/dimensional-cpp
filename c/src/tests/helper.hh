#define EXPLICIT [[clang::annotate("explicit_unit_annotation")]]

#define M [[clang::annotate("unit:miles")]]
#define KM [[clang::annotate("unit:km")]]
#define H [[clang::annotate("unit:hour")]] 

struct Miles {
    M int value;
};

M EXPLICIT int miles50 = 50;
H EXPLICIT int hours2 = 2;
KM EXPLICIT int km25 = 25;