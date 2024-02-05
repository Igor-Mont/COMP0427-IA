import { BayesNode } from "./BayesNode";

type MapCpt = Map<boolean[], number>;
type MapCptInput = Map<boolean, number> | MapCpt;
type Cpt = number | MapCptInput;

interface NodeSpec {
  variable: string;
  parents: string | string[];
  cpt: Cpt;
}

class BayesNet {
  nodes: BayesNode[] = [];
  variables: string[] = [];

  constructor(node_specs: NodeSpec[] | null = null) {
    node_specs = node_specs || [];

    node_specs.forEach((node_spec) => {
      this.add(node_spec);
    });
  }

  private add({ variable, parents, cpt }: NodeSpec) {
    const node = new BayesNode(variable, parents, cpt);
    if (this.variables.includes(variable)) {
      throw new Error("Assertion Failed: Variable is already in variables.");
    }
    if (!node.parents.every((parent) => this.variables.includes(parent))) {
      throw new Error("Assertion Failed: Some parent is not in variables.");
    }

    this.nodes.push(node);
    this.variables.push(node.variable);

    node.parents.forEach((parent) => {
      this.variable_node(parent).children.push(node);
    });
  }

  private variable_node(parent: string): BayesNode {
    const parentNode = this.nodes.find((node) => node.variable === parent);

    if (!parentNode) throw new Error("Assertion Failed: Parent not found.");

    return parentNode;
  }

  variable_values(variable?: string): boolean[] {
    return [true, false];
  }

  show() {
    return `BayesNet(${this.nodes.map((node) => node.show()).join(", ")})`;
  }
}

const burglary = new BayesNet([
  { variable: "Burglary", parents: "", cpt: 0.001 },
  { variable: "Earthquake", parents: "", cpt: 0.002 },
  {
    variable: "Alarm",
    parents: "Burglary Earthquake",
    cpt: new Map([
      [[true, true], 0.95],
      [[true, false], 0.94],
      [[false, true], 0.29],
      [[false, false], 0.001],
    ]),
  },
  {
    variable: "JohnCalls",
    parents: "Alarm",
    cpt: new Map([
      [true, 0.9],
      [false, 0.05],
    ]),
  },
  {
    variable: "MaryCalls",
    parents: "Alarm",
    cpt: new Map([
      [true, 0.7],
      [false, 0.01],
    ]),
  },
]);

export { BayesNet };
