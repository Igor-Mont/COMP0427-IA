import { BayesNet } from "./BayesNet";
import { ProbDist } from "./ProbDist";
import { consistent_with } from "./utils/consistent_with";
import { prior_sample } from "./utils/prior_sample";

function rejection_sampling(
  X: string,
  e: Map<string, boolean>,
  bn: BayesNet,
  N = 10000
) {
  const variable_values = bn.variable_values(X);
  const counts = new Map<(typeof variable_values)[0], number>();
  variable_values.forEach((variable_value) => {
    counts.set(variable_value, 0);
  });

  for (let j: number = 0; j < N; j++) {
    const sample = prior_sample(bn);

    if (consistent_with(sample, e)) {
      const key = sample.get(X) as boolean;
      const current_value_count = counts.get(key) as number;
      counts.set(key, current_value_count + 1);
    }
  }

  return new ProbDist(X, counts);
}

const evidence = new Map([
  ["JohnCalls", true],
  ["MaryCalls", true],
]);
const burglary = new BayesNet([
  { variable: "Burglary", parents: "", cpt: 0.001 },
  { variable: "Earthquake", parents: "", cpt: 0.002 },
  {
    variable: "Alarm",
    parents: "Burglary Earthquake",
    cpt: new Map([
      [[true, true], 0.95],
      [[true, false], 0.94],
      [[false, true], 0.29],
      [[false, false], 0.001],
    ]),
  },
  {
    variable: "JohnCalls",
    parents: "Alarm",
    cpt: new Map([
      [true, 0.9],
      [false, 0.05],
    ]),
  },
  {
    variable: "MaryCalls",
    parents: "Alarm",
    cpt: new Map([
      [true, 0.7],
      [false, 0.01],
    ]),
  },
]);
console.log(rejection_sampling("Burglary", evidence, burglary).show_approx());

export { rejection_sampling };
