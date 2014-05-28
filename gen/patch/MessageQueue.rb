
c = RBe::Cpp::Class["MessageQueue"]
if c
  # TODO
  %w(IsLocked).each do |s|
    c.functions(s).each{|f| f.set_option :noimp, true }
  end
end
