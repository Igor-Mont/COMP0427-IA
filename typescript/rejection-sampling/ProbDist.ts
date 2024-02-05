class ProbDist<T> {
  private prob: Map<T, number>;
  private values: T[];

  constructor(private var_name = "?", freq: Map<T, number> | null = null) {
    this.prob = new Map();
    this.values = [];

    if (freq) {
      const items = [...freq.entries()];
      items.forEach(([v, p]) => {
        if (!this.values.includes(v)) this.values.push(v);
        this.prob.set(v, p);
      });
      this.normalize();
    }
  }

  private normalize(): ProbDist<T> {
    const probValues = this.prob.values();
    const total = [...probValues].reduce((acc, value) => acc + value, 0);

    if (Math.abs(total - 1.0) > Number.EPSILON) {
      for (const [key, value] of this.prob) {
        this.prob.set(key, value / total);
      }
    }

    return this;
  }

  show_approx(maxDecimalCase = 3) {
    const sortedProbabilities: [T, number][] = Array.from(
      this.prob.entries()
    ).sort(([a], [b]) => (a < b ? -1 : 1));

    const formattedProbabilities: string[] = sortedProbabilities.map(
      ([v, p]) => {
        return `${v}: ${p.toFixed(maxDecimalCase)}`;
      }
    );

    return formattedProbabilities.join(", ");
  }

  getValue(key: T): number {
    const result = this.prob.get(key);
    if (!result) throw new Error("Key not found in prob.");

    return result;
  }

  show() {
    return `P(${this.var_name})`;
  }
}

// const p = new ProbDist(
//   "X",
//   new Map([
//     ["lo", 125],
//     ["med", 375],
//     ["hi", 500],
//   ])
// );
// console.log(p.show());
// console.log(p.getValue("lo"));
// console.log(p.getValue("med"));
// console.log(p.getValue("hi"));
export { ProbDist };
