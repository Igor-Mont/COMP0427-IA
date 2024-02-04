type Freq = Record<string, number> | null;

class ProbDist {
  private prob: Map<string, number>;
  private values: string[];

  constructor(private var_name = "?", freq: Freq = null) {
    this.prob = new Map();
    this.values = [];

    if (freq) {
      const items = Object.entries(freq);
      items.forEach(([v, p]) => {
        if (!this.values.includes(v)) this.values.push(v);
        this.prob.set(v, p);
      });
      this.normalize();
    }
  }

  private normalize(): ProbDist {
    const probValues = this.prob.values();
    const total = [...probValues].reduce((acc, value) => acc + value, 0);

    if (Math.abs(total - 1.0) > Number.EPSILON) {
      for (const [key, value] of this.prob) {
        this.prob.set(key, value / total);
      }
    }

    return this;
  }

  private show_approx(maxDecimalCase = 3) {
    const sortedProbabilities: [string, number][] = Array.from(
      this.prob.entries()
    ).sort(([a], [b]) => (a < b ? -1 : 1));

    const formattedProbabilities: string[] = sortedProbabilities.map(
      ([v, p]) => {
        return `${v}: ${p.toFixed(maxDecimalCase)}`;
      }
    );

    return formattedProbabilities.join(", ");
  }

  show() {
    return `P(${this.var_name})`;
  }
}

export { ProbDist };
