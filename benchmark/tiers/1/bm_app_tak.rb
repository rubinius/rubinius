def tak(x, y, z)
  unless y < x
    z
  else
    tak(tak(x-1, y, z),
         tak(y-1, z, x),
         tak(z-1, x, y))
  end
end


def Bench.run
  tak 18, 9, 0
end
