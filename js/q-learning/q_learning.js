/*
 * ----------------------------------------------------------------------------
 * A implementação do Q-Learning de maneira razoavelmente modular requer a
 * construção de alguns tipos que modelam a categoria dos problemas que o
 * Q-Learning deve resolver, assim como instâncias dessas categorias. 
 * Esses tipos são: MDP, GridMDP e QLearningAgent.
 *
 * Algumas função auxiliares/utils também são definidas.
 * ----------------------------------------------------------------------------
 */

/*
 * ----------------------------------------------------------------------------
 * Utils.
 * ----------------------------------------------------------------------------
 */

// Converte uma string da forma 'x<sep>y<sep>z<sep>...' numa array numérica [x, y, z].
function strToArr(s, sep = ',') {
  return s.split(sep).map(Number);
}

const [EAST, NORTH, WEST, SOUTH] = [[1, 0], [0, 1], [-1, 0], [0, -1]];
const orientations = [EAST, NORTH, WEST, SOUTH];

const [LEFT, RIGHT] = [+1, -1];

function turnHeading(heading, inc, headings=orientations.map(String)) {
  return headings[(headings.indexOf(heading) + inc) % headings.length];
}

function turnRight(heading) {
  return turnHeading(heading, RIGHT);
}

function turnLeft(heading) {
  return turnHeading(heading, LEFT);
}

function vectorAdd(xs, ys) {
  if (xs.length != ys.length) {
    return null;
  }
  let sum = new Array(xs.length).fill(0);
  for (let i = 0; i < xs.length; i++) {
    sum[i] += xs[i] + ys[i];
  }
  return sum;
}

function maxBy(it, keyFunc) {
  let arr = [...it];
  if (arr.length === 0) {
    return undefined;
  }
  return arr.reduce((max, current) => (keyFunc(current) > keyFunc(max) ? current : max), arr[0]);
}


// Igual um Map, mas aceita uma factory para gerar valores default
// para keys inexistentes.
class DefaultMap extends Map {
  constructor(factory = null) {
    super();
    this.factory = factory;
  }
  
  get(key) {
    let x = super.get(key);
    if (x == undefined && this.factory != null) {
      x = this.factory();
      this.set(key, x);
    }
    return x;
  }

  inc(key, v) {
    let x = this.get(key);
    this.set(key, x + v);
  }
}


/*
 * ----------------------------------------------------------------------------
 * MDP: Modela um Markovian Decision Process (MDP), um problema de decisão
 * sequencial, estocástico (com um modelo de transição markoviano, ou seja, as
 * probabilidades não dependem do histórico de estados) e completamentamente observável.
 * ----------------------------------------------------------------------------
*/

class MDP {
  constructor(
    init,
    actlist,
    terminals,
    states,
    transitions = null,
    reward = null,
    gamma = 0.9) {
    
    this.states = states;
    this.init = init;
    this.gamma = gamma;
    this.actlist = actlist;

    // Para evitar comparação de dois objetos mutáveis.
    this.terminals = terminals.map(x => x.toString());

    // transitions DEVE ser um Map de strings, que representam estados, para
    // outros Maps. Esses outros Maps são de strings, que representam actions,
    // para Arrays de pares (p, s), onde p é uma probabilidade (number) e s é
    // uma string representando um estado.
    this.transitions = transitions || new Map();

    if (this.reward == null) {
      this.reward = new Map();
      for (const s of this.states) {
        // strings são mais apropriadas para chaves.
        this.reward.set(s.toString(), 0);
      }
    } else {
      this.reward = reward;
    }
  }

  // Retorna a reward pra esse estado.
  R(state) {
    return this.reward.get(state.toString());
  }

  // Modelo de transição. Para um estado e ação, retorna uma Array de pares
  // (propabilidade, estado resultado).
  T(state, action) {
    return action ?
      this.transitions.get(state.toString()).get(action.toString())
      : [[0.0, state.toString()]];
  }

  // Ações disponíveis no estado. Por default, a lista de ações é fixa.
  actions(state) {
    if (this.terminals.contains(state.toString())) {
      return [null];
    } else {
      return this.actlist;
    }
  }
}

/*
 * ----------------------------------------------------------------------------
 * GridMDP: O MDP usado como exemplo no AIMA, no qual é dado um grid 4x3.
 * Um obstáculo (O) existe próximo ao meio do grid, e o estado inicial (I) é o
 * (1,1): 
 * - - - W
 * - O - L
 * I - - -
 *
 * W e L são estados terminais, com rewards 1 e -1. A transição para todos os
 * outros estados tem reward -0.04. Um fator de desconto gamma também é utilizado para
 * calcular a utilidade de uma sequência de estados.
 * ----------------------------------------------------------------------------
 */

class GridMDP extends MDP {
  constructor(grid, terminals, init = [0, 0], gamma=0.9) {
    // Pois queremos row 0 na parte inferior.
    grid.reverse()
    let reward = new Map();
    let states = new Set();
    this.rows = grid.length;
    this.cols = grid[0].length;

    for (let x = 0; x < this.cols; x++) {
      for (let y = 0; y < this.rows; y++) {
        if (grid[x][y]) {
          let p = [x, y].toString();
          states.add(p)
          reward.set(p, grid[x][y]);
        }
      }
    }

    let actlist = orientations.map(String);
    let transitions = new Map();

    for (const s of states) {
      transitions.set(s, new Map());
      for (const a of actlist) {
        transitions.get(s).set(a, this.calculateT(s, a));
      }
    }

    super(init, actlist=actlist, terminals=terminals, transitions=transitions,
      reward=reward, states=states, gamma=gamma);
  }

  calculateT(state, action) {
    if (action) {
      return [
        [0.8, this.go(state, action)],
        [0.1, this.go(state, turnRight(action))],
        [0.1, this.go(state, turnLeft(action))],
      ];

    } else {
      return [[0.0, state.toString()]];
    }
  }

  // O estado resultante de ir nessa direção.
  go(state, direction) {
    let s = vectorAdd(strToArr(state), strToArr(direction));
    return s.toString();
  }
}

/*
 * ----------------------------------------------------------------------------
 * QLearningAgent: Modela um agente que faz uso do algortimo Q-Learning com
 * diferença temporal. Este não precisa aprender o modelo de transição.
 * ----------------------------------------------------------------------------
 */

class QLearningAgent {
  constructor(mdp, Ne, Rplus, alpha = (n) => 1.0 / (1 + n)) {
    this.gamma = mdp.gamma;
    this.terminals = mdp.terminals;
    this.allAct = mdp.actlist;
    this.Ne = Ne;
    this.Rplus = Rplus;
    this.Q = new DefaultMap(Number);
    this.Nsa = new DefaultMap(Number);
    this.s = null;
    this.a = null;
    this.r = null;
  }

  // Função de exploração.
  f(u, n) {
    return n < this.Ne ? this.Rplus : u;
  }

  // Ações disponíveis no estado.
  actionsInState(state) {
    if (this.terminals.contains(state.toString())) {
      return [null];
    } else {
      return this.actlist;
    }
  }

  call(percept) {
    let [s1, r1] = updateState(percept);
    let [Q, Nsa, s, a, r] = [this.Q, this.Nsa, this.s, this.a, this.r];
    let actionsInState = this.actionsInState;

    if (terminals.includes(s.toString())) {
      Q.set([s,null].toString(), r1);
    }

    if (s != null) {
      let k = [s, a].toString();

      Nsa.inc(k, 1);

      let max = -Infinity;
      for (const a1 of actionsInState(s1)) {
        let x = Q.get([s1, a1].toString());
        max = Math.max(max, x);
      }
      Q.inc(k, alpha(Nsa.get(k) * (r + gamma * max - Q.get(k))));
    }

    if (terminals.includes(s.toString())) {
      this.s = this.a = this.r = null;
    } else {
      [this.s, this.r] = s1, r1;
      let k = [s1, r1].toString();
      this.a = maxBy(actionsInState(s1), a1 => this.f(Q.get(k), Ns.get(k)));
    }
    return this.a;
  }

  updateState(percept) {
    return percept;
  }
}

/*
 * ----------------------------------------------------------------------------
 * Testes.
 * ----------------------------------------------------------------------------
 */
