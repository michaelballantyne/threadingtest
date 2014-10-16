#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/function.hpp>
#include "Threading.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int fact(int val) {
    if (val == 0) {
        return 1;
    } else {
        return val * fact(val-1);
    }
}

void runFact(long times) {
    long res = 0;
    for (long i = 0; i < times; i++) {
        for (long j = 0; j < 100; j++) {
            res += fact(j);
        }
    }
    __asm__ __volatile__("" :: "m" (res));
}

int main(int argc, const char *argv[])
{
    po::options_description desc("options");
    desc.add_options()
        ("nthreads", po::value<long>(), "")
        ("times", po::value<long>(), "")
        ("tasksize", po::value<long>(), "")
        ;

    po::variables_map args;
    po::store(po::parse_command_line(argc, argv, desc), args);
    po::notify(args);


    long nthreads = args["nthreads"].as<long>();
    long times = args["times"].as<long>();
    long tasksize = args["tasksize"].as<long>();

    Threading::setup(nthreads);

    boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::universal_time();
    for (long n = 0; n < times; n++) {
        std::vector<boost::function0<void> > tasks(nthreads);
        for (long i = 0; i < nthreads; i++) {
            tasks[i] = boost::bind(runFact, tasksize / nthreads);
        }
        Threading::getInstance().scheduleTasks(tasks);
    }
    boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::universal_time();

    std::cout << (endTime - startTime).total_milliseconds() << std::endl;

    return 0;
}
