import * as utils from "../utils.mjs";

/*
 * A dataset for a machine learning problem.
 */
export class DataSet {
  constructor({
    examples = null,
    attrs = null,
    name = "",
    target = -1,
    inputs = null,
  }) {
    this.name = name;

    // Initialize examples from string/list/data dir.
    if (typeof examples === "string") {
      this.examples = utils.parseCsv(examples);
    } else if (!examples) {
      const csvData = utils.openData(name + ".csv");
      this.examples = utils.parseCsv(csvData);
    } else if (examples instanceof Array) {
      this.examples = examples;
    }

    // attrs defaults to indices of examples.
    if (this.examples && !attrs) {
      attrs = [...new utils.Range(this.examples[0].length)];
    }
    this.attrs = attrs;

    this.setProblem(target, inputs);
  }

  /*
   * Set the target and/or inputs.
   * inputs is a list of attributes.
   * exclude is a list of attributes to not be used on inputs.
   */
  setProblem(target, inputs = null) {
    this.target = this.attrNum(target);
    if (inputs) {
      this.inputs = utils.removeAll(this.target, inputs);
    } else {
      this.inputs = this.attrs.filter(a => a != this.target);
    }

    if (!this.values) {
      this.updateValues();
    }
  }

  // Returns the number used for attr, which is either a string or number.
  attrNum(attr) {
    if (typeof attr === "string") {
      return this.attrNames.indexOf(attr);
    } else if (attr < 0) {
      return this.attrs.length + attr;
    } else {
      return attr;
    }
  }

  updateValues() {
    this.values = utils.zip(...this.examples).map(utils.unique);
  }
}
