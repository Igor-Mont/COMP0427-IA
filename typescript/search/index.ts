abstract class Problem<State, Action> {
  constructor(private initial: State, private goal: State) {}

  abstract actions(state: State): Action[];
  abstract result(state: State, action: Action): State;
  abstract goal_test(state: State): boolean;
  abstract path_cost(
    currentCost: number,
    from: State,
    action: Action,
    to: State
  ): number;

  get initialState(): State {
    return this.initialState;
  }
}

// NOde porque Node é uma palabra reservada do JS
// Ao aplicar ver formas de fazer algo parecido com o __hash__ do python.
// Possível solução: criar um set personalizado com os hashs codes e fazendo a verificação.
class NOde<State, Action> {
  private depth = 0;

  constructor(
    private _state: State,
    private _parent: NOde<State, Action> | null = null,
    private _action: Action | null = null,
    private _pathCost = 0
  ) {
    if (_parent) this.depth = _parent.depth + 1;
  }

  isSmallerThan(other: NOde<State, Action>): boolean {
    return this.state < other.state;
  }

  isEqualTo(other: NOde<State, Action>): boolean {
    return other instanceof NOde && this.state === other.state;
  }

  expand(problem: Problem<State, Action>): NOde<State, Action>[] {
    return problem
      .actions(this.state)
      .map((action: Action) => this.childNode(problem, action));
  }

  solution(): Action[] {
    return this.path()
      .slice(1)
      .filter(Boolean)
      .map((node) => node.action as Action);
  }

  private path(): NOde<State, Action>[] {
    let node: NOde<State, Action> = this;
    const pathBack = [];
    while (node) {
      pathBack.push(node);
      node = node.parent as NOde<State, Action>;
    }

    return pathBack.toReversed();
  }

  private childNode(
    problem: Problem<State, Action>,
    action: Action
  ): NOde<State, Action> {
    const nextState = problem.result(this.state, action);
    const nextNode = new NOde(
      nextState,
      this,
      action,
      problem.path_cost(this._pathCost, this.state, action, nextState)
    );

    return nextNode;
  }

  get state() {
    return this._state;
  }

  get action() {
    return this._action;
  }

  get parent() {
    return this._parent;
  }
}

interface KeyPair<S, A> {
  key: number;
  value: NOde<S, A>;
}

type ToDouble<S, A> = (node: NOde<S, A>) => number;

class PriorityQueue<S, A> {
  private pq: KeyPair<S, A>[] = [];

  constructor(private items: NOde<S, A>[], private f: ToDouble<S, A>) {
    for (const item of items) {
      this.push(item);
    }
  }

  push(item: NOde<S, A>): void {
    const newElem: KeyPair<S, A> = { key: this.f(item), value: item };
    this.pq.push(newElem);
  }

  pop(): NOde<S, A> | undefined {
    const item = this.pq.pop();
    return item ? item.value : undefined;
  }

  top(): NOde<S, A> | undefined {
    return this.pq.length > 0 ? this.pq[0].value : undefined;
  }

  get length(): number {
    return this.pq.length;
  }

  get isEmpty(): boolean {
    return this.pq.length === 0;
  }
}

function best_first_search<State, Action>(
  problem: Problem<State, Action>,
  f: ToDouble<State, Action>
): NOde<State, Action> | undefined {
  let node: NOde<State, Action> | undefined = new NOde(problem.initialState);
  const frontier = new PriorityQueue<State, Action>([node], f);
  const reached = new Map<State, NOde<State, Action>>([
    [problem.initialState, node],
  ]);

  while (!frontier.isEmpty) {
    node = frontier.pop() as NOde<State, Action>;
    if (problem.goal_test(node.state)) return node;

    for (const child of node.expand(problem)) {
      const s = child.state;
      const contains = reached.get(s);
      if (!contains) {
        reached.set(s, child);
        frontier.push(child);
      }
    }
  }
}
