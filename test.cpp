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

void runFact(int times) {
    long res = 0;
    for (int i = 0; i < times; i++) {
        for (int j = 0; j < 10000; j++) {
            res += fact(j);
        }
    }
}

int main(int argc, const char *argv[])
{
    po::options_description desc("options");
    desc.add_options()
        ("nthreads", po::value<int>(), "")
        ("times", po::value<int>(), "")
        ("tasksize", po::value<int>(), "")
        ;

    po::variables_map args;
    po::store(po::parse_command_line(argc, argv, desc), args);
    po::notify(args);


    int nthreads = args["nthreads"].as<int>();
    int times = args["times"].as<int>();
    int tasksize = args["tasksize"].as<int>();

    Threading::setup(nthreads);

    boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::universal_time();
    for (int n = 0; n < times; n++) {
        std::vector<boost::function0<void> > tasks(nthreads);
        for (int i = 0; i < nthreads; i++) {
            tasks[i] = boost::bind(runFact, tasksize / nthreads);
        }
        Threading::getInstance().scheduleTasks(tasks);
    }
    boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::universal_time();

    std::cout << endTime - startTime;

    return 0;
}
