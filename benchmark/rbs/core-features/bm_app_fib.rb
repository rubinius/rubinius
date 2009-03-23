def fib(n)
  if n < 2
    n
  else
    fib(n-1) + fib(n-2)
  end
end

Bench.run [20, 30, 35] do |n|
  fib n
end
