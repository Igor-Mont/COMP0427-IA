import * as utils from '../utils.mjs';

/*
 * Discrete probability distribution.
 * > P = new ProbDist('Flip'); P.set('H', 0.25); P.set('T', 0.75);
 * > P.get('H')
 * 0.25
 * > P = new ProbDist('X', [['lo', 125], ['med', 375], ['hi', 500]]);
 * > ['lo', 'med', 'hi'].map(P.get);
 * [0.125, 0.375, 0.5]
 */

export class ProbDist extends Map {
  /*
   * Freqs is an array of [value, frequency] pairs.
   * Then, ProbDist is normalized.
   */
  constructor(varName='?', freqs=null) {
    super(freqs);
    this.varName = varName;
    if (freqs instanceof Array) {
      this.normalize();
    }
  }

  normalize() {
    let total = utils.sum(this.values());
    if (!utils.isClose(total, 1.0)) {
      for (const [k, v] of this) {
        this.set(k, v / total);
      }
    }
    return this;
  }
}

/*
 * A Bayesian Net of boolean-variables nodes.
 */
export class BayesNet {
  /*
   * nodeSpecs is an array of {variableName, parentsName, cpt} objects
   * following topological order (parents before children).
   */
  constructor(nodeSpecs = []) {
    this.nodes = [];
    this.variables = [];
    for (const nodeSpec of nodeSpecs) {
      this.add(nodeSpec);
    }
  }

  /*
   * Add a node to the net. Assumes that all invariants are held:
   * - All parents already present in the net.
   * - The node's variable isn't present yet.
   */
  add(node_spec) {
    const node = new BayesNode(
      node_spec.variable,
      node_spec.parents,
      node_spec.cpt
    );

    this.nodes.push(node);
    this.variables.push(node.variable);
    for (const parent of node.parents) {
      const newVariable = this.variableNode(parent);
      if (newVariable)
        newVariable.children.push(node);
    }
  } 

  /*
   * Returns the node for variable.
   */
  variableNode(variable) {
    for (const n of this.nodes) {
      if (n.variable === variable) {
        return n;
      }
    }

    return null;
  }

  /*
   * Returns the domain of variable.
   */
  variableValues(variable) {
    return [true, false];
  }
}


/*
 * Returns the values of variables in an event.
 *
 * event: map of variables to their values, or an array of values.
 * variables: array of variable names.
 */
export function eventValues(event, vars) {
  if (Array.isArray(event) && event.length === variables.length) {
    return event;
  }
  else {
    let values = [];
    for (const v of vars) {
      values.push(event.get(v));
    }
    return values;
  }
}

/*
 * A conditional probability distribution for a boolean variable.
 * Patrt of a BayesNet.
 */
export class BayesNode {
  /*
   * - variable: Variable name.
   * - parents: iterable of variable names or space-separated string.
   * - cpt: the conditional probability table. Takes one of these:
   *   - A number, the unconditional probability P(X=true).
   *   - A StringMap, { [v1, v2, ...]:p }, the conditional probability
   *   P(X=true | parent1=v1, parent2=v2, ...).
   * 
   * Example:
   * let Z = new BayesNode('Z', 'P Q', new StringMap([
   * [[T,T], 0.2], [[T,F], 0.3], [[F,T], 0.5], [[F,F], 0.7]
   * ]);
   */
  constructor(X, parents, cpt) {
    if (typeof parents === 'string') {
      parents = parents.split(' ');
    }
    if (typeof cpt === 'number') {
      cpt = new StringMap([['', cpt]]);
    }

    this.variable = X;
    this.parents = parents;
    this.cpt = cpt;
    this.children = [];
  }

  /*
   * Return the conditional probability * P(X=value | parents=parentValues).
   * parentValues come from events.
   */
  p(value, event) {
    let ptrue = this.cpt.get(eventValues(event, this.parents));
    return value ? ptrue : 1 - ptrue;
  }

  /*
   * Return the true/false according to the CPT row for the event.
   */
  sample(event) {
    let p = this.p(true, event);
    return utils.probability(p);
  }
}

function likelihood_weighting(X, e, bn, N=10000) {
  /*
     * Estimate the probability distribution of variable X given
     * evidence e in BayesNet bn.
     */
  let W = {};
  bn.variableValues(X).forEach(value => {
    W[value] = 0;
  });

  for (let j = 0; j < N; j++) {
    let sample_weight = weighted_sample(bn, e); // boldface x, w in [Figure 14.15]
    let sample = sample_weight[0];
    let weight = sample_weight[1];
    W[sample[X]] += weight;
  }

  return utils.normalize(W);
}

function weighted_sample(bn, e) {
  /*
     * Sample an event from bn that's consistent with the evidence e;
     * return the event and its weight, the likelihood that the event
     * accords to the evidence.
  */

  let w = 1;
  let event = Object.assign({}, e); // boldface x in [Figure 14.15]
  bn.nodes.forEach(node => {
    let Xi = node.variable;
    if (Xi in e) {
      w *= node.p(e[Xi], event);
    } else {
      event[Xi] = node.sample(event);
    }
  });

  return [event, w];
}

const bNode = {
  variable: "Alarm",
  parents: "Burglary Earthquake",
  cpt: new Map([
    [[true, true], 0.95],
    [[true, false], 0.94],
    [[false, true], 0.29],
    [[false, false], 0.001],
  ])
};

const johnCallsNode = {
  variable: "JohnCalls",
  parents: "Alarm",
  cpt: new Map([
    [true, 0.9],
    [false, 0.05],
  ])
};

const maryCallsNode = {
  variable: "MaryCalls",
  parents: "Alarm",
  cpt: new Map([
    [true, 0.7],
    [false, 0.01],
  ])
};

const burglaryNode = {
  variable: "Burglary",
  parents: [],
  cpt: new Map([
    [true, 0.001],
    [false, 0.999],
  ])
};

const earthquakeNode = {
  variable: "Earthquake",
  parents: [],
  cpt: new Map([
    [true, 0.002],
    [false, 0.998],
  ])
};

const bn = new BayesNet();
bn.add(bNode);
bn.add(johnCallsNode);
bn.add(maryCallsNode);
bn.add(burglaryNode);
bn.add(earthquakeNode);

console.log(bn)
