import { NOde, Problem } from "../utils";

type Children<State, Action> = Map<MCT_Node<State, Action>, Action>;

class MCT_Node<State, Action> {
  children: Children<State, Action> = new Map<
    MCT_Node<State, Action>,
    Action
  >();
  actions: Action[] = [];

  constructor(
    public parent: MCT_Node<State, Action> | undefined = undefined,
    public state: State,
    public U = 0,
    public N = 0
  ) {}
}

function ucb<State, Action>(
  { parent, N, U }: MCT_Node<State, Action>,
  C = 1.4
) {
  if (N === 0) return Infinity;
  if (parent) return U / N + C * Math.sqrt(Math.log(parent.N) / N);
}

function max<State, Action>(
  nodes: MCT_Node<State, Action>[]
): MCT_Node<State, Action> {
  return nodes.reduce((maxNode, currentNode) => {
    const ucbMax = ucb(maxNode) as number;
    const ucbCurrent = ucb(currentNode) as number;
    return ucbCurrent > ucbMax ? currentNode : maxNode;
  }, nodes[0]);
}

function select<State, Action>(
  n: MCT_Node<State, Action>
): MCT_Node<State, Action> {
  return n.children ? select(max([...n.children.keys()])) : n;
}

function monte_carlo_tree_search<State, Action>(
  state: NOde<State, Action>,
  game: Problem<State, Action>,
  N = 1000
) {}
