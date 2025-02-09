#include <iostream>
#include <bit>
#include <bitset>
#include <chrono>
#include <string.h>
#include <stdexcept>
#include <fstream>
#include <vector>


int countr(ulong input) {
    int output = std::__countr_zero(input);
    return output;
}


int my_loop(ulong input) {
    int output = 0;
    for (; !((input >> output) & 1); ++output);
    return output;
}


int their_loop(ulong input) {
    int output = 0;
    input = (input ^ (input - 1)) >> 1;  // Set input's trailing 0s to 1s and zero rest
    for (; input; output++) {
        input >>= 1;
    }
    return output;
}


int lookup_37mod(ulong input) {
    static const int lookup[] = {
        32, 0, 1, 26, 2, 23, 27, 0,
        3, 16, 24, 30, 28, 11, 0, 13,
        4, 7, 17, 0, 25, 22, 31, 15,
        29, 10, 12, 6, 0, 21, 14, 9,
        5, 20, 8, 19, 18
    };

    int output = 0;
    ulong diff = -input & input;
    if (diff < 0xffffffffULL) output = lookup[diff % 37];
    else output = lookup[(diff >> 32) % 37] + 32;

    return output;
}


int parallel(ulong input) {
    unsigned int output = 64;
    input = input & ~(input - 1);
    // std::bitset<64> bits(input);
    // std::cout << bits << std::endl;
    if (input) output--;
    if (input & 0x00000000FFFFFFFFULL) output -= 32;
    if (input & 0x0000FFFF0000FFFFULL) output -= 16;
    if (input & 0x00FF00FF00FF00FFULL) output -= 8;
    if (input & 0x0F0F0F0F0F0F0F0FULL) output -= 4;
    if (input & 0x3333333333333333ULL) output -= 2;
    if (input & 0x5555555555555555ULL) output -= 1;
    return output;
}


int bsearch(ulong input) {
    unsigned int output;

    if (input & 0x1) {
        output = 0;

    } else {
        output = 1;
        if ((input & 0xffffffff) == 0) {  
            input >>= 32;  
            output += 32;
        }
        if ((input & 0xffff) == 0) {  
            input >>= 16;  
            output += 16;
        }
        if ((input & 0xff) == 0) {  
            input >>= 8;  
            output += 8;
        }
        if ((input & 0xf) == 0) {
            input >>= 4;
            output += 4;
        }
        if ((input & 0x3) == 0) {  
            input >>= 2;
            output += 2;
        }
        output -= input & 0x1;
    }
    return output;
}


class FunctionTimer {
    public:
        int (*fptr)(ulong);
        const char *name;
    
    private:
        void test_range(ulong min, ulong max, ulong step) {
            for (ulong i = min; i < max; i += step) {
                int output = fptr(i);
                int expected = std::__countr_zero(i);
                if (output != expected) {
                    char errstr[200];
                    sprintf(
                        errstr,
                        "Error: %s provides invalid input \"%d\" instead of \"%d\" for value %lu.",
                        name, output, expected, i
                    );
                    throw std::runtime_error(errstr);
                }
            }
        }


        void test() {
            test_range(1ULL, 1000ULL, 1ULL);
            for (int i = 1; i < 64; ++i) {
                test_range((1ULL << i), (1ULL << i) + 1ULL, 1ULL);
            }
        }


        double nano_to_milli(ulong nano) {
            char nanoStr[32];
            sprintf(nanoStr, "%lu", nano);
            int len = strlen(nanoStr);
            nanoStr[len + 1] = '\0';
            for (int i = len - 1; i >= len - 6; --i) {
                nanoStr[i + 1] = nanoStr[i];
            }
            nanoStr[len - 6] = '.';
            double milli = 0.;
            sscanf(nanoStr, "%lf", &milli);

            return milli;
        }


    public:
        FunctionTimer(int (*fptr)(ulong), const char *name) {
            this->fptr = fptr;
            this->name = name;
            test();
        }

        double get_discrete_time(ulong input, int repeat) {
            // get time of loop
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            for (int i = 1; i <= repeat; ++i) {
                ulong output = this->fptr(input);
            }

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            ulong nano = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

            return nano_to_milli(nano);
        }


        double get_range_time(ulong min, ulong max) {
            // get time of loop
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            for (ulong i = min; i < max; ++i) {
                ulong output = this->fptr(i);
            }

            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            ulong nano = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

            return nano_to_milli(nano);
        }
};


class FunctionTester {
    std::vector<FunctionTimer> funcs;
    

    void test_discrete() {
        std::ofstream fout(("discrete.txt"));
        if (!fout.is_open()) {
            throw new std::runtime_error("Failed to open discrete.txt");
        }

        fout << "name";
        for (int i = 0; i < 64; ++i) {
            fout << "," << i;
        }
        fout << std::endl;


        for (FunctionTimer func : funcs) {
            fout << func.name << std::flush;

            for (int i = 0; i < 64; ++i) {
                double total_milli = 0.;
                for (int j = 0; j < 3; ++j) {
                    total_milli += func.get_discrete_time((1 << i), 5000000);
                }
                fout << "," << total_milli / 3. << std::flush;
            }

            fout << std::endl;
        }
    }


    void test_range() {
        std::ofstream fout(("range.txt"));
        if (!fout.is_open()) {
            throw new std::runtime_error("Failed to open range.txt");
        }

        fout << "name,time" << std::endl;

        for (FunctionTimer func : funcs) {
            fout << func.name << std::flush;

            double total_milli = 0.;
            for (int j = 0; j < 3; ++j) {
                total_milli += func.get_range_time(1ULL, 500000000ULL);
            }
            fout << "," << total_milli / 3. << std::flush;

            fout << std::endl;
        }
    }


    public:
        FunctionTester() {
        }


        void add_func(FunctionTimer &timer) {
            funcs.push_back(timer);
        }


        void test_funcs() {
            test_discrete();
            test_range();
        }
};


int main() {
    // Create tester and timers
    FunctionTester tester = FunctionTester();
    
    FunctionTimer funcs[6] = {
        FunctionTimer(&my_loop, "my_loop"),
        FunctionTimer(&their_loop, "their_loop"),
        FunctionTimer(&countr, "countr"),
        FunctionTimer(&lookup_37mod, "lookup_37mod"),
        FunctionTimer(&parallel, "parallel"),
        FunctionTimer(&bsearch, "bsearch")
    };

    // Add timers to tester
    for (int i = 0; i < 6; ++i) {
        tester.add_func(funcs[i]);
    }

    // Run tester
    tester.test_funcs();
}