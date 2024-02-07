/*
 * A Bayesian Net of boolean-variables nodes.
 */
class BayesNet {
  /*
   * node_specs is an array of
   * {variableName, parentsName, cpt} objects.
   */
  constructor(node_specs = []) {
    this.nodes = [];
    this.variables = [];
    for (const node_spec of node_specs) {
      this.add(node_spec);
    }
  }

  /*
   * Add a node to the net. Assumes that all invariants are held:
   * - All parents already present in the net.
   * - The node's variable isn't present yet.
   */
  add(node_spec) {
    node = new BayesNode(
      node_spec.variableName,
      node_spec.parentsName,
      node_spec.cpt
    );

    this.nodes.push(node);
    this.variables.push(node.variable);
    for (const parent of node.parents) {
      this.variableNode(parent).children.append(node);
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

class BayesNode {
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
    if (typeof cpt === 'number') {
      cpt = new Map([['', cpt]]);
    }
  }
}
