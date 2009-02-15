def tarai( x, y, z )
  if x <= y
  then y
  else tarai(tarai(x-1, y, z),
             tarai(y-1, z, x),
             tarai(z-1, x, y))
  end
end

Bench.run [3, 4, 5] do |n|
  tarai 12, n, 0
end
