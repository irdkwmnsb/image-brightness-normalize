#!/usr/bin/env python3
from subprocess import check_output, check_call
import os
import sys

SCHEDULE_TYPES = ["static", "dynamic", "guided"]
SCHEDULE_SIZES = [str(2 ** i) for i in range(10)]
TEST_SCHEDULE_TYPES = SCHEDULE_TYPES + [type + "," + size for type in SCHEDULE_TYPES for size in SCHEDULE_SIZES] + [
    "auto"]
NUM_TESTS = 10

IMIN = "im/in2.ppm"
IMOUT = "im/out2.ppm"


def compile(ofast, compile_schedule_type=None, openmp=True):
    if compile_schedule_type is None:
        compile_schedule_type = "runtime"
    orig_f = open("normalize.h.templ", "r")
    orig = orig_f.read()
    orig_f.close()
    modif = orig.replace("%%ARGS%%", compile_schedule_type)
    modif_f = open("normalize.h", "w")
    modif_f.write(modif)
    modif_f.close()
    args = ["clang", "main.c", "-o", "out/main.exe"]
    if openmp:
        args.append("-fopenmp")
    if ofast:
        args.append("-Ofast")
    check_call(args)


def test_schedule_type(imin, imout):
    cores_n = 8
    compile(False)
    print("Using {} cores".format(cores_n))
    print("\ttest_res")
    for schedule in TEST_SCHEDULE_TYPES:
        for test_n in range(NUM_TESTS):
            env = os.environ.copy()
            env.update({"OMP_SCHEDULE": schedule, "OMP_NUM_THREADS": str(cores_n)})
            res = check_output(["out/main.exe", imin, imout], env=env).decode().strip()
            print(schedule + "\t" + res, flush=True)


def test_schedule_type_compile(imin, imout):
    cores_n = 8
    print("Using {} cores".format(cores_n))
    print("\ttest_res")
    for schedule in TEST_SCHEDULE_TYPES:
        compile(False, schedule)
        # call(["clang", "-fopenmp", "main.c", "-Ofast", "-o", "out/main.exe"])
        for test_n in range(NUM_TESTS):
            res = check_output(["out/main.exe", imin, imout]).decode().strip()
            print(schedule + "\t" + res, flush=True)


def test_threads_num(imin, imout, runtime=False, ofast=False):
    print("\ttest_res")
    schedule = "guided"
    compile(ofast, [schedule, "runtime"][int(runtime)])
    for cores_n in range(1, 12):
        for test_n in range(NUM_TESTS):
            env = os.environ.copy()
            if runtime:
                env.update({"OMP_SCHEDULE": schedule})
            env.update({"OMP_NUM_THREADS": str(cores_n)})
            res = check_output(["out/main.exe", imin, imout], env=env).decode().strip()
            print(str(cores_n) + "\t" + res, flush=True)


def test_single_thread_with_and_without_openmp(imin, imout):
    print("\ttest_res")
    compile(False, "guided", True)
    for test_n in range(NUM_TESTS):
        env = os.environ.copy()
        env.update({"OMP_NUM_THREADS": "1"})
        res = check_output(["out/main.exe", imin, imout], env=env).decode().strip()
        print("with" + "\t" + res, flush=True)
    compile(False, "guided", False)
    for test_n in range(NUM_TESTS):
        res = check_output(["out/main.exe", imin, imout]).decode().strip()
        print("without" + "\t" + res, flush=True)


def main():
    # test_threads_num(IMIN, IMOUT, True, True)
    test_single_thread_with_and_without_openmp(IMIN, IMOUT)


# test_schedule_type(sys.argv[1:])
# test_schedule_type_compile(sys.argv[1:])


if __name__ == "__main__":
    main()
