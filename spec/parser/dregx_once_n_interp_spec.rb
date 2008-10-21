def test_case
{"RawParseTree"=>
  [:dregx_once, "", [:evstr, [:const, :IAC]], [:evstr, [:const, :SB]], 16],
 "Ruby"=>"/\#{IAC}\#{SB}/no",
 "RubyParser"=>
  s(:dregx_once,
   "",
   s(:evstr, s(:const, :IAC)),
   s(:evstr, s(:const, :SB)),
   16),
 "Ruby2Ruby"=>"/\#{IAC}\#{SB}/o"}
end
