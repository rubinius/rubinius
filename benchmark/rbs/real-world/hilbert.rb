require 'mathn' # also brings in matrix, rational and complex

# return a Hilbert matrix of given dimension
def hilbert(dimension)
  rows = Array.new
  (1..dimension).each do |i|
    row = Array.new
    (1..dimension).each do |j|
      row.push(1/(i + j - 1))
    end
    rows.push(row)
  end
  return(Matrix.rows(rows))
end
