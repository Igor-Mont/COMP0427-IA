import {  DecisionTreeLearner } from "./DecisionTree.mjs";
import { DataSet } from "./DataSet.mjs";

function testDecisionTreeLearner() {
  let iris = new DataSet({name: "iris"});
  let dtl = new DecisionTreeLearner(iris);
  console.assert(dtl.call([5, 3, 1, 0.1]) === "setosa", "Failed to classify setosa.");
  console.assert(dtl.call([6, 5, 3, 1.5]) === "versicolor", "Failed to classify versicolor.");
  console.assert(dtl.call([7.5, 4, 6, 2]) === "virginica", "Failed to classify virginica.");
}

testDecisionTreeLearner();
