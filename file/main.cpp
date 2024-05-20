#include <stdio.h>
#include <stdint.h>
unsigned long long factorial(int n) {
  if (n == 0) {
    return 1;
  } else {
    return n * factorial(n - 1);
  }
}

int main() {
  int n = 10;
  int iterations = 100000;

  // Вычисление факториала n iterations раз
  for (int i = 0; i < iterations; ++i) {
    factorial(n);
  }
  // printf("Факториал %d вычислен %d раз.\n", n, iterations);
  // printf("Затраченное время: %.6f секунд\n", elapsed);

  return 0;
}
