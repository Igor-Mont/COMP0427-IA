import * as utils from '../utils.mjs';

/*
 * A dataset for a machine learning problem.
 */
export class DataSet {
  constructor({examples=null, attrs=null, attrNames=null, name='', target=null, inputs=null, exclude=[]}) {
    this.name = name;

    // Initialize examples from string/list/data dir.
    if (typeof examples === 'string') {
      this.examples = utils.parseCsv(examples);
    }
    else if (!examples) {
      let csvData = utils.openData(name + '.csv');
      this.examples = utils.parseCsv(csvData);
    }
    else if (examples instanceof Array) {
      this.examples = examples;
    }

    // attrs defaults to indices of examples.
    if (this.examples && !attrs) {
      attrs = [...utils.Range(this.examples[0].length)];
    }
    this.attrs = attrs;

    // Initialize attrNames from string/list.
    if (typeof attrNames === 'string') {
      this.attrNames = attrNames.split();
    } else {
      this.attrNames = attrNames ?? attrs;
    }

    // Target defaults to last field.
    if (target === null) {
      target = this.examples.length - 1;
    }
    this.setProblem(target, inputs, exclude);
  }

  /*
   * Set the target and/or inputs.
   * inputs is a list of attributes.
   * exclude is a list of attributes to not be used on inputs.
   */
  setProblem(target, inputs=null, exclude=[]) {
    this.target = this.attrNum(target);
    exclude = exlude.map(this.attrNum);
    if (inputs) {
      this.inputs = utils.removeAll(this.target, inputs);
    } else {
      this.inputs = this.attrs.filter(a => a != this.target && !exclude.includes(a));
    }

    if (!this.values) {
      this.updateValues();
    }
  }

  addExample(example) {
    this.examples.push(example);
  }

  // Returns the number used for attr, which is either a string or number.
  attrNum(attr) {
    if (typeof attr === 'string') {
      return this.attrNames.indexOf(attr);
    }
    else if (attr < 0) {
      return this.attrs.length + attr;
    }
    else {
      return attr;
    }
  }

  updateValues() {
    this.values = utils.zip(...this.examples).map(utils.unique);
  }
}
