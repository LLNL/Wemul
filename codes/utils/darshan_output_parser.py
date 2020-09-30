from copy import copy
import math
import numpy as np
import re

class DarshanOutputParser:

    def __init__(self, datafilepath, max_iteration, node_nums):
        self.datafilepath = datafilepath
        splitted_datafilepath = datafilepath.split('_')
        try:
            self.procs_per_node = int(splitted_datafilepath[len(splitted_datafilepath)-1])
        except:
            self.procs_per_node = 8
        self.num_total_procs_regex = re.compile(r'^Number of processes:.+')
        self.num_total_bytes_read_regex = re.compile(r'^Total bytes read by app:.+')
        self.num_total_bytes_written_regex = re.compile(r'^Total bytes written by app:.+')
        self.total_read_time_regex = re.compile(r'^Total absolute read I/O time:.+')
        self.total_write_time_regex = re.compile(r'^Total absolute write I/O time:.+')
        self.max_iteration = max_iteration
        self.node_nums = node_nums
        self.read_file()

    def get_num_nodes(self, line):
        found_num_nodes = False
        num_total_procs = -1
        num_nodes = -1
        if self.num_total_procs_regex.search(line):
            num_total_procs = int(line.split(':')[1].strip())
            num_nodes = int(num_total_procs / self.procs_per_node)
            found_num_nodes = True
        return found_num_nodes, num_nodes

    def get_total_bytes_read(self, line):
        found_total_bytes_read = False
        num_total_bytes_read = -1
        if self.num_total_bytes_read_regex.search(line):
            num_total_bytes_read = int(line.split(':')[1].strip())
            found_total_bytes_read = True
        return found_total_bytes_read, num_total_bytes_read

    def get_total_bytes_written(self, line):
        found_total_bytes_written = False
        num_total_bytes_written = -1
        if self.num_total_bytes_written_regex.search(line):
            num_total_bytes_written = int(line.split(':')[1].strip())
            found_total_bytes_written = True
        return found_total_bytes_written, num_total_bytes_written

    def get_total_read_time(self, line):
        found_total_read_time = False
        total_read_time = -1
        if self.total_read_time_regex.search(line):
            total_read_time = float(line.split(':')[1].strip())
            found_total_read_time = True
        return found_total_read_time, total_read_time

    def get_total_write_time(self, line):
        found_total_write_time = False
        total_write_time = -1
        if self.total_write_time_regex.search(line):
            total_write_time = float(line.split(':')[1].strip())
            found_total_write_time = True
        return found_total_write_time, total_write_time

    def read_file(self):
        with open(self.datafilepath, 'r') as dfile:
            max_num_node_len = len(self.node_nums)
            self.read_bandwidth_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.read_latency_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.write_bandwidth_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.write_latency_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            current_iteration = 1
            current_num_nodes = -1
            current_num_total_bytes_read = -1
            current_num_total_bytes_written = -1
            current_total_read_time = -1
            current_total_write_time = -1
            line = dfile.readline()
            while (line):
                found_num_nodes, num_nodes = self.get_num_nodes(line)
                if found_num_nodes:
                    current_num_nodes = num_nodes
                found_total_bytes_read, num_total_bytes_read = self.get_total_bytes_read(line)
                if found_total_bytes_read:
                    current_num_total_bytes_read = num_total_bytes_read
                found_total_bytes_written, num_total_bytes_written = self.get_total_bytes_written(line)
                if found_total_bytes_written:
                    current_num_total_bytes_written = num_total_bytes_written
                found_total_read_time, total_read_time = self.get_total_read_time(line)
                if found_total_read_time:
                    current_total_read_time = total_read_time
                found_total_write_time, total_write_time = self.get_total_write_time(line)
                if found_total_write_time:
                    current_total_write_time = total_write_time
                if not (current_num_nodes == -1 or current_num_total_bytes_read == -1\
                    or current_num_total_bytes_written == -1 or current_total_read_time == -1\
                    or current_total_write_time == -1):
                    read_bw = current_num_total_bytes_read / (1024 * 1024 * current_total_read_time)
                    write_bw = current_num_total_bytes_written / (1024 * 1024 * current_total_write_time)
                    self.read_bandwidth_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = read_bw
                    self.read_latency_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = current_total_read_time
                    self.write_bandwidth_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = write_bw
                    self.write_latency_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = current_total_write_time
                    current_iteration += 1
                    current_num_nodes = -1
                    current_num_total_bytes_read = -1
                    current_num_total_bytes_written = -1
                    current_total_read_time = -1
                    current_total_write_time = -1
                line = dfile.readline()

class DarshanOutputParserApp:

    def __init__(self, datafilepath, max_iteration, num_stages, node_nums):
        self.datafilepath = datafilepath
        splitted_datafilepath = datafilepath.split('_')
        try:
            self.procs_per_node = int(splitted_datafilepath[len(splitted_datafilepath)-1])
        except:
            self.procs_per_node = 8
        self.num_total_procs_regex = re.compile(r'^Number of processes:.+')
        self.num_total_bytes_read_regex = re.compile(r'^Total bytes read by app:.+')
        self.num_total_bytes_written_regex = re.compile(r'^Total bytes written by app:.+')
        self.total_read_time_regex = re.compile(r'^Total absolute read I/O time:.+')
        self.total_write_time_regex = re.compile(r'^Total absolute write I/O time:.+')
        self.max_iteration = max_iteration
        self.node_nums = node_nums
        self.num_stages = num_stages
        self.read_file()

    def get_num_nodes(self, line):
        found_num_nodes = False
        num_total_procs = -1
        num_nodes = -1
        if self.num_total_procs_regex.search(line):
            num_total_procs = int(line.split(':')[1].strip())
            num_nodes = int(num_total_procs / self.procs_per_node)
            found_num_nodes = True
        return found_num_nodes, num_nodes

    def get_total_bytes_read(self, line):
        found_total_bytes_read = False
        num_total_bytes_read = -1
        if self.num_total_bytes_read_regex.search(line):
            num_total_bytes_read = int(line.split(':')[1].strip())
            found_total_bytes_read = True
        return found_total_bytes_read, num_total_bytes_read

    def get_total_bytes_written(self, line):
        found_total_bytes_written = False
        num_total_bytes_written = -1
        if self.num_total_bytes_written_regex.search(line):
            num_total_bytes_written = int(line.split(':')[1].strip())
            found_total_bytes_written = True
        return found_total_bytes_written, num_total_bytes_written

    def get_total_read_time(self, line):
        found_total_read_time = False
        total_read_time = -1
        if self.total_read_time_regex.search(line):
            total_read_time = float(line.split(':')[1].strip())
            found_total_read_time = True
        return found_total_read_time, total_read_time

    def get_total_write_time(self, line):
        found_total_write_time = False
        total_write_time = -1
        if self.total_write_time_regex.search(line):
            total_write_time = float(line.split(':')[1].strip())
            found_total_write_time = True
        return found_total_write_time, total_write_time

    def read_file(self):
        with open(self.datafilepath, 'r') as dfile:
            max_num_node_len = len(self.node_nums)
            self.read_bandwidth_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.read_latency_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.write_bandwidth_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            self.write_latency_array = np.full((self.max_iteration, max_num_node_len), -1.0)
            current_iteration = 1
            current_step = 1
            current_num_nodes = -1
            current_num_total_bytes_read = -1
            current_num_total_bytes_written = -1
            current_total_read_time = -1
            current_total_write_time = -1
            line = dfile.readline()
            while (line):
                found_num_nodes, num_nodes = self.get_num_nodes(line)
                if found_num_nodes:
                    current_num_nodes = num_nodes

                found_total_bytes_read, num_total_bytes_read = self.get_total_bytes_read(line)
                if found_total_bytes_read:
                    if current_step%self.num_stages == 1:
                        current_num_total_bytes_read = num_total_bytes_read
                    else:
                        current_num_total_bytes_read += num_total_bytes_read

                found_total_bytes_written, num_total_bytes_written = self.get_total_bytes_written(line)
                if found_total_bytes_written:
                    if current_step%self.num_stages == 1:
                        current_num_total_bytes_written = num_total_bytes_written
                    else:
                        current_num_total_bytes_written += num_total_bytes_written

                found_total_read_time, total_read_time = self.get_total_read_time(line)
                if found_total_read_time:
                    if current_step%self.num_stages == 1:
                        current_total_read_time = total_read_time
                    else:
                        current_total_read_time += total_read_time

                found_total_write_time, total_write_time = self.get_total_write_time(line)
                if found_total_write_time:
                    if current_step%self.num_stages == 1:
                        current_total_write_time = total_write_time
                    else:
                        current_total_write_time += total_write_time

                current_step += 1
                if (current_step%self.num_stages):
                    continue
                if not (current_num_nodes == -1 or current_num_total_bytes_read == -1\
                    or current_num_total_bytes_written == -1 or current_total_read_time == -1\
                    or current_total_write_time == -1):
                    read_bw = current_num_total_bytes_read / (1024 * 1024 * current_total_read_time)
                    write_bw = current_num_total_bytes_written / (1024 * 1024 * current_total_write_time)
                    self.read_bandwidth_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = read_bw
                    self.read_latency_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = current_total_read_time
                    self.write_bandwidth_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = write_bw
                    self.write_latency_array[current_iteration%self.max_iteration-1, \
                        int(math.log(current_num_nodes, 2))] = current_total_write_time
                    current_iteration += 1
                    current_num_nodes = -1
                    current_num_total_bytes_read = -1
                    current_num_total_bytes_written = -1
                    current_total_read_time = -1
                    current_total_write_time = -1
                line = dfile.readline()
