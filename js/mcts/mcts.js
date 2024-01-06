"use strict";

// ----------------------------------------------------------------------------
// Utils.

class MCTNode {
  static lastAssignedId = 0;

  constructor(state = null, parent = null, U = 0, N = 0) {
    this.parent = parent;
    this.state = state;
    this.U = U;
    this.N = N;
    this.children = new Map();
    this.actions = null;
    MCTNode.lastAssignedId += 1;
    this.id = MCTNode.lastAssignedId;
  }
}

class GameState {
  constructor(to_move, utility, board = new Map(), moves = []) {
    this.to_move = to_move;
    this.utility = utility;
    // This will be a Map from stringified 2D indexes to strings representing the player.
    this.board = board;
    this.moves = moves;
  }
}

function ucb(n, C = 1.4) {
  return n.N == 0 ? Infinity : n.U / n.N + C * Math.sqrt(Math.log(n.parent.N) / n.N)
}

function isEmpty(it) {
  for (const i of it) return false;
  return true;
}

function maxBy(it, keyFunc) {
  let arr = [...it];
  if (arr.length === 0) {
    return undefined;
  }
  return arr.reduce((max, current) => (keyFunc(current) > keyFunc(max) ? current : max), arr[0]);
}

// Checks if an array is in an iterable. This assumes the iterable has arrays as
// its elements.
function isIn(arr, iterable) {
  for (const x of iterable) {
    if (x.every((val, idx) => val === arr[idx])) {
      return true;
    }
  }
  return false;
}

function randomChoice(arr) {
  if (arr.length === 0) {
    return undefined;
  }

  const randomIndex = Math.floor(Math.random() * arr.length);
  return arr[randomIndex];
}

// ----------------------------------------------------------------------------
// Monte Carlo Tree Search.

function monteCarloTreeSearch(state, game, N = 1000) {
  const select = (n) => {
    if (!isEmpty(n.children))
      return select(maxBy(n.children.keys(), ucb));
    return n;
  };

  const expand = (n) => {
    if (isEmpty(n.children) && !game.terminal_test(n.state)) {
      n.children = new Map();
      game.actions(n.state).forEach((action) => {
        const newNode = new MCTNode(game.result(n.state, action), n);
        n.children.set(newNode, action);
      })
    }
    return select(n);
  };

  const simulate = (game, state) => {
    const player = game.to_move(state);
    while (!game.terminal_test(state)) {
      const action = randomChoice(game.actions(state));
      state = game.result(state, action);
    }
    const v = game.utility(state, player);
    return -v;
  };

  const backprop = (n, utility) => {
    if (utility > 0)
      n.U += utility;
    n.N += 1;
    if (n.parent != null)
      // Negates because the tree is alternated.
      backprop(n.parent, -utility);
  };

  let root = new MCTNode(state);

  for (let i = 0; i < N; i++) {
    const leaf = select(root);
    const child = expand(leaf);
    const result = simulate(game, child.state);
    backprop(child, result);
  }

  const max_state = maxBy(root.children.keys(), (p) => p.N);
  return root.children.get(max_state);
}

// ----------------------------------------------------------------------------
// Games.

/*
 * Interface for an abstract game.
 */
class Game {
  actions(state) {
    return undefined;
  }

  result(state, move) {
    return undefined;
  }

  utility(state, player) {
    return undefined;
  }

  terminal_test(state) {
    return this.actions(state).length == 0;
  }

  to_move(state) {
    return state.to_move;
  }
}

class TicTacToe extends Game {
  constructor(h = 3, v = 3, k = 3) {
    super();
    this.h = h;
    this.v = v;
    this.k = k;
    
    const moves = [];
    for (let x = 1; x <= h; x++) {
      for (let y = 1; y <= v; y++) {
        moves.push([x, y]);
      }
    }

    this.initial = new GameState('X', 0, new Map(), moves);
  }

  actions(state) {
    return state.moves;
  }

  result(state, move) {
    if (!state.moves.some(el => el[0] == move[0] && el[1] == move[1])) // Intankavel o JavaScript
      return state;
    const board = new Map(state.board);
    board.set(move.toString(), state.to_move);
    let moves = Array.from(state.moves);
    moves = moves.filter(x => !x.every((val, idx) => val == move[idx]));
    return new GameState(
      state.to_move === 'X' ? 'O' : 'X',
      this.compute_utility(board, move, state.to_move),
      board,
      moves
    );
  }

  utility(state, player) {
    return player === 'X' ? state.utility : -state.utility;
  }

  terminal_test(state) {
    return state.utility != 0  || state.moves.length === 0;
  }

  compute_utility(board, move, player) {
    if (this.k_in_row(board, move, player, [0, 1]) ||
        this.k_in_row(board, move, player, [1, 0]) ||
        this.k_in_row(board, move, player, [1,-1]) ||
        this.k_in_row(board, move, player, [1, 1]))
      return player === 'X' ? 1 : -1;
    else
      return 0
  }

  k_in_row(board, move, player, delta_x_y) {
    const [delta_x, delta_y] = delta_x_y;
    let [x, y] = move;
    let n = 0;
    while (board.get([x, y].toString()) == player) {
      n += 1;
      [x, y] = [x + delta_x, y + delta_y];
    }
    [x, y] = move;
    while (board.get([x, y].toString()) == player) {
      n += 1;
      [x, y] = [x - delta_x, y - delta_y];
    }
    n -= 1
    return n >= this.k;
  }
}

// Tests

const ttt = new TicTacToe();

function gen_state(to_move = 'X', x_positions = [], o_positions = [], h = 3, v = 3) {
  let moves = []
  for (let x = 1; x <= h; x++) {
    for (let y = 1; y <= v; y++) {
      moves.push([x, y]);
    }
  }
  const xSet = new Set(x_positions);
  const oSet = new Set(o_positions);
  moves = moves.filter(pair => !isIn(pair, xSet) && !isIn(pair, oSet));
  const board = new Map();
  for (const pos of x_positions)
    board.set(pos.toString(), 'X');
  for (const pos of o_positions)
    board.set(pos.toString(), 'O');
  return new GameState(to_move, 0, board, moves);
}

function test_monte_carlo_tree_search() {
  // Plays on middle if it guarantees a win.
  let state = gen_state(
    'X',
    [[1, 1], [3, 3]], // X positions.
    [[1, 2], [3, 2]]  // O positions.
  );
  let result = monteCarloTreeSearch(state, ttt).toString();
  console.log(result == '2,2');

  // Plays on either position to win.
  state = gen_state(
    'X',
    [[1,1], [3, 3], [3, 1]],
    [[1,2], [3, 2]]
  );
  result = monteCarloTreeSearch(state, ttt).toString();
  console.log(['2,2', '2,1'].includes(result));

  // Plays to avoid enemy's win.
  state = gen_state(
    'X',
    [[1, 1]],
    [[1, 2], [3, 2]]
  );
  result = monteCarloTreeSearch(state, ttt).toString();
  console.log(result == '2,2');
}

test_monte_carlo_tree_search();
