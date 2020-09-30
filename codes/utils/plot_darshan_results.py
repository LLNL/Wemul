import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
from pprint import pprint
import seaborn as sns
import sys

from darshan_output_parser import *

def plot_figure(data_frame_mean, data_frame_sem, key_column_name, xlabel_text, ylabel_text1, \
    ylabel_text2, max_bw, max_latency, outputdir, appname, annotate=False, log_xscale=False, \
    log_yscale=False):

    if len(data_frame_mean) == 0:
        return

    key_column_elements = data_frame_mean[key_column_name]
    xtick_labels = data_frame_mean["nodes"]
    # style
    plt.style.use('classic')
    fig, sub_plot = plt.subplots()

    # create a color palette
    palette = plt.get_cmap('Dark2')

    _, ax2 = plt.subplots()
    ax1 = ax2.twinx()

    matplotlib.rcParams['hatch.linewidth'] = 0.8
    hatch_list = ['\\\\', '--', '//', '--', '*', '||']
    markers_list = ['o', '^', 's', 'p', 'H', 'X', '*', 'P', 'D', '8']

    # ax1_plot = None
    ax2_plots = []
    bar_columns = []
    width = 0.35
    if appname != 'dl_train':
        x_diff = -1 * width
    else:
        x_diff = 0

    num = 0
    for column in data_frame_mean.drop(key_column_name, axis=1):
        if column == 'nodes' or column == 'read_latency' or column == 'write_latency':
            continue
        bar_columns.append(column)

        ax2_plots.append(ax2.bar([x+x_diff/2 for x in key_column_elements], data_frame_mean[column], width, \
            yerr=data_frame_sem[column], color=palette(num), linewidth=0.8, alpha=0.7, label=column, \
            hatch=hatch_list[num]))

        num += 1

        if appname != 'dl_train':
            x_diff += 2 * 0.35

    if appname == 'dl_train':
        num += 1

    line_columns = []
    marker_id = 0
    for column in data_frame_mean.drop(key_column_name, axis=1):
        if column == 'nodes' or column == 'read_bw' or column == 'write_bw':
            continue

        line_columns.append(column)
        marker = 'o'
        try:
            marker = markers_list[marker_id]
        except:
            marker = 'o'

        line_style = '-'

        ax1_plot = ax1.errorbar(key_column_elements, data_frame_mean[column], yerr=data_frame_sem[column], \
            marker=marker, color=palette(num), linewidth=2, alpha=1, label=column, linestyle=line_style, \
            markersize=7)

        if annotate:
            for xy in zip(key_column_elements, data_frame_mean[column]):
                ax1.annotate('%.2f' % xy[1], xy=xy, textcoords='offset points', \
                    xytext=(-10, 5), fontsize=8)

        num += 1
        marker_id += 1

    ax1.legend(loc='upper right', fontsize='medium', framealpha=0.5)
    ax2.legend(loc='upper left', fontsize='medium', framealpha=0.5)
    # plt.figlegend((ax1_plot[0], ax2_plots[0], ax2_plots[1]), \
    #     (line_columns[0], bar_columns[0], bar_columns[1]), \
    #     framealpha=0.5, prop={'size': 15}, loc='upper left')

    ax1.set_ylabel(ylabel_text1, fontsize='x-large')
    ax2.set_ylabel(ylabel_text2, fontsize='x-large')
    ax2.set_xlabel(xlabel_text, fontsize='x-large')
    ax1.set_xticks(key_column_elements)
    ax1.set_xticklabels(xtick_labels)
    sub_plot.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())

    negate_from_min = 1
    add_to_max = 1
    if log_xscale:
        ax1.set_xscale('log', basex=2)
        ax1.set_xticks(key_column_elements)
        sub_plot.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        negate_from_min = 3
        add_to_max = 30

    if log_yscale:
        ax1.yscale('log')
        sub_plot.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        sub_plot.get_yaxis().get_major_formatter().set_scientific(False)

    ax1.set_xlim(left=key_column_elements[0]-negate_from_min, \
        right=key_column_elements[len(key_column_elements)-1] + add_to_max)

    ax1.set_ylim(bottom=0, top=max_latency)
    ax2.set_ylim(bottom=0, top=max_bw)

    ax1.tick_params(labelsize='large')
    ax2.tick_params(labelsize='large')

    file_name = outputdir + '/' + appname + '_plot.pdf'
    print(file_name)
    pprint(data_frame_mean)
    pprint(data_frame_sem)

    plt.savefig(file_name, dpi=300, bbox_inches='tight')

    plt.clf()

def plot_dict_colored(data_dict, columns, appname, storage, outputdir, max_bw, max_latency):
    _, ax = plt.subplots()
    indices = list(range(len(columns)))

    df_read_bw = pd.DataFrame(data_dict[("read_bw", appname, storage)], columns=indices)
    df_read_latency = pd.DataFrame(data_dict[("read_latency", appname, storage)], columns=indices)
    df_write_bw = pd.DataFrame(data_dict[("write_bw", appname, storage)], columns=indices)
    df_write_latency = pd.DataFrame(data_dict[("write_latency", appname, storage)], columns=indices)

    if appname == 'dl_train':
        df_mean = pd.DataFrame({"index": indices, "nodes": columns, \
            "read_bw": df_read_bw.mean(), "read_latency": df_read_latency.mean()})
        df_sem = pd.DataFrame({"index": indices, "nodes": columns, \
            "read_bw": df_read_bw.sem(), "read_latency": df_read_latency.sem()})
        plot_figure(df_mean[["index", "nodes", "read_bw", "read_latency"]], \
            df_sem[["index", "nodes", "read_bw", "read_latency"]], \
            "index", "Number of Nodes", "Latency (s)", "Bandwidth (MiB/s)", max_bw, max_latency, \
            outputdir, appname)
    else:
        df_mean = pd.DataFrame({"index": indices, "nodes": columns, \
            "read_bw": df_read_bw.mean(), "read_latency": df_read_latency.mean(), \
            "write_bw": df_write_bw.mean(), "write_latency": df_write_latency.mean()})
        df_sem = pd.DataFrame({"index": indices, "nodes": columns, \
            "read_bw": df_read_bw.sem(), "read_latency": df_read_latency.sem(), \
            "write_bw": df_write_bw.sem(), "write_latency": df_write_latency.sem()})
        plot_figure(df_mean[["index", "nodes", "read_bw", "write_bw", "read_latency", "write_latency"]], \
            df_sem[["index", "nodes", "read_bw", "write_bw", "read_latency", "write_latency"]], \
            "index", "Number of Nodes", "Latency (s)", "Bandwidth (MiB/s)", max_bw, max_latency, \
            outputdir, appname)

def plot_dict(data_dict, columns, appname, storage, outputdir):
    _, ax = plt.subplots()
    indices = list(range(len(columns)))

    df_read_bw = pd.DataFrame(data_dict[("read_bw", appname, storage)], columns=indices)
    df_read_latency = pd.DataFrame(data_dict[("read_latency", appname, storage)], columns=indices)
    df_write_bw = pd.DataFrame(data_dict[("write_bw", appname, storage)], columns=indices)
    df_write_latency = pd.DataFrame(data_dict[("write_latency", appname, storage)], columns=indices)

    df_mean = pd.DataFrame({"index": indices, "nodes": columns, \
        "read_bw": df_read_bw.mean(), "read_latency": df_read_latency.mean(), \
        "write_bw": df_write_bw.mean(), "write_latency": df_write_latency.mean()})
    df_sem = pd.DataFrame({"index": indices, "nodes": columns, \
        "read_bw": df_read_bw.sem(), "read_latency": df_read_latency.sem(), \
        "write_bw": df_write_bw.sem(), "write_latency": df_write_latency.sem()})

    df_mean[["index", "read_bw", "write_bw"]].plot(x="index", kind="bar", \
        yerr=df_sem[["index", "read_bw", "write_bw"]], capsize=4, ax=ax, rot=0,
        color="white", edgecolor="black")

    # # only read bandwidth for DL
    # df_mean[["index", "read_bw"]].plot(x="index", kind="bar", \
    #     yerr=df_sem[["index", "read_bw"]], capsize=4, ax=ax, rot=0,
    #     color="white", edgecolor="black")

    bars = ax.patches
    hatches = ''.join(h*len(df_mean[["index", "read_bw", "write_bw"]]) for h in '\\--')
    for bar, hatch in zip(bars, hatches):
        bar.set_hatch(hatch)

    df_mean[["index", "read_latency"]].plot(x="index", kind="line", \
        yerr=df_sem[["index", "read_latency"]], \
        capsize=4, ax=ax, rot=0, secondary_y=True, \
        marker='o', color="black")
    df_mean[["index", "write_latency"]].plot(x="index", kind="line", \
        yerr=df_sem[["index", "write_latency"]], \
        capsize=4, ax=ax, rot=0, secondary_y=True, \
        marker='x', color="black")

    print(df_mean[["index", "read_bw", "write_bw", "read_latency", "write_latency"]])

    ax.set_xticklabels(df_mean["nodes"])
    ax.set_ylim(bottom=0, top=300000)
    ax.right_ax.set_ylim(bottom=0, top=300)
    ax.set_xlabel("Number of nodes", fontsize='x-large')
    ax.set_ylabel("Bandwidth (MiB/s)", fontsize='x-large')
    ax.right_ax.set_ylabel("Latency (s)", fontsize='x-large')
    ax.legend(loc="upper left", fontsize='medium')
    ax.right_ax.legend(loc="upper right", fontsize='medium')
    plt.xticks(fontsize='large')
    plt.yticks(fontsize='large')
    plt.savefig(outputdir + "/" + appname + "_plot.pdf", dpi=300, bbox_inches='tight')
    plt.clf()

def add_darshan_output_to_dict(dop, data_dict, appname):
    data_dict[("read_bw", appname, "gpfs")] = \
        dop.read_bandwidth_array
    data_dict[("write_bw", appname, "gpfs")] = \
        dop.write_bandwidth_array
    data_dict[("read_latency", appname, "gpfs")] = \
        dop.read_latency_array
    data_dict[("write_latency", appname, "gpfs")] = \
        dop.write_latency_array

def plot_dataframe(dataframe, appname, outputdir):
    _, ax = plt.subplots()

    bar = sns.barplot(x="num_nodes", y="bandwidth", hue="operation", data=dataframe)

    # Define some hatches
    hatches = ['//', '--', '\\\\']

    # Loop over the bars
    for i,thisbar in enumerate(bar.patches):
        # Set a different hatch for each bar
        thisbar.set_hatch(hatches[i%len(hatches)])

    ax.set_ylim(bottom=0, top=40000)
    # ax.right_ax.set_ylim(bottom=0, top=500)
    ax.set_xlabel("Number of nodes", fontsize='x-large')
    ax.set_ylabel("Bandwidth (MiB/s)", fontsize='x-large')
    # ax.right_ax.set_ylabel("Latency (s)", fontsize='x-large')
    ax.legend(loc="upper left", fontsize='medium')
    # ax.right_ax.legend(loc="upper right", fontsize='medium')
    plt.xticks(fontsize='large')
    plt.yticks(fontsize='large')
    plt.savefig(outputdir + "/" + appname + "_sbn_plot.pdf", dpi=300, bbox_inches='tight')
    plt.clf()

def add_darshan_output_to_dataframe(dop, dataframe):
    columns = len(dop.node_nums)
    index = 0
    for column in range(columns):
        for read_bw, read_latency, write_bw, write_latency in zip(dop.read_bandwidth_array[:,column], dop.read_latency_array[:,column],  dop.write_bandwidth_array[:,column], dop.write_latency_array[:,column]):
            row = [2**column]
            row.append("read")
            row.append(read_bw)
            row.append(read_latency)
            dataframe.loc[index] = row
            index += 1
            row = [2**column]
            row.append("write")
            row.append(write_bw)
            row.append(write_latency)
            dataframe.loc[index] = row
            index += 1

def main():
    darshan_datafilepath = sys.argv[1]
    outputdir = sys.argv[2]
    appname = sys.argv[3]
    max_iteration = int(sys.argv[4])
    max_bw = int(sys.argv[5])
    max_latency = int(sys.argv[6])
    data_dict = {}
    num_nodes = [1, 2, 4, 8, 16]
    dop = None
    if appname == 'app':
        num_stages = 3
        dop = DarshanOutputParserApp(darshan_datafilepath, max_iteration, num_stages, num_nodes)
    else:
        dop = DarshanOutputParser(darshan_datafilepath, max_iteration, num_nodes)

    add_darshan_output_to_dict(dop, data_dict, appname)
    plot_dict_colored(data_dict, num_nodes, appname, "gpfs", outputdir, max_bw, max_latency)
    # dataframe = pd.DataFrame(columns=["num_nodes", "operation", "bandwidth", "latency"])
    # add_darshan_output_to_dataframe(dop, dataframe)
    # plot_dataframe(dataframe, "dag_emulation", outputdir)

if __name__ == "__main__":
    main()
