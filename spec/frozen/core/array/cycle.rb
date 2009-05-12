describe "Enumerable#cycle" do
  ruby_version_is '1.8.7' do
    it "returns nil and does nothing for non positive n or empty arrays" do
      [1,2,3].cycle(0){ throw "ball"}.should be_nil
      [].cycle(0){ throw "ball"}.should be_nil
    end

    it "cycle as many times as requested" do
      [1,2,3].cycle(2).to_a.should == [1,2,3,1,2,3]
    end

    it "loop indefinitely if no n" do
      bomb = 10
      [1,2,3].cycle do
        bomb -= 1
        break 42 if bomb <= 0
      end.should == 42
      bomb.should == 0
    end
  end
end
