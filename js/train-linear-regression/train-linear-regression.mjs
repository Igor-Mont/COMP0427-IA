
// Função para treinar um modelo de regressão linear usando SGD com regularização L1 e L2
function trainLinearRegression(
  X,
  y,
  learningRate,
  l1Penalty,
  l2Penalty,
  numIterations
) {
  const numFeatures = X[0].length;
  let weights = Array.from({ length: numFeatures + 1 }, () => Math.random()); // Um peso para cada característica mais o termo de interceptação

  for (let iteration = 0; iteration < numIterations; iteration++) {
    for (let sampleIndex = 0; sampleIndex < X.length; sampleIndex++) {
      const x = [1, ...X[sampleIndex]]; // Adiciona o termo de bias
      const actualLabel = y[sampleIndex];

      const prediction = x.reduce(
        (sum, feature, idx) => sum + weights[idx] * feature,
        0
      );

      // Update weights using SGD with L1 and L2 regularization
      for (
        let featureIndex = 0;
        featureIndex < numFeatures + 1;
        featureIndex++
      ) {
        const error = actualLabel - prediction;
        const l1Gradient = l1Penalty * (weights[featureIndex] >= 0 ? 1 : -1);
        const l2Gradient = l2Penalty * weights[featureIndex];

        weights[featureIndex] +=
          learningRate * (error * x[featureIndex] - l1Gradient - l2Gradient);
      }
    }
  }

  return weights;
}

// Exemplo de características escolhidas manualmente
const X = [[3], [4], [3], [2], [5], [4]];

const y = [2, 5, 3, 2, 4, 4]; // Rótulos para cada amostra (0 ou 1)

// Parâmetros do treinamento
const learningRate = 0.01;
const l1Penalty = 0.1;
const l2Penalty = 0.1;
const numIterations = 1000;

// Treina um modelo de regressão linear
const finalWeights = trainLinearRegression(
  X,
  y,
  learningRate,
  l1Penalty,
  l2Penalty,
  numIterations
);
console.log("Final Weights:", finalWeights);
