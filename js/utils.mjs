// Utils genéricas.

// ----------------------------------------------------------------------------
// Estruturas de dados.
// ----------------------------------------------------------------------------

// Igual um Map, mas aceita uma factory para gerar valores default
// para keys inexistentes.
export class DefaultMap extends Map {
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

// ----------------------------------------------------------------------------
// Funções sobre arrays.
// ----------------------------------------------------------------------------

export function maxBy(it, f) {
  let arr = [...it];
  if (arr.length === 0) {
    return undefined;
  }
  const max = (acc, x) => f(x) > f(acc) ? x : acc;
  return arr.reduce(max, arr[0]);
}

export function isEmpty(it) {
  for (const i of it) return false;
  return true;
}

export function vectorAdd(xs, ys) {
  if (xs.length != ys.length) {
    return null;
  }
  let sum = new Array(xs.length).fill(0);
  for (let i = 0; i < xs.length; i++) {
    sum[i] += xs[i] + ys[i];
  }
  return sum;
}

/*
 * Verifica se arr é um elemento de it.
 * Faz comparação por valor, não por referência
 *
 * > isIn([[1, 2], [3, 4]], [3, 4])
 * true
 * > isIn([[1, 2], [3, 4]], [3, 5])
 * false
 */
export function isIn(arr, it) {
  let xs = [...it].map((x) => JSON.stringify(x));
  let e = JSON.stringify(arr);
  return xs.indexOf(e) !== -1;
}

/*
 * Faz uma escolha aleatória-uniforme de um elemento de arr.
 */
export function randomChoice(arr) {
  const randomIndex = Math.floor(Math.random() * arr.length);
  return arr[randomIndex];
}

// ----------------------------------------------------------------------------
// Orientações e movimentações.
// ----------------------------------------------------------------------------

export const [EAST, NORTH, WEST, SOUTH] = [[1, 0], [0, 1], [-1, 0], [0, -1]];
export const orientations = [EAST, NORTH, WEST, SOUTH];

export const [LEFT, RIGHT] = [+1, -1];

export function turnHeading(heading, inc, headings=orientations.map(String)) {
  let idx = headings.indexOf(heading) + inc;
  if (idx == -1) {
    idx = headings.length;
  }
  idx = idx % headings.length;
  return headings[idx];
}

export function turnRight(heading) {
  return turnHeading(heading, RIGHT);
}

export function turnLeft(heading) {
  return turnHeading(heading, LEFT);
}

// ----------------------------------------------------------------------------
// Funções estranhas de conversão arrays <-> strings.
// ----------------------------------------------------------------------------

// Converte uma string da forma 'x<sep>y<sep>z<sep>...' numa array numérica [x, y, z].
export function strToArr(s, sep=',') {
  return s.split(sep).map(Number);
}
