#pragma once
#include<bits/stdc++.h> 
#include "exact_solver.h"

// Edge is a cost, a source and a destination
using Edge = std::pair<float, std::pair<int, int>>;
using Edges = std::vector<Edge>;
using ListOfPairs = std::vector<std::pair<int, int>>;
using VisitedList = std::vector<bool>;
using VisitedList_ptr = std::shared_ptr<VisitedList>;
using Path_ptr = std::shared_ptr<Path>;
using int_ptr = std::shared_ptr<int>;
using BigPartition = std::vector<ListOfNodes>;
using BigPartition_ptr = std::shared_ptr<BigPartition>;
using Distances = std::vector<float>;

  
// To represent Disjoint Sets 
struct DisjointSets 
{ 
    int *parent, *rnk; 
    int n; 
  
    // Constructor. 
    DisjointSets(int n) 
    { 
        // Allocate memory 
        this->n = n; 
        parent = new int[n+1]; 
        rnk = new int[n+1]; 
  
        // Initially, all vertices are in 
        // different sets and have rank 0. 
        for (int i = 0; i <= n; i++) 
        { 
            rnk[i] = 0; 
  
            //every element is parent of itself 
            parent[i] = i; 
        } 
    }
    ~DisjointSets(){
        delete[] parent;
        delete[] rnk;
    }
  
    // Find the parent of a node 'u' 
    // Path Compression 
    int find(int u) 
    { 
        /* Make the parent of the nodes in the path 
        from u--> parent[u] point to parent[u] */
        if (u != parent[u]) 
            parent[u] = find(parent[u]); 
        return parent[u]; 
    } 
  
    // Union by rank 
    void merge(int x, int y) 
    { 
        x = find(x), y = find(y); 
  
        /* Make tree with smaller height 
        a subtree of the other tree */
        if (rnk[x] > rnk[y]) 
            parent[y] = x; 
        else // If rnk[x] <= rnk[y] 
            parent[x] = y; 
  
        if (rnk[x] == rnk[y]) 
            rnk[y]++; 
    } 
}; 

std::ostream& operator<<(std::ostream& os, const Edges& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i].first << "\t" << v[i].second.first << "\t" << v[i].second.second << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const BigPartition& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << "Partition " << i << std::endl;
        int n = v[i].size();
        for(int j=0; j<n; j++){
            os << v[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const ListOfPairs& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i].first << "\t" << v[i].second << std::endl;
    }
    return os;
};

std::ostream& operator<<(std::ostream& os, const std::vector<bool>& v){
    int size = v.size();
    for (int i = 0; i < size; i++){
        os << v[i] << " ";
    }
    return os;
};