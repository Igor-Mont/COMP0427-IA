import { NOde } from "./Node";

interface KeyPair<S, A> {
  key: number;
  value: NOde<S, A>;
}

type ToDouble<S, A> = (node: NOde<S, A>) => number;

class PriorityQueue<S, A> {
  private pq: KeyPair<S, A>[] = [];

  constructor(private items: NOde<S, A>[], private f: ToDouble<S, A>) {
    for (const item of items) {
      this.push(item);
    }
  }

  push(item: NOde<S, A>): void {
    const newElem: KeyPair<S, A> = { key: this.f(item), value: item };
    const insertIndex = this.pq.findIndex((el) => el.key >= newElem.key);

    if (insertIndex === -1) this.pq.push(newElem);
    else this.pq.splice(insertIndex, 0, newElem);
  }

  pop(): NOde<S, A> | undefined {
    const item = this.pq.pop();
    return item ? item.value : undefined;
  }

  top(): NOde<S, A> | undefined {
    return this.pq.length > 0 ? this.pq[0].value : undefined;
  }

  get length(): number {
    return this.pq.length;
  }

  get isEmpty(): boolean {
    return this.pq.length === 0;
  }
}

export { PriorityQueue, KeyPair, ToDouble };
