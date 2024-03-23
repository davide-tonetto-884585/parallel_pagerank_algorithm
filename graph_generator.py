import networkx as nx

# Number of nodes
n = 10

# Probability for edge creation
p = 0.7

# Create a random directed graph
G = nx.gnp_random_graph(n, p, directed=True)

# export the graph to a file txt as edge list with no weights
nx.write_edgelist(G, "data/graph.txt")
