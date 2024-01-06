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

class Game<State, Action, Player> extends Problem<State, Action> {
  actions(state: State): Action[] {
    throw new Error("Method not implemented.");
  }
  result(state: State, action: Action): State {
    throw new Error("Method not implemented.");
  }
  goal_test(state: State): boolean {
    throw new Error("Method not implemented.");
  }
  path_cost(
    currentCost: number,
    from: State,
    action: Action,
    to: State
  ): number {
    throw new Error("Method not implemented.");
  }

  terminal_test(state: State): boolean {
    throw new Error("Method not implemented.");
  }

  to_move(state: State): Player {
    throw new Error("Method not implemented.");
  }

  utility(state: State, player: Player): Player {
    throw new Error("Method not implemented.");
  }
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
  return nodes.reduce((max_node, current_node) => {
    const ucb_max = ucb(max_node) as number;
    const ucb_current = ucb(current_node) as number;
    return ucb_current > ucb_max ? current_node : max_node;
  }, nodes[0]);
}

function select<State, Action>(
  n: MCT_Node<State, Action>
): MCT_Node<State, Action> {
  return n.children ? select(max([...n.children.keys()])) : n;
}

function expand<State, Action, Player>(
  n: MCT_Node<State, Action>,
  game: Game<State, Action, Player>
) {
  if (!n.children.size && !game.terminal_test(n.state)) {
    game.actions(n.state).forEach((action) => {
      const newNode = new MCT_Node(n, game.result(n.state, action));
      n.children.set(newNode, action);
    });
  }

  return select(n);
}

function randomChoice<T>(list: T[]) {
  const randomIndex = Math.floor(Math.random() * list.length);
  return list[randomIndex];
}

function simulate<State, Action, Player>(
  game: Game<State, Action, Player>,
  state: State
) {
  const player = game.to_move(state);
  while (!game.terminal_test(state)) {
    const action = randomChoice(game.actions(state));
    state = game.result(state, action);
  }
  const v = game.utility(state, player);
  return -v;
}

function monte_carlo_tree_search<State, Action>(
  state: NOde<State, Action>,
  game: Problem<State, Action>,
  N = 1000
) {}
