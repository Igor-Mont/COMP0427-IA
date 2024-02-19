// Utils genéricas.

import { readFileSync } from 'node:fs';

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

// Um Map que transforma as chaves em strings.
// Útil quando as chaves são reference types.
export class StringMap extends Map {
  get(key) {
    let strKey = JSON.stringify(key);
    return super.get(strKey);
  }

  set(key, value) {
    let strKey = JSON.stringify(key);
    return super.set(strKey, value);
  }
}

export class Range {
  constructor(from, to) {
    if (!to) {
      this.from = 0;
      this.to = from;
    } else {
      this.from = from;
      this.to = to;
    }
  }

  includes(x) {
    return this.from <= x && x <= this.to;
  }
  
  *[Symbol.iterator]() {
    for (let x = Math.ceil(this.from); x <= this.to; x++) {
      yield x;
    }
  }
}

// ----------------------------------------------------------------------------
// Utils de arrays e outros iterables.
// ----------------------------------------------------------------------------

/*
 * Return element e with maximum f(e).
 */
export function maxBy(it, f=identity) {
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
  return xs.includes(e);
}

export function sum(it) {
  let xs = [...it].reduce((acc, x) => acc + x, 0);
  return xs;
}

// True if all elements in it are true.
export function all(it) {
  return [...it].reduce((acc, x) => x && acc, true);
}

/*
 * Retorna uma cópia de seq com todas as ocorrências de item removidas.
 */
export function removeAll(item, seq) {
  if (typeof seq === 'string') {
    return seq.replaceAll(item, '');
  }
  else {
    let xs = [...seq];
    return xs.filter(x => x !== item);
  }
}

/*
 * Remove duplicatas em seq.
 */
export function unique(seq) {
  return [...new Set(seq)];
}

/*
 * Retorna o i-ésimo elemento de cada array em arrs.
 */
export function ith(i, arrs) {
  let result = [];
  for (const arr of arrs) {
    result.push(arr[i]);
  }
  return result;
}

/*
 * Replica o elemento x n vezes.
 * O parâmetro copyEach controla se uma cópia de x será feita n vezes.
 * Esse parâmetro só faz sentido se x for de reference type.
 *
 * > replicate(3, 9)
 * [9, 9, 9]
 * > replicate(4, [0, 1])
 * [[0, 1], [0, 1], [0, 1], [0, 1]]
 */
export function replicate(n, x, copyEach=false) {
  let xs = [];
  for (let i = 0; i < n; i++) {
    let item;
    if (copyEach && (typeof x === 'object')) {
      item = {...x};
    } else {
      item = x;
    }
    xs.push(item);
  }
  return xs;
}

/*
 * > zip([1, 2, 10], [3, 4], [5, 6], [7, 8, 9])
 * [[1, 3, 5, 7], [2, 4, 6, 8]]
 */
export function zip(...arrs) {
  let lengths = arrs.map(a => a.length);
  let n = Math.min(...lengths);
  let result = [];
  for (let i = 0; i < n; i++) {
    result.push(ith(i, arrs));
  }
  return result;
}

/*
 * In-place removal of first-ocurrence of x in xs.
 */
export function remove(x, xs) {
  let i = xs.indexOf(x);
  xs.splice(i, 1);
}

// ----------------------------------------------------------------------------
// Probabilidade e outras utils matemáticas.
// ----------------------------------------------------------------------------

export function identity(x) {
  return x;
}

export const argmax = maxBy;

// "Embaralha" a cópia de um iterable.
export function shuffled(it) {
  let xs = [...it];
  let n = xs.length;
  let result = [];
  for (let i = 0; i < n; i++) {
    let x = randomChoice(xs);
    remove(x, xs);
    result.push(x);
  }
  return result;
}
/*
 * Faz uma escolha aleatória-uniforme de um elemento de arr.
 */
export function randomChoice(arr) {
  const randomIndex = Math.floor(Math.random() * arr.length);
  return arr[randomIndex];
}

/*
 * Retorna true se x e y estão arbitrariamente próximos.
 */
export function isClose(x, y, eps=Number.EPSILON) {
  return Math.abs(x - y) > eps;
}

/*
 * Retorna true com probabilidade p.
 */
export function probability(p) {
  return p > Math.random();
}

/*
 * Multiplica cada número por uma constante tal que a soma é 1.
 */
export function normalize(dist) {
  let total;
  if (dist instanceof Map) {
    total = sum(dist.values());
    for (const [k, v] of dist) {
      dist.set(k, v / total);
    }
    return dist;
  }
  total = sum(dist);
  return dist.map(n => n / total);
}

// ----------------------------------------------------------------------------
// Leitura e parse de arquivos.
// ----------------------------------------------------------------------------

export function atom(x) {
  return isNan(Number(x)) ? Number(x) : String(x);
}

export function openData(name) {
  let filePath = '../data/' + name;
  return readFileSync(filePath, 'utf8');
}

export function lines(input, lineEnd='\n') {
  return input.split(lineEnd);
}

export function parseCsv(input, delim=',') {
  return lines(input)
         .map(l => l.split(delim).map(atom));
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
