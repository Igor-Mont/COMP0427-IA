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

export { Problem };
