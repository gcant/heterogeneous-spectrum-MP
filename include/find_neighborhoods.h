#pragma once
#include "WeightedGraph.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <tuple>

std::vector<std::vector<int>> list_paths(WGraph &G, int i, int j, int r) {
  if (r==1) {
    if (G.has_edge(i,j))
      return {{i,j}};
    else
      return {};
  }
  else if (r>1) {
    std::vector<std::vector<int>> ans;
    for (auto const [k, w] : G.neighbors(i)){
      if (k!=i){
        for (auto x : list_paths(G, k, j, r-1)){
          if (std::find(x.begin(), x.end(), i) == x.end() ) {
            x.insert(x.begin(),i);
            ans.push_back(x);
          }
        }
      }
    }
    return ans;
  }
  return {};
}

unsigned long long int eid(WGraph &G, int i, int j){
  if (i<j) {
    return G.number_of_nodes()*i + j;
  }
  else {
    return G.number_of_nodes()*j + i;
  }
}

std::pair<int,int> eid(WGraph &G, unsigned long long int e){
  return {e/G.number_of_nodes(), e%G.number_of_nodes()};
}

std::vector<std::pair<int,int>> find_neighborhood_edges(WGraph &G, int i, int r){
  std::unordered_set<unsigned long long int> edges;
  for (auto const [j,w] : G.neighbors(i)){
    edges.insert( eid(G,i,j) );
  }
  for (auto const [j,w1] : G.neighbors(i)){
    for (auto const [k,w2] : G.neighbors(i)){
      if (j<k) {
        for (int rr=1; rr<=r; ++rr){
          for (auto path : list_paths(G, j, k, rr)){
            for (int y=1; y<path.size(); ++y){
              edges.insert( eid(G, path[y-1], path[y]) );
            }
          }
        }
      }
    }
  }
  std::vector<std::pair<int,int>> ans;
  for (auto edge : edges){
    ans.push_back( eid(G, edge) );
  }
  return ans;
}

std::pair<std::vector<std::pair<int,int>>,int> find_neighborhood_edges_r(WGraph &G, int i, int r, int r_min, int KMAX){
  std::vector<std::unordered_set<unsigned long long int>> edges(r+1);
  std::unordered_set<int> nodes;
  nodes.insert( i );
  for (auto const [j,w] : G.neighbors(i)) {
    edges[0].insert( eid(G,i,j) );
    nodes.insert( j );
  }
  int local_r=r_min;
  for (int rr=1; rr<=r_min; ++rr) {
    for (auto const [j,w1] : G.neighbors(i)) {
      for (auto const [k,w2] : G.neighbors(i)){
        if (j<k) {
          for (auto path : list_paths(G, j, k, rr)){
            for (int y=1; y<path.size(); ++y){
              edges[rr].insert( eid(G, path[y-1], path[y]) );
	      nodes.insert( path[y-1] );
	      nodes.insert( path[y] );
            }
          }
        }
      }
    }
  }
  for (int rr=std::max(r_min+1,1); rr<=r; ++rr) {
    if (nodes.size() > KMAX+1) break;
    for (auto const [j,w1] : G.neighbors(i)) {
      for (auto const [k,w2] : G.neighbors(i)){
        if (j<k) {
          for (auto path : list_paths(G, j, k, rr)){
            for (int y=1; y<path.size(); ++y){
              edges[rr].insert( eid(G, path[y-1], path[y]) );
	      nodes.insert( path[y-1] );
	      nodes.insert( path[y] );
            }
          }
        }
      }
    }
    if (nodes.size() > KMAX+1) break;
    else local_r = rr;
  }
  std::vector<std::pair<int,int>> ans;
  for (auto edge : edges[0]) {
    ans.push_back( eid(G, edge) );
  }
  for (int rr=1; rr<=local_r; ++rr) {
    for (auto edge : edges[rr]) {
      ans.push_back( eid(G, edge) );
    }
  }
  return {ans, local_r};
}

std::vector<std::pair<int,int>> intersection( WGraph &G, std::vector<std::pair<int,int>> const &N1, std::vector<std::pair<int,int>> const &N2 ){
  std::unordered_set<int> edges1;
  std::vector<std::pair<int,int>> edges_intersect;
  for (auto x : N1){
    edges1.insert( eid(G, x.first, x.second) );
  }
  for (auto x : N2){
    if (edges1.count( eid(G, x.first, x.second) ))
      edges_intersect.push_back(x);
  }
  return edges_intersect;
}

std::vector<std::pair<int,int>> difference( WGraph &G, std::vector<std::pair<int,int>> const &N1, std::vector<std::pair<int,int>> const &N2 ){
  std::unordered_set<int> edges2;
  std::vector<std::pair<int,int>> edges_difference;
  for (auto x : N2){
    edges2.insert( eid(G, x.first, x.second) );
  }
  for (auto x : N1){
    if (!(edges2.count( eid(G, x.first, x.second) )))
      edges_difference.push_back(x);
  }
  return edges_difference;
}
