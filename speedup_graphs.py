import os
import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import numpy as np


def parse_speedup_csv(filename):
    with open(filename) as file:
        content = file.readlines()

        # Remove the header
        content = content[1:]
        content = [x.strip().split(',') for x in content]
        content = [[int(x[0]), float(x[1])] for x in content]

    return content


def plot_speedup(dataset, seq_times, max_threads, axs, axs_index=None, title='Speedup vs Number of threads'):
    if axs_index is not None:
        axs = axs[axs_index]

    # plot all csv files
    for filename, data in dataset.items():
        legend_name = filename
        if 'pagerank' in filename:
            legend_name = filename[filename.index('_', filename.index('_') + 1)
                                   + 1:filename.index('.')]

        axs.plot([x[0] for x in data], [seq_times[filename] / x[1] for x in data], label=legend_name, marker='o')

    # draw a line for ideal speedup and areas of interest
    system_cores = os.cpu_count()

    line = mlines.Line2D([0, max_threads], [0, max_threads], color='red', linestyle="dashed", label="Linear speedup")
    axs.add_line(line)

    axs.fill_between([x for x in range(0, max_threads + 1)],
                     [1] * (max_threads + 1),
                     alpha=0.3, color="red", label="No improvement zone")

    axs.fill_between([x for x in range(0, max_threads + 1)],
                     [x for x in range(0, max_threads + 1)],
                     alpha=0.1, color="blue", label="Sublinear speedup zone")

    axs.fill_between([x for x in range(0, max_threads + 1)],
                     [x for x in range(0, max_threads + 1)],
                     max_threads, alpha=0.1, color="green", label="Super linear speedup zone")

    axs.set_xlim(0, max_threads)
    axs.set_ylim(0, system_cores)

    axs.set_xlabel('Number of threads')
    axs.set_ylabel('Speedup')
    axs.set_title(title)
    axs.grid()

    axs.legend()


if __name__ == '__main__':
    # get all csv in folder
    folder_path = 'src'
    filenames = [f for f in os.listdir(folder_path) if os.path.isfile(os.path.join(folder_path, f))]

    # parse all csv files
    data_alg_1 = {}
    for filename in filenames:
        if '_v2' not in filename:
            data_alg_1[filename] = parse_speedup_csv(os.path.join(folder_path, filename))

    data_alg_2 = {}
    for filename in filenames:
        if '_v2' in filename:
            temp = filename[:filename.index('_v2')] + filename[filename.index('_v2') + 3:]
            data_alg_2[temp] = parse_speedup_csv(os.path.join(folder_path, filename))

    # sort data by name
    data_alg_1 = dict(sorted(data_alg_1.items()))
    data_alg_2 = dict(sorted(data_alg_2.items()))

    # sequential times
    seq_times_alg_1 = {filename: data_alg_1[filename][0][1] for filename in data_alg_1}
    seq_times_alg_2 = {filename: data_alg_2[filename][0][1] for filename in data_alg_2}

    # plot data
    fig, axs = plt.subplots(2, 2, figsize=(15, 10))
    axs = [item for sublist in axs for item in sublist]

    plot_speedup(data_alg_1, seq_times_alg_1, 27, axs, 0, 'Algorithm 1')
    plot_speedup(data_alg_2, seq_times_alg_2, 27, axs, 1, 'Algorithm 2')

    # find minimum sequential time for each algorithm
    min_seq_times = {}
    for filename in filenames:
        if filename in seq_times_alg_1:
            min_seq_times[filename] = seq_times_alg_1[filename]

        if filename in seq_times_alg_2:
            if filename in min_seq_times:
                min_seq_times[filename] = min(min_seq_times[filename], seq_times_alg_2[filename])
            else:
                min_seq_times[filename] = seq_times_alg_2[filename]

    # plot data
    plot_speedup(data_alg_1, min_seq_times, 27, axs, 2,
                 'Algorithm 1 using minimum sequential time')
    plot_speedup(data_alg_2, min_seq_times, 27, axs, 3,
                 'Algorithm 2 using minimum sequential time')

    plt.legend(bbox_to_anchor=(-0.1, -0.15), loc='upper center', ncol=7)
    fig.suptitle('Speedup vs Number of Threads', fontsize=20)

    # plot for sequential times grouped by algorithm using grouped bar chart
    fig, ax = plt.subplots()
    bar_width = 0.45
    multiplier = 0
    dict_seq_times = {'Algorithm 1': [], 'Algorithm 2': []}
    for filename in seq_times_alg_1:
        dict_seq_times['Algorithm 1'].append(seq_times_alg_1[filename])

        if filename in seq_times_alg_2:
            dict_seq_times['Algorithm 2'].append(seq_times_alg_2[filename])
        else:
            dict_seq_times['Algorithm 2'].append(0)

    for attribute, measurement in dict_seq_times.items():
        offset = bar_width * multiplier
        rects = ax.bar(np.arange(len(seq_times_alg_1)) + offset, measurement, bar_width, label=attribute)
        ax.bar_label(rects, padding=3)
        multiplier += 1

    plt.ylabel('Time (ms)')
    plt.title('Sequential times')
    ax.set_xticks(np.arange(len(seq_times_alg_1)) + bar_width, [filename[filename.index('_', filename.index('_') + 1)
                                                                         + 1:filename.index('.')] for filename in
                                                                seq_times_alg_1], rotation=20, ha='right')
    plt.ylim([0, 35000])

    plt.legend()
    plt.grid()

    # make plt bigger
    fig = plt.gcf()
    fig.set_size_inches(9, 7)

    plt.show()

    # plot differnce in speedup between src and src_old folders
    data_alg_old = parse_speedup_csv('src_old/pagerank_speedup_soc_LiveJournal1.csv')
    data_alg_new = parse_speedup_csv('src/pagerank_speedup_soc_LiveJournal1.csv')

    fig, axs = plt.subplots(1, 1, figsize=(8, 5))
    plot_speedup({'Dynamic scheduling': data_alg_new, 'Static scheduling': data_alg_old},
                 {'Dynamic scheduling': min_seq_times['pagerank_speedup_soc_LiveJournal1.csv'],
                  'Static scheduling': min_seq_times['pagerank_speedup_soc_LiveJournal1.csv']}, 27, axs, None,
                 'Static vs Dynamic scheduling speedup for soc_LiveJournal1 graph')
    plt.show()
