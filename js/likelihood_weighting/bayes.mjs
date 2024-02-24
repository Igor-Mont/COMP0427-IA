import * as utils from "../utils.mjs";

export class ProbDist {
  constructor(var_name = "?", freq = null) {
    this.prob = new Map();
    this.values = [];

    if (freq) {
      const items = [...freq.entries()];
      items.forEach(([v, p]) => {
        if (!this.values.includes(v)) this.values.push(v);
        this.prob.set(v, p);
      });
      this.normalize();
    }
  }

  normalize() {
    const probValues = this.prob.values();
    const total = [...probValues].reduce((acc, value) => acc + value, 0);

    if (Math.abs(total - 1.0) > Number.EPSILON) {
      for (const [key, value] of this.prob) {
        this.prob.set(key, value / total);
      }
    }

    return this;
  }

  show_approx() {
    const sortedProbabilities = Array.from(this.prob.entries()).sort(
      ([a], [b]) => (a < b ? -1 : 1)
    );

    const formattedProbabilities = sortedProbabilities.map(([v, p]) => {
      return [v, p];
    });

    return formattedProbabilities;
  }
}

/*
 * A Bayesian Net of boolean-variables nodes.
 */
export class BayesNet {
  constructor(nodeSpecs = []) {
    this.nodes = [];
    this.variables = [];
    for (const nodeSpec of nodeSpecs) {
      this.add(nodeSpec);
    }
  }

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
      if (newVariable) newVariable.children.push(node);
    }
  }

  variableNode(variable) {
    for (const n of this.nodes) {
      if (n.variable === variable) {
        return n;
      }
    }

    return null;
  }

  variableValues(variable) {
    return ["true ", "false "];
  }
}

export function eventValues(event, vars) {
  if (Array.isArray(event) && event.length === vars.length) {
    return event;
  } else {
    let values = "";
    for (const v of vars) {
      values = values + event[v];
    }
    return values;
  }
}

export class BayesNode {
  constructor(X, parents, cpt) {
    if (typeof parents === "string") {
      parents = parents.split(" ");
    }

    if (typeof cpt === "number") {
      cpt = { "": cpt };
    }

    this.variable = X;
    this.parents = parents;
    this.cpt = cpt;
    this.children = [];
  }

  p(value, event) {
    let resultado = eventValues(event, this.parents);
    if (resultado.length == 0) resultado = "";

    let ptrue = this.cpt[resultado];
    return value ? ptrue : 1 - ptrue;
  }

  sample(event) {
    let p = this.p(true, event);
    return utils.probability(p);
  }
}

function likelihood_weighting(X, e, bn, N = 10000) {
  let W = {};

  bn.variableValues(X).forEach((value) => {
    W[value] = 0;
  });

  for (let j = 0; j < N; j++) {
    let [sample, weight] = weighted_sample(bn, e);
    W[sample[X]] += weight;
  }

  return utils.normalize(W);
}

function weighted_sample(bn, e) {
  let w = 1;
  let event = Object.assign({}, e);

  bn.nodes.forEach((node) => {
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
  cpt: {
    "true true ": 0.95,
    "true false ": 0.94,
    "false true ": 0.29,
    "false false ": 0.001,
  },
};

const johnCallsNode = {
  variable: "JohnCalls",
  parents: "Alarm",
  cpt: { "true ": 0.9, "false ": 0.05 },
};

const maryCallsNode = {
  variable: "MaryCalls",
  parents: "Alarm",
  cpt: {
    "true ": 0.7,
    "false ": 0.01,
  },
};

const burglaryNode = {
  variable: "Burglary",
  parents: [],
  cpt: 0.001,
};

const earthquakeNode = {
  variable: "Earthquake",
  parents: [],
  cpt: 0.002,
};

const bn = new BayesNet();
bn.add(bNode);
bn.add(johnCallsNode);
bn.add(maryCallsNode);
bn.add(burglaryNode);
bn.add(earthquakeNode);

const X = "Alarm";
const e = {
  Burglary: "false ",
  Earthquake: "false ",
  MaryCalls: "false ",
  JohnCalls: "false ",
};
const resultado = likelihood_weighting(X, e, bn, 1000000);
console.log("true:", resultado[0], ",false:", resultado[1]);
