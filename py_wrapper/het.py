import ctypes
import numpy as np
import networkx as nx

# C arrays of ints/doubles using numpy
array_int = np.ctypeslib.ndpointer(dtype=ctypes.c_int,ndim=1, flags='CONTIGUOUS')
array_double = np.ctypeslib.ndpointer(dtype=np.double,ndim=1, flags='CONTIGUOUS')

lib = ctypes.cdll.LoadLibrary("./het.so")

lib.spectrum.argtypes = [ ctypes.c_int,
        array_int, array_int, array_double,
        ctypes.c_double, array_double,
        ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, array_double ]

lib.spectrum.restype = ctypes.c_int 

lib.get_tol.restype = ctypes.c_double
lib.set_tol.restype = ctypes.c_double
lib.set_tol.argtypes = [ctypes.c_double]

def spectrum(edges, edge_weights, eps, X, r_max, k_max, r_min):

    e1 = np.array(edges[:,0]).astype(ctypes.c_int)
    e2 = np.array(edges[:,1]).astype(ctypes.c_int)

    out = np.ascontiguousarray(np.zeros(len(X)))

    lib.spectrum(ctypes.c_int(len(edges)),
    e1, e2, np.ascontiguousarray(edge_weights),
    ctypes.c_double(eps), X,
    ctypes.c_int(len(X)), ctypes.c_int(r_max), ctypes.c_int(k_max), ctypes.c_int(r_min),
    out)

    return out

def sparse_matrix_to_edges(M):
    i = M.nonzero()
    return np.array(i).T, np.array(M[i[0],i[1]],dtype=np.double)[0]


if __name__=="__main__":
#    G = nx.watts_strogatz_graph(5000, 4, 0.25)
#    edges, edge_weights = sparse_matrix_to_edges(nx.laplacian_matrix(G))
#    X = np.linspace(0,5,101)
#    out = spectrum(edges, edge_weights, 0.1, X, 3, 10, -1)
    A = np.genfromtxt('a_as.txt')
    edges = A[:,0:2].astype(int) 
    edge_weights = A[:,2]
    X = np.linspace(-5,5,101)
    out = spectrum(edges, edge_weights, 0.25, X, 1, 10, -1)

