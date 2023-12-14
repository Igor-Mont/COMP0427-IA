// NOde porque Node é uma palabra reservada do JS
// Ao aplicar ver formas de fazer algo parecido com o __hash__ do python.

import { Problem } from "./Problem";

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
    const pathBack: NOde<State, Action>[] = [];
    while (node) {
      pathBack.push(node);
      node = node.parent as NOde<State, Action>;
    }

    pathBack.reverse();
    return pathBack;
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

export { NOde };
