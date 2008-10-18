def test_case
{"RawParseTree"=>
  [:op_asgn_or,
   [:ivar, :@fetcher],
   [:iasgn,
    :@fetcher,
    [:fcall,
     :new,
     [:array,
      [:call,
       [:call, [:const, :Gem], :configuration],
       :[],
       [:array, [:lit, :http_proxy]]]]]]],
 "Ruby"=>"@fetcher ||= new(Gem.configuration[:http_proxy])",
 "ParseTree"=>
  s(:op_asgn_or,
   s(:ivar, :@fetcher),
   s(:iasgn,
    :@fetcher,
    s(:call,
     nil,
     :new,
     s(:arglist,
      s(:call,
       s(:call, s(:const, :Gem), :configuration, s(:arglist)),
       :[],
       s(:arglist, s(:lit, :http_proxy)))))))}
end
