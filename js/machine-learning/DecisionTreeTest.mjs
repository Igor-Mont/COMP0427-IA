import { DecisionTreeLearner } from "./DecisionTree.mjs";
import { DataSet } from "./DataSet.mjs";

function testDecisionTreeLearner() {
  let iris = new DataSet({ name: "iris" });
  let dtl = new DecisionTreeLearner(iris);
  console.log(dtl.call([5, 3, 1, 0.1]) === "setosa");
  console.log(dtl.call([6, 5, 3, 1.5]) === "versicolor");
  console.log(dtl.call([7.5, 4, 6, 2]) === "virginica");
}

testDecisionTreeLearner();
