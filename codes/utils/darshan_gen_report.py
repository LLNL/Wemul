import os
from glob import glob
from pprint import pprint
import re
import subprocess
import sys


def run_darshan_util(log_file_name, output_file_name, output_pdf_file):
    command_args = ["darshan/darshan-job-summary-read-write.pl", log_file_name, "--verbose", "--output", output_pdf_file]
    popen = subprocess.Popen(command_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, errors = popen.communicate()

    output_file = open(output_file_name, "a")
    output_file.write("=====================================================================" + os.linesep)
    output_file.write(log_file_name + os.linesep)
    output_file.write(str(output_pdf_file))
    output_file.write(str(output))
    output_file.write(str(errors))
    output_file.write("=====================================================================" + os.linesep)


def copy_detail_files(output_path, output_file_name):
    output_file = open(output_file_name, "r")
    line = output_file.readline()
    line_no = 1
    dir_num = 1
    while line:
        verbose_regex = re.compile(r'.*verbose:.*')
        could_extract = False

        if verbose_regex.search(line):
            detail_path = line.split('verbose:')[1].strip()
            command_args = ["cp", "-r", detail_path, output_path + "/" + str(dir_num)]
            popen = subprocess.Popen(command_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            dir_num += 1

        line = output_file.readline()
        line_no += 1


if __name__ == "__main__":
    log_path = sys.argv[1]
    output_path = sys.argv[2]

    darshan_file_list = [y for x in os.walk(log_path) for y in glob(os.path.join(x[0], '*.darshan'))]

    file_count = 1
    for darshan_file in darshan_file_list:
        run_darshan_util(darshan_file, output_path + "/" + "out.txt", \
            output_path + "/" + str(file_count) + ".pdf")

        file_count += 1

    copy_detail_files(output_path, output_path + "/" + "out.txt")
