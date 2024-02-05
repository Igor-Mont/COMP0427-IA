import { BayesNet } from "../BayesNet";

function prior_sample(bn: BayesNet): Map<string, boolean> {
  const event = new Map<string, boolean>();

  bn.nodes.forEach((node) => {
    event.set(node.variable, node.sample(event));
  });

  return event;
}

export { prior_sample };
