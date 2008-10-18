def test_case
{"RawParseTree"=>
  [:iter, [:fcall, :a, [:array, [:lit, 1]]], [:dasgn_curr, :c], [:vcall, :d]],
 "Ruby"=>"a (1) {|c|d}",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist, s(:lit, 1))),
   s(:lasgn, :c),
   s(:call, nil, :d, s(:arglist))),
 "Ruby2Ruby"=>"a(1) { |c| d }"}
end
