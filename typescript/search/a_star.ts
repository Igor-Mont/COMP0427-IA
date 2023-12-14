import { NOde, PriorityQueue, Problem, ToDouble } from "../utils";

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
