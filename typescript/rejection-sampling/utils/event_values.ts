type EventValue = Map<string, boolean> | boolean[];

function event_values(event: EventValue, variables: string[]): boolean[] {
  if (Array.isArray(event) && event.length === variables.length) return event;

  console.log(
    variables.map(
      (variable) => (event as Map<string, boolean>).get(variable) as boolean
    )
  );
  return variables.map(
    (variable) => (event as Map<string, boolean>).get(variable) as boolean
  );
}

export { event_values };
