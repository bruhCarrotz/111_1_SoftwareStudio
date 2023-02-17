import sys

def topological_sort(dictGraph):
    inDeg = {node : 0 for node in dictGraph} #setting up the dict and adjacency matrix
    for node in dictGraph:
        for neighbor in dictGraph[node]:
            inDeg[neighbor] += 1

    nodes_with_no_incoming_edges = [] #track nodes with in-degree 0
    for node in dictGraph:
        if inDeg[node] == 0:
            nodes_with_no_incoming_edges.append(node)

    nodesList = [] #result array
                
    #as long as there are nodes with no incoming edges that can be added to the ordering 
    while len(nodes_with_no_incoming_edges) > 0:
        node = nodes_with_no_incoming_edges.pop() #adding the nodes to an array
        nodesList.append(node)

        for neighbor in dictGraph[node]: #decrement in-degrees
            inDeg[neighbor] -= 1
            if inDeg[neighbor] == 0:
                nodes_with_no_incoming_edges.append(neighbor)

    if len(nodesList) == len(dictGraph):
        return nodesList #successfuly sorting all nodes
    else: print('Encountered a cycle!') #there's a cycle

def main():
    source_arr = []
    dest_arr = []
    res = []

    content=[] #parsing the input from .dot file
    for line in sys.stdin.readlines():
        content.append(line.strip())

    del content [0:4] #removing digraph information
    sortedElem = [i for i in content if "->" in i] #sorting the necessary info
    arr = [i.replace('"', '') for i in sortedElem] #remove quotation of the nodes
    #print(arr)

    for i in arr: #getting the sources and destination
        temp1 = i.split(' ')
        source_arr.append(temp1[0])
        dest_arr.append(temp1[2])
    #print(source_arr)
    #print(dest_arr)


    allnodes = zip(source_arr, dest_arr)
    temp = list(allnodes)
    #print(temp)

    dict1 = {} #turning allnodes list to be a dict
    for src, dest in temp:
        try: dict1[src].append(dest)
        except KeyError: dict1[src] = [dest]

    finalDict = {i: [] for i in dest_arr} #setting all destination to have a value of []
    finalDict.update(dict1) #combining both dictionaries
    #print(finalDict)

    result = topological_sort(finalDict)
    for i in result: print(i) #printing

if __name__ == "__main__":
    main()