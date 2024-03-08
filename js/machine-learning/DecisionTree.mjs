/* eslint-disable no-unused-vars */
/*
 * Types for implementing discrete decision trees.
 */

import * as utils from "../utils.mjs";

/*
 * A fork on a specific attribute, with a map of branches for each of its values.
 */
class DecisionFork {
  // Specify for which attribute this node is for.
  constructor(attr, defaultChild = null, branches = null) {
    this.attr = attr;
    this.defaultChild = defaultChild;
    this.branches = branches ?? new Map();
  }

  // Classify an example using the attribute and branches.
  classify(example) {
    let attrVal = example[this.attr];
    if (utils.isIn(attrVal, this.branches.keys())) {
      return this.branches.get(attrVal).call(example);
    } else {
      return this.defaultChild(example);
    }
  }

  // Add a branch. If self.attr = val, go to the given subtree.
  add(val, subtree) {
    this.branches.set(val, subtree);
  }
}

/*
 * A leaf of a decision tree holds just a result.
 */
class DecisionLeaf {
  constructor(result) {
    this.result = result;
  }

  call(example) {
    return this.result;
  }
}

/*
 * An agent that learns by first constructing a boolean decision tree and them
 * classifying new inputs.
 */
export class DecisionTreeLearner {
  constructor(dataset) {
    this.dataset = dataset;
    this.tree = this.buildTree(dataset.examples, dataset.inputs);
  }

  buildTree(examples, attrs, parentExamples = []) {
    if (examples.length === 0) {
      return this.pluralityValue(parentExamples);
    }
    if (this.allSameClass(examples)) {
      return new DecisionLeaf(examples[0][this.dataset.target]);
    }
    if (attrs.length === 0) {
      return this.pluralityValue(examples);
    }

    let A = this.chooseAttribute(attrs, examples);
    let tree = new DecisionFork(
      A,
      this.pluralityValue(examples)
    );

    for (const [v, e] of this.splitBy(A, examples)) {
      let subtree = this.buildTree(e, utils.removeAll(A, attrs), examples);
      tree.add(v, subtree);
    }
    return tree;
  }

  // Returns most frequent target in examples.
  pluralityValue(examples) {
    let popular = utils.argmax(this.dataset.values[this.dataset.target], (v) =>
      this.count(this.dataset.target, v, examples)
    );
    return new DecisionLeaf(popular);
  }

  // Count the number of examples that have example[attr] === val.
  count(attr, val, examples) {
    return utils.sum(examples.map((e) => e[attr] === val));
  }

  // Are all the examples in the same target class?
  allSameClass(examples) {
    let firstClass = examples[0][this.dataset.target];
    return utils.all(
      examples.map((e) => e[this.dataset.target] === firstClass)
    );
  }

  // Choose attribute with highest information gain.
  chooseAttribute(attrs, examples) {
    return utils.argmax(attrs, (a) => this.informationGain(a, examples));
  }

  // Return the expected reduction in entropy from splitting by attr.
  informationGain(attr, examples) {
    const I = (exs) => {
      let values = [];
      for (const v of this.dataset.values[this.dataset.target]) {
        let count = this.count(this.dataset.target, v, exs);
        values.push(count);
      }
      return informationContent(values);
    };

    let n = examples.length;
    let entropies = [];
    for (const [, e] of this.splitBy(attr, examples)) {
      let entropy = (e.length / n) * I(e);
      entropies.push(entropy);
    }
    let remainder = utils.sum(entropies);

    return I(examples) - remainder;
  }

  // Array of [val, examples] pairs for each val of attr.
  splitBy(attr, examples) {
    let vals = this.dataset.values[attr];
    let result = [];
    for (const v of vals) {
      let valExamples = [];
      for (const e of examples) {
        if (e[attr] === v) {
          valExamples.push(e);
        }
      }

      result.push([v, valExamples]);
    }
    return result;
  }

  call(x) {
    return this.tree.classify(x);
  }
}

function informationContent(values) {
  let probabilities = utils.normalize(utils.removeAll(0, values));
  let terms = [];
  for (const p of probabilities) {
    terms.push(-p * Math.log2(p));
  }
  return utils.sum(terms);
}
