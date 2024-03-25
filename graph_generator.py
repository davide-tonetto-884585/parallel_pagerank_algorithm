import networkx as nx


def save_graph_to_file(graph, filename):
    with open(filename, "w") as file:
        for edge in graph.edges:
            file.write(f"{edge[0]} {edge[1]}\n")


n = 10000
density = 0.8

print(int(density * (n * (n - 1))))

G = nx.dense_gnm_random_graph(n, int(density * (n * (n - 1))))

# Convert it to a directed graph
G = nx.DiGraph(G)

print(f"Number of nodes: {G.number_of_nodes()}")
print(f"Number of edges: {G.number_of_edges()}")
print(f'Density: {nx.density(G)}')

save_graph_to_file(G, 'data/graph.txt')
