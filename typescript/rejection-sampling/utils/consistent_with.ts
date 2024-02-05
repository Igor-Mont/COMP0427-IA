function consistent_with(
  event: Map<string, boolean>,
  evidence: Map<string, boolean>
): boolean {
  return [...event.entries()].every(([k, v]) => (evidence.get(k) ?? v) === v);
}

export { consistent_with };
