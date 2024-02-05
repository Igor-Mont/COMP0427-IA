import { event_values } from "./utils/event_values";
import { probability } from "./utils/probability";

type MapCpt = Map<boolean[], number>;
type MapCptInput = Map<boolean, number> | MapCpt;
type Cpt = number | MapCptInput;
type ParentValues = Map<string, boolean>;

class BayesNode {
  parents!: string[];
  cpt!: MapCpt;
  variable!: string;
  children: BayesNode[] = [];

  // Use cpt: number when it has no parents
  constructor(X: string, parents: string | string[], cpt: Cpt) {
    if (typeof parents === "string")
      parents = parents ? parents.split(" ") : [];

    if (typeof cpt === "number") {
      cpt = new Map<boolean[], number>([[[], cpt as number]]);
    } else if (cpt instanceof Map) {
      const cptMap = cpt as MapCpt;
      const firstKey = [...cptMap.keys()][0];
      if (cpt && typeof firstKey === "boolean") {
        const entries = cptMap.entries();
        cpt = new Map(
          [...entries].map(([v, p]) => [Array.isArray(v) ? v : [v], p])
        );
      }
    }

    this.validateBayesNodeInput(cpt, parents);

    this.variable = X;
    this.parents = parents;
    this.cpt = cpt as MapCpt;
  }

  p(value: boolean, event: ParentValues): number {
    if (typeof value !== "boolean") {
      throw new Error(
        "Assertion Failed: Parameter 'value' must be of type boolean."
      );
    }

    // O TypeScript usa a igualdade estrita (===) ao comparar chaves em um Map. Quando você tenta chamar m.get([false]), está criando um novo array [false], e esse array não é considerado igual ao array usado como chave ao criar o Map.
    console.log(this.cpt);
    const key = event_values(event, this.parents) as boolean[];
    const pTrue = this.getPTrueInCptMap(key);

    return value ? pTrue : 1 - pTrue;
  }

  sample(event: ParentValues) {
    return probability(this.p(true, event));
  }

  private getPTrueInCptMap(keyToBeSearched: boolean[]): number {
    let p: number = -1;
    for (const [key, value] of this.cpt) {
      if (key.every((keyArr, i) => keyArr === keyToBeSearched[i])) {
        p = value;
        break;
      }
    }

    if (p >= 0) return p;

    throw new Error("Key in cpt not found.");
  }

  show() {
    return `(${this.variable}, ${this.parents.join(" ")})`;
  }

  private validateBayesNodeInput(cpt: Cpt, parents: string[]) {
    if (!(cpt instanceof Map))
      throw new Error("Assertion Failed: Parameter cpt in invalid format.");
    [...cpt.entries()].forEach(([vs, p]) => {
      console.log({ vs });
      if (!(Array.isArray(vs) && vs.length === parents.length)) {
        throw new Error(
          "Assertion failed: Tuple length must match the number of parents."
        );
      }
      if (!vs.every((v) => typeof v === "boolean")) {
        throw new Error(
          "Assertion failed: All elements in the tuple must be booleans."
        );
      }

      if (typeof p !== "number" || p < 0 || p > 1) {
        throw new Error(
          "Assertion failed: Probability must be a number between 0 and 1"
        );
      }
    });
  }
}

const node1 = new BayesNode(
  "X",
  "Burglary",
  new Map([
    [[true], 0.2],
    [[false], 0.625],
  ])
);

console.log(
  node1.p(
    false,
    new Map([
      ["Burglary", false],
      ["Earthquake", true],
    ])
  )
);
console.log(node1.show());

export { BayesNode };
