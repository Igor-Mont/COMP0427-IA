import { Dbscan } from "./Dbscan.mjs";
import { DataSet } from "./DataSet.mjs";

// Testing hyperparams combinations.
function exploreHyperParams() {
  let iris = new DataSet({ name: "iris" });
  let results = [];
  for (let eps = 0.2; eps < 2.5; eps += 0.05) {
    for (let minPts = 1; minPts < 8; minPts++) {
      let dbscan = new Dbscan(iris, eps, minPts);
      let clusters = dbscan.train();
      let result = {
        eps,
        minPts,
        clusters: clusters.length
      };
      results.push(result);
    }
  }

  console.table(results);
}

function testIris() {
  let iris = new DataSet({ name: "iris" });
  let [eps, minPts] = [2, 4];
  let clusters = new Dbscan(iris, eps, minPts).train();
  // Should identify 4 clusters: 3 dominated by each of the species,
  // and another of mixed versicolor/virginica.
  console.log(clusters);
}


function testSimple() {
  // Should detect two clusters:
  // 1 - [ [ 1, 2 ], [ 1, 2 ], [ 2, 2 ], [ 2, 3 ] ]
  // 2 - [ [ 8, 7 ], [ 8, 7 ], [ 8, 8 ] ]
  // The last point is an outlier.
  let X = new DataSet({ examples: [[1, 2], [2, 2], [2, 3], [8, 7], [8, 8], [25, 80]]});
  let [eps, minPts] = [3, 2];
  let clusters = new Dbscan(X, eps, minPts).train();
  console.log(clusters);
}

testIris();
testSimple();
