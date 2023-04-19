import csv

import matplotlib.pyplot as plt

plt.switch_backend('agg')

# Output file attributes
label_input_data_size = 'InputDataSize (MB)'
label_bloom_filter_bits = 'Bits per entry'
label_max_buffer_pool_size = 'Buffer Pool Max Size (# of directory entries)'
label_throughput = 'Throughput (MB /sec)'
label_latency = 'Latency (sec)'
memtable_max_size = 'dbMemtableMaxSize(MB)'
buffer_pool_max_size = 'bufferPoolMaxSize'
eviction_policy = 'evictionPolicy'
search_type = 'searchType'
input_data_size = 'inputDataSize(MB)'
throughput = 'throughput(MB/sec)'
latency = 'latency(sec)'
bloom_filter_bits = 'bloomFilterBits'
source_dir = './build/experiments/experiments_db_CSV'

# Other useful global vars
colors = [
    ('darkblue', 'navy'),
    ('salmon', 'lightcoral'),
    ('#ebe234', '#f5d576'),
    ('salmon', 'lightcoral'),
    ('lightseagreen', 'darkturquoise')
]


def read_csv(file, fixed_attribute, x, y):
    with open(file) as csvfile:
        reader = csv.DictReader(csvfile)
        dic = {}
        for row in reader:
            key = row[fixed_attribute]
            if key not in dic:
                dic[key] = ([], [])
            dic[key][0].append(row[x])
            dic[key][1].append(float(row[y]))
    return dic


def draw_graph(data_dict, file_name, title, x_label, y_label, legend_key="", exact=False, log_scale_y=False,
               log_scale_x=False):
    y_elements = []
    plt.figure(constrained_layout=True)
    for i, key in enumerate(data_dict):
        x, y = data_dict[key]
        y_elements.extend(y)
        plt.plot(
            x, y,
            color=colors[i][0],
            linewidth=3,
            marker='o',
            markerfacecolor=colors[i][1],
            markersize=5,
            label=legend_key.format(str(key))
        )

    if log_scale_y:
        plt.yscale("log")

    if log_scale_x:
        plt.xscale("log")

    # Set labels
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    ax = plt.gca()
    plt.legend()
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.spines['left'].set_visible(False)
    ax.spines['bottom'].set_visible(False)
    if exact:
        plt.yticks(y_elements)
    plt.savefig(file_name)
    plt.clf()


def draw_step_one():
    exp1_source_dir = f'{source_dir}_step1'

    legend_key = 'DB memtable size: {} MB'
    get_operation_csv_file = f'{exp1_source_dir}/get_operation.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./get_operation.png',
        title='Get operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        legend_key=legend_key,
        log_scale_y=True,
    )

    draw_graph(
        data_dict=read_csv(get_operation_csv_file, memtable_max_size, input_data_size, latency),
        file_name=f'./get_operation_latency.png',
        title='Get operation latency (Latency vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_latency,
        legend_key=legend_key
    )

    put_operation_csv_file = f'{exp1_source_dir}/put_operation.csv'
    draw_graph(
        data_dict=read_csv(put_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./put_operation.png',
        title='Put operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        legend_key=legend_key
    )

    scan_operation_csv_file = f'{exp1_source_dir}/scan_operation.csv'
    draw_graph(
        data_dict=read_csv(scan_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./scan_operation.png',
        title='Scan operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        legend_key=legend_key,
        log_scale_y=True
    )


def draw_step_two():
    exp2_source_dir = source_dir + "_step2"

    # 2-1
    legend_key_evictions = 'Eviction Policy: {}'
    get_operation_csv_lru = f'{exp2_source_dir}/get_operation_eviction_spatial.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_lru, eviction_policy, buffer_pool_max_size, throughput),
        file_name=f'./get_operation_eviction_spatial.png',
        title='Queries with spatial locality (Throughput vs. Max buffer pool size)',
        x_label=label_max_buffer_pool_size,
        y_label=label_throughput,
        legend_key=legend_key_evictions
    )

    get_operation_csv_clock = f'{exp2_source_dir}/get_operation_eviction_random.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_clock, eviction_policy, buffer_pool_max_size, throughput),
        file_name=f'./get_operation_eviction_random.png',
        title='Random Queries (Throughput vs. Max buffer pool size)',
        x_label=label_max_buffer_pool_size,
        y_label=label_throughput,
        legend_key=legend_key_evictions
    )

    # 2-2
    legend_key_search_types = 'Search Types: {}'
    get_operation_csv_file_search_types = f'{exp2_source_dir}/get_operation_search_types.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_file_search_types, search_type, input_data_size, throughput),
        file_name=f'./get_operation_search_types.png',
        title='Search Operations (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        legend_key=legend_key_search_types
    )

    # 2-2
    legend_key_search_types = 'Search Types: {}'
    get_operation_csv_file_search_types = f'{exp2_source_dir}/scan_operation_search_types.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_file_search_types, search_type, input_data_size, throughput),
        file_name=f'./scan_operation_search_types.png',
        title='Scan Types (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        legend_key=legend_key_search_types
    )


def draw_step_three():
    exp3_source_dir = source_dir + "_step3"

    # 3-1
    get_operation_csv_file = f'{exp3_source_dir}/get_operation_lsm.csv'
    draw_graph(
        data_dict=read_csv(get_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./get_operation_lsm.png',
        title='Get operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
    )

    draw_graph(
        data_dict=read_csv(get_operation_csv_file, memtable_max_size, input_data_size, latency),
        file_name=f'./get_operation_latency_lsm.png',
        title='Get operation latency (Latency vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_latency,
    )

    put_operation_csv_file = f'{exp3_source_dir}/put_operation_lsm.csv'
    draw_graph(
        data_dict=read_csv(put_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./put_operation_lsm.png',
        title='Put operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput,
        log_scale_x=True
    )

    scan_operation_csv_file = f'{exp3_source_dir}/scan_operation_lsm.csv'
    draw_graph(
        data_dict=read_csv(scan_operation_csv_file, memtable_max_size, input_data_size, throughput),
        file_name=f'./scan_operation_lsm.png',
        title='Scan operation (Throughput vs. Input data size)',
        x_label=label_input_data_size,
        y_label=label_throughput
    )

    # 3-2
    get_operation_bf_csv_file = f'{exp3_source_dir}/get_operation_bf.csv'
    draw_graph(
        data_dict=read_csv(get_operation_bf_csv_file, memtable_max_size, bloom_filter_bits, throughput),
        file_name=f'./get_operation_bf.png',
        title='Get queries (Throughput vs. Bloom Filter Bits per Entry)',
        x_label=label_bloom_filter_bits,
        y_label=label_throughput
    )


if __name__ == "__main__":
    draw_step_one()
    draw_step_two()
    draw_step_three()
