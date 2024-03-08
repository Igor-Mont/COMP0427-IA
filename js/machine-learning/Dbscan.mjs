/*
 * Types for implementing DBSCAN.
 */

import * as utils from "../utils.mjs";


export class Dbscan {
  constructor(dataset, epsilon = 1.0, minPts = 5) {
    this.dataset = dataset;
    this.epsilon = epsilon;
    this.minPts = minPts;
    this.labels = [];
  }

  train() {
    let visited = new Set();
    let clusters = [];

    for (const x of this.dataset.examples) {
      if (visited.has(x))
        continue;
      let neighbors = this.rangeQuery(x, this.epsilon);
      if (neighbors.length >= this.minPts) {
        let cluster = this.expandCluster(x, neighbors, visited, this.epsilon, this.minPts);
        clusters.push(cluster);
      }
    }

    return clusters;
  }

  rangeQuery(x, eps) {
    let neighborhood = [];
    for (const y of this.dataset.examples)
      if (utils.euclideanDist(x, y) < eps)
        neighborhood.push(y);
    return neighborhood;
  }

  expandCluster(x, neighbors, visited, eps, minPts) {
    utils.union(visited, neighbors);
    for (const y of neighbors) {
      if (visited.has(y))
        continue;
      if (utils.euclideanDist(x, y) < eps)
        this.expandCluster(y, this.rangeQuery(y, eps), visited, eps, minPts);
    }
    return [x, ...neighbors];
  }
}

