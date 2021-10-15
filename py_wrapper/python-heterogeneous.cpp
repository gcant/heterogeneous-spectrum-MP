#include <iostream>
#include "neighborhoods.h"

double TOL = 1.0e-8;

double run_MP(std::vector<Neighborhood> &H,
    std::unordered_map<int,std::unordered_map<int,Neighborhood>> &H_diff,
    std::vector<std::pair<int,int>> &mess_id,
    WGraph &G,
    COMPLEX &z,
    std::vector<int> &r)
{
  int num_nodes = G.number_of_nodes();
  for (int s=0; s<100; ++s) {
    double Delta = 0.0;
    #pragma omp parallel for reduction(+:Delta)
    for (int x=0; x<mess_id.size(); x++) {
        Delta += H_diff[mess_id[x].first][mess_id[x].second].update_value(z,H_diff,r);
    }
    if (Delta < TOL*mess_id.size()) break;
  }

  #pragma omp parallel for
  for (int i=0; i<num_nodes; ++i) {
    H[i].update_value(z,H_diff,r);
  }

  double rho = 0.0;
  for (int i=0; i<num_nodes; ++i){
    rho += (1./(z-H[i].c_value)).imag();
  }
  rho = -rho/(num_nodes*3.14159265);
  return rho;
}


int count_nodes(EDGE_LIST const &edges, int node) {
  std::unordered_set<int> node_set;
  node_set.insert(node);
  for (auto edge : edges) {
    node_set.insert(edge.first);
    node_set.insert(edge.second);
  }
  node_set.erase(node);
  return node_set.size();
}

extern "C" {
  double get_tol(void) {
    return TOL;
  }

  double set_tol(double new_tol) {
    TOL = new_tol;
    return TOL;
  }

  int spectrum(int num_edges,
		  int *e1, int *e2, double *w,
		  double eps, double *X,
		  int num_pts, int r_max, int k_max, int r_min, double *out) {
    
    WGraph G;
    for (int i=0; i<num_edges; ++i) G.add_edge(e1[i],e2[i],w[i]);

    std::vector<int> r(G.number_of_nodes());
    std::vector<std::vector<std::pair<int,int>>> E_r(G.number_of_nodes());
    std::vector<Neighborhood> H(G.number_of_nodes());

    std::cerr << "Constructing node neighborhoods..." << std::endl;
    for (int i : G.nodes()) {
      if (G.degree(i) > k_max) {
        r[i] = r_min;
        E_r[i] = find_neighborhood_edges(G,i,r[i]);
      }
      else {
        r[i] = r_max;
        E_r[i] = find_neighborhood_edges(G,i,r[i]);
        while ((count_nodes(E_r[i],i)>k_max) && (r[i]>r_min)) {
          r[i] -= 1;
          E_r[i] = find_neighborhood_edges(G,i,r[i]);
        }
      }
      H[i].init(E_r[i],i,G);
    }

    std::cerr << "Constructing message neighborhoods..." << std::endl;
    std::unordered_map<int,std::unordered_map<int,Neighborhood>> H_diff;
    std::vector<std::pair<int,int>> mess_id;
    for (int i : G.nodes()) {
      for (int j : H[i].nodes) {
        if (r[j]>=0) {
          auto edges = difference(G,E_r[j], E_r[i]);
          H_diff[i][j].init(edges,j,G);
          mess_id.push_back({i,j});
        }
        else if (r[j]==-1) {
          if (!(H_diff[-1].count(j))) {
            H_diff[-1][j].init(E_r[j],j,G);
            mess_id.push_back({-1,j});
          }
        }
      }
    }


    std::cerr << "Starting MP..." << std::endl;
    for (int x=0; x<num_pts; x++) {
      COMPLEX z = {X[x],eps};
      std::cerr << X[x] << " " << (out[x] = run_MP(H,H_diff,mess_id,G,z,r)) << std::endl;
    }

    return 0;
  }
}

